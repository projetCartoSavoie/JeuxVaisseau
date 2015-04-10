#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <setjmp.h>
#include "tiny_gc.h"

typedef struct info_cell *info;

struct info_cell {
  info* adr;
  size_t size;
};

static void* stack_bottom;
static size_t stack_direction = 0;

static info references;
static info first_free = 0;
static info last_references = 0;
static info end_references = 0;

#define ALIGN 2 /* 16 bits alignment */
#define MARK 0x1
#define ATOMIC 0x2
#define OLD 04
#define OTHERS (~((size_t)0x0 | MARK | ATOMIC | OLD))

static void* min_alloc;
static void* max_alloc;
 
static size_t allocated;
static size_t live;

static void*** globals;
static void*** last_globals;
static void*** end_globals;

void GC_register_global(void* global) {
  if (last_globals >= end_globals) {
    fprintf(stderr,
	    "Not enough globals, increase max_globals when calling GC_init.\n");
    exit(1);
  }
  *(last_globals++) = (void**) global;
}

int GC_debug = 0;

void set_stack_direction(int *i) {
  int j = 0;
  stack_direction = (&j > i)?1:(-1);
}


void GC_init(int* argc, size_t max_allocation, size_t max_globals) {
  set_stack_direction(argc);
  stack_bottom = argc;
  references = (info)
    mmap(0, sizeof(struct info_cell) * max_allocation, 
	 PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
  globals = (void***)
    mmap(0, sizeof(void**) * max_globals,
	 PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
  assert((size_t)references>0 && (size_t)globals>0);
  last_references = references; 
  end_references = references + max_allocation;
  last_globals = globals; 
  end_globals = globals + max_globals;
  first_free = 0;
  live = 0;
  allocated = 0;
  min_alloc = malloc(1);
  assert(min_alloc);
  max_alloc = min_alloc;
  free(min_alloc);
}

void GC_free2(info ref) {
  if (GC_debug > 1) printf("Free: %ld\n", ref->size >> 4);
  free(ref->adr);
  ref->adr = (info*) first_free;
  ref->size=0;
  first_free = ref;
}

void GC_free(void* ptr) {
  info* ptr1 = (info*)ptr - 1;
  info ref = *ptr1;
  if (ref->adr == ptr1) {
    size_t s = ref->size >> 4;
    if (ref->size & OLD) live -= s; else allocated -= s;
    GC_free2(ref);
  }
}


void Mark(void* ptr) {
  info* ptr1 = ((info*)ptr) - 1;
  if ((void*) ptr1 < min_alloc || (void*) ptr1 > max_alloc) return;
  info ref  = *ptr1;
  if (ref < references || ref >= last_references || ref->adr != ptr1) return;
  size_t info = ref->size;
  if (GC_debug > 1) 
    printf("Mark: %ld (%ld|%ld|%ld)\n", 
	   (size_t)ptr, ref->size >> 4, ref->size & ATOMIC, ref->size & MARK);
  if (info & MARK) return;
  ref->size |= MARK;
  size_t s = info >> 4;
  live += s;
  if (!(info & ATOMIC)) { 
    size_t i;
    for (i=0;i<s;i+=ALIGN) Mark(*(void**)((size_t)ptr+i));
  }
}

void Sweep() {
  info ptr;
  for(ptr = references; ptr < last_references; ptr++) {
    size_t info = ptr->size;
    if (info) {
      if (info & MARK) ptr->size = (ptr->size ^ MARK) | OLD;
      else  GC_free2(ptr);
    }
  }
}

/* To read the top of the stack and resists to compiler optimisations, one     *
 * uses set_jump(buf) which forces to save registers on the stack, where buf   *
 * is a volatile variable as well as the variable in which we write the top of *
 * the stack to make sure the function calls are not inverted. Moreover, one   *
 * should make sure the two variables are considered "used" by the compiler.   *
 * This is achieved by writing these in a volatile and global variables ...    *
 */

volatile void* GC_sink_unused;

void noop(volatile void* ptr) {
 GC_sink_unused = ptr;
}

void* get_stack_top()
{
  volatile void* top;
  top = (void*)(&top);
  noop(top);
  return (void*) top;
}

void GC_collect() {
  size_t oldlive = live, oldallocated = allocated;
  live = 0;
  allocated = 0;
  void **ptr=0,***gptr=0;
  /* The line below emits a warning ... buf can not be casted because it is an *
   * array. And, volatile is mandatory.                                        *
   */
  volatile jmp_buf buf;  setjmp((struct __jmp_buf_tag *)buf);
  volatile void* stack_top = get_stack_top(10);
  if (GC_debug > 0)
    printf("Collect: stack: %ld -> %ld dir : %ld\n", 
	   (size_t)stack_bottom, 
	   stack_direction * ((size_t)stack_top - (size_t)stack_bottom),
	   stack_direction);
  switch(stack_direction) {
  case 1:
    for(ptr = (void**) stack_bottom; ptr < (void**) stack_top; ptr = (void**) ((size_t) ptr + ALIGN))
      Mark(*(void**)ptr);
    break;
  case -1:
    for(ptr = (void**) stack_bottom; ptr > (void**) stack_top; ptr = (void**) ((size_t) ptr - ALIGN))
      Mark(*(void**)ptr);
    break;
  default:
    assert(0);
  }
  noop(buf);
  for(gptr=globals; gptr < last_globals; gptr++)
    Mark(**gptr);
  Sweep();
  if (GC_debug > 0) 
    printf("Collect: allocated: %ld, live: %ld -> %ld\n", oldallocated, oldlive, live);
}


void* GC_malloc(size_t s) {
  if (allocated >  (live * 2)) GC_collect();
  allocated += s;
  info* ptr = (info*) malloc(s + sizeof(info*));
  if (!ptr) return 0;
  if ((void*) ptr < min_alloc) min_alloc = ptr;
  if ((void*) ptr > max_alloc) max_alloc = ptr;
  info ref;
  
  if (first_free) {
    ref = first_free;
    first_free = (info)(ref->adr);
  } else {
    if (last_references >= end_references) {
      fprintf(stderr,
	      "Not enough allocation slots, increase max_allocation when calling GC_init.\n");
      exit(1);
    }
    ref = last_references++;
  }
  
  *ptr = ref;
  ref->adr = ptr++;
  ref->size = (s<<4);  

  return(void*)ptr;
}

void* GC_malloc_atomic(size_t s) {
  if (allocated >  (live + 0x1000)) GC_collect();
  allocated += s;
  info* ptr = (info*) malloc(s + sizeof(info*));
  if (!ptr) return 0;
  if ((void*) ptr < min_alloc) min_alloc = ptr;
  if ((void*) ptr > max_alloc) max_alloc = ptr;
  info ref;
  
  if (first_free) {
    ref = first_free;
    first_free = (info)(ref->adr);
  } else {
    if (last_references >= end_references) {
      fprintf(stderr,
	      "Not enough allocation slots, increase max_allocation when calling GC_init.\n");
      exit(1);
    }
    ref = last_references++;
  }
  
  *ptr = ref;
  ref->adr = ptr++;
  ref->size = (s<<4) | ATOMIC;  

  return(void*)ptr;
}

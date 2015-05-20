#ifndef TINY_GC_H
#define TINY_GC_H

/* Macro USE_BOEHM and USE_NO_GC allow to control what is really used */

#ifdef USE_BOEHM

  #include <gc.h>

#else

#include <stdlib.h>
#ifdef NO_GC
  #define GC_init(argc_adr, max_allocation,  max_globals)
  #define GC_register_global(global)
  #define GC_malloc(x) malloc(x)
  #define GC_malloc_atomic(x) malloc(x)
  #define GC_free(x) free(x)
#else

/******************************************************************************
 * Simplistic Conservative GC                                                 *
 * ==========================                                                 *
 *                                                                            *
 * This code is a trivial stop the world, Mark & Sweep, thread unsafe         *
 * concervative GC. Moreover, global variables must be declared, GC_init()    *
 * must be called with correct arguments (see below) and                      *
 * POINTER IN MEMORY ZONE ALLOCATED BY MALLOC ARE NOT MARKED                  *
 *                                                                            *
 * This means that GC_mallocated block must be accessible from the stack or   *
 * another GC_mallocated non atomic block to be retained by the GC.           *
 ******************************************************************************/

/* global variable to control the debug level */
extern int GC_debug;

/******************************************************************************
 * This function must be called in main, at the beginning with                *
 * - argc_adr : the ardress of argc, the first argument of main (to detect    *
 *   stack bottom)                                                            *
 * - max_allocation : maximum number of lived GC_mallocated object.           *
 * - max_globals : maximum number of global variables to register.            *
 * Nice messages are printed on stderr if the two last parameters are two     *
 * small                                                                      *
 ******************************************************************************/
   
void GC_init(int* argc_adr, size_t max_allocation, size_t max_globals);

/******************************************************************************
 * You must declare all global variable, by giving their adress in argument   *
 * to this function if the global variable may point to GC_mallocated value   *
 ******************************************************************************/
void GC_register_global(void* global);

/******************************************************************************
 * Allocation functions, use the second one if the allocated memory never     *
 * contains any pointer.                                                      *
 ******************************************************************************/
void* GC_malloc(size_t s);
void* GC_malloc_atomic(size_t s);

/******************************************************************************
 * You can still free memory manually                                         *
 ******************************************************************************/
void GC_free(void* ptr);

/******************************************************************************
 * You may also trigger a garbage collection                                  *
 ******************************************************************************/
void GC_collect();

#endif 
#endif
#endif

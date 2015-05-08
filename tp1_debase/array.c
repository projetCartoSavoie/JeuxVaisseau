#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "tiny_gc.h"
#include "array.h"
#define VERTEX gl_vertex
#include "triangulation.h"


gl_vertex* GLvertex2f(float x, float y) {
  gl_vertex* v = (gl_vertex*) GC_malloc(sizeof(gl_vertex));
  point2f p = (point2f) GC_malloc_atomic(sizeof(point2f_cell));

  p->x = x; p->y = y;
  v->vertex_type = GL_FLOAT;
  v->normal_type = 0;
  v->dim = 2;
  v->coord.a2f = p;
  v->normal.raw = NULL;

  return v;
}

gl_vertex* GLvertex3f(float x, float y,float z) {
  gl_vertex* v = (gl_vertex*) GC_malloc(sizeof(gl_vertex));
  point3f p = (point3f) GC_malloc_atomic(sizeof(point3f_cell));

  p->x = x; p->y = y; p->z = z;
  v->vertex_type = GL_FLOAT;
  v->normal_type = 0;
  v->dim = 3;
  v->coord.a3f = p;
  v->normal.raw = NULL;

  return v;
}

gl_vertex* GLvertex2d(double x, double y) {
  gl_vertex* v = (gl_vertex*) GC_malloc(sizeof(gl_vertex));
  point2d p = (point2d) GC_malloc_atomic(sizeof(point2d_cell));

  p->x = x; p->y = y;
  v->vertex_type = GL_DOUBLE;
  v->normal_type = 0;
  v->dim = 2;
  v->coord.a2d = p;
  v->normal.raw = NULL;

  return v;
}

gl_vertex* GLvertex3d(double x, double y,double z) {
  gl_vertex* v = (gl_vertex*) GC_malloc(sizeof(gl_vertex));
  point3d p = (point3d) GC_malloc_atomic(sizeof(point3d_cell));

  p->x = x; p->y = y; p->z = z;
  v->vertex_type = GL_DOUBLE;
  v->normal_type = 0;
  v->dim = 3;
  v->coord.a3d = p;
  v->normal.raw = NULL;

  return v;
}

void GLnormal3f(gl_vertex *v, float x, float y, float z) {
  vecteur3f p = (vecteur3f) GC_malloc_atomic(sizeof(vecteur3f_cell));

  p->x = x; p->y = y; p->z = z;
  v->normal_type = GL_FLOAT;
  v->normal.a3f = p;
}

void GLnormal3d(gl_vertex *v, double x, double y, double z) {
  vecteur3d p = (vecteur3d) GC_malloc_atomic(sizeof(vecteur3d_cell));

  p->x = x; p->y = y; p->z = z;
  v->normal_type = GL_DOUBLE;
  v->normal.a3d = p;
}

void print_vertex(gl_vertex *v) {
  if (v->dim == 2) {
    if (v->vertex_type == GL_DOUBLE) {
      printf("(%lf,%lf)",v->coord.a2d->x,v->coord.a2d->y);
  } else if (v->vertex_type == GL_FLOAT) {
      printf("(%f,%f)",v->coord.a2f->x,v->coord.a2f->y);
    }
  } else {
    if (v->vertex_type == GL_DOUBLE) {
      printf("(%lf,%lf,%lf)",v->coord.a3d->x,v->coord.a3d->y,v->coord.a3d->z);
    } else if (v->vertex_type == GL_FLOAT) {
      printf("(%f,%f,%f)",v->coord.a3f->x,v->coord.a3f->y,v->coord.a3f->z);
    }
  }
}

gl_object* triangulation_to_gl_object(half_edge e) {

  size_t nb_vertices = 0;
  size_t nb_triangles = 0;

  void incr_nbv(half_edge e, gl_vertex* v) { nb_vertices++; }
  iter_vertices(e, incr_nbv);

  void incr_nbt(half_edge e, gl_vertex* v1, gl_vertex* v2, gl_vertex* v3) { nb_triangles++; }
  iter_triangles(e, incr_nbt);

  gl_vertex* v0 = e->vertex;

  gl_object* obj =  (gl_object*) GC_malloc(sizeof(gl_object));
  obj->vertex_type = v0 -> vertex_type;
  obj->normal_type = v0 -> normal_type;
  obj->dim =v0 -> dim;
  obj->nb_vertices = nb_vertices;
  obj->nb_triangles = nb_triangles;

  size_t coord_size = 0;
  size_t normal_size = 0;
  size_t index_size = 0;

  if (nb_vertices < 0x100) {
    obj->index_type = GL_UNSIGNED_BYTE;
    index_size = sizeof(GLubyte);
  } else if (nb_vertices <= 0x10000) {
    obj->index_type = GL_UNSIGNED_SHORT;
    index_size = sizeof(GLushort);
  } else {
    obj->index_type = GL_UNSIGNED_INT;
    index_size = sizeof(GLuint);
  }
  /* copie et indexation des sommets et des normals */

  if (obj->dim == 2) {

    if (obj->vertex_type == GL_FLOAT)
      coord_size = sizeof(point2f_cell);
    else if (obj->vertex_type == GL_DOUBLE)
      coord_size = sizeof(point2d_cell);
    else assert(0);

  } else if (obj->dim == 3) {

    if (obj->vertex_type == GL_FLOAT)
      coord_size = sizeof(point3f_cell);
    else if (obj->vertex_type == GL_DOUBLE)
      coord_size = sizeof(point3d_cell);
    else assert(0);

  } else assert(0);

  if (obj->normal_type == GL_FLOAT)
    normal_size = sizeof(vecteur3f_cell);
  else if (obj->normal_type == GL_DOUBLE)
    normal_size = sizeof(vecteur3d_cell);

  obj->vertices = (union points) GC_malloc_atomic(coord_size * nb_vertices);
  obj->normals = (normal_size == 0)?(union vecteurs) NULL:
    (union vecteurs) GC_malloc_atomic(normal_size * nb_vertices);

  int i = 0;
  void setcn(half_edge e, gl_vertex* v) {
    gl_vertex *w = v;
    w->index = i;
    memcpy(obj->vertices.raw + i * coord_size, w->coord.raw, coord_size);
    if (normal_size != 0)
      memcpy(obj->normals.raw + i * normal_size, w->normal.raw, normal_size);
    i++;
    }
  iter_vertices(e, setcn);

  obj->indices = (union indices) GC_malloc_atomic(index_size * nb_triangles * 3);

  i=0;
  void sett(half_edge e, gl_vertex* v1, gl_vertex* v2, gl_vertex* v3) {
    gl_vertex *w1 = v1, *w2 = v2, *w3 = v3;
    switch(obj->index_type) {
    case GL_UNSIGNED_BYTE:
      obj->indices.ab[i++] = (GLubyte) w1->index;
      obj->indices.ab[i++] = (GLubyte) w2->index;
      obj->indices.ab[i++] = (GLubyte) w3->index;
      break;
    case GL_UNSIGNED_SHORT:
      obj->indices.as[i++] = (GLushort) w1->index;
      obj->indices.as[i++] = (GLushort) w2->index;
      obj->indices.as[i++] = (GLushort) w3->index;
      break;
    case GL_UNSIGNED_INT:
      obj->indices.ai[i++] = (GLuint) w1->index;
      obj->indices.ai[i++] = (GLuint) w2->index;
      obj->indices.ai[i++] = (GLuint) w3->index;
      break;
    }
  }
  iter_triangles(e, sett);

  return(obj);

}

gl_object* triangulation_poly_to_gl_object(half_edge e) {

    printf("debut triangulation\n");
  size_t nb_triangles = 0;

  void incr_nbt(half_edge e, gl_vertex* v1, gl_vertex* v2, gl_vertex* v3) { nb_triangles++; }
  iter_triangles(e, incr_nbt);
  
  printf("%d triangles \n", nb_triangles);

  size_t nb_vertices = nb_triangles * 3;

  gl_vertex* v0 = e->vertex;
  
  printf("v0 = e->vertex OK \n");

  gl_object* obj =  (gl_object*) GC_malloc(sizeof(gl_object));
  obj->vertex_type = v0 -> vertex_type;
  obj->normal_type = v0 -> vertex_type;
  obj->dim =v0 -> dim;
  obj->nb_vertices = nb_vertices;
  obj->nb_triangles = nb_triangles;

  size_t coord_size = 0;
  size_t index_size = 0;

  if (nb_vertices < 0x100) {
    obj->index_type = GL_UNSIGNED_BYTE;
    index_size = sizeof(GLubyte);
  } else if (nb_vertices <= 0x10000) {
    obj->index_type = GL_UNSIGNED_SHORT;
    index_size = sizeof(GLushort);
  } else {
    obj->index_type = GL_UNSIGNED_INT;
    index_size = sizeof(GLuint);
  }

  if (obj->dim == 3) {
    if (obj->vertex_type == GL_FLOAT)
      coord_size = sizeof(point3f_cell);
    else if (obj->vertex_type == GL_DOUBLE)
      coord_size = sizeof(point3d_cell);
    else assert(0);
  } else assert(0);

  obj->vertices = (union points)   GC_malloc_atomic(coord_size * nb_vertices);
  obj->normals  = (union vecteurs) GC_malloc_atomic(coord_size * nb_vertices);
  obj->indices  = (union indices)  GC_malloc_atomic(index_size * nb_triangles * 3);

  size_t i=0;

  union vecteurs normal(gl_vertex* w1, gl_vertex* w2, gl_vertex* w3) {
    union vecteurs N0;

    if (obj->normal_type == GL_DOUBLE) {
       
      point3d A=w1->coord.a3d, B=w2->coord.a3d, C=w3->coord.a3d;
      vecteur3d_cell AB, AC;
      vecteur3d N = N0.a3d = (vecteur3d) GC_malloc(sizeof(vecteur3d_cell));

      vec3d(&AB,A,B);
      vec3d(&AC,A,C);
      vec_prod3d(N,&AB,&AC);
      normalize3d(N);
    } else if (obj->normal_type == GL_FLOAT) {
      point3f A=w1->coord.a3f, B=w2->coord.a3f, C=w3->coord.a3f;
      vecteur3f_cell AB, AC;
      vecteur3f N = N0.a3f = (vecteur3f) GC_malloc(sizeof(vecteur3f_cell));

      vec3f(&AB,A,B);
      vec3f(&AC,A,C);
      vec_prod3f(N,&AB,&AC);
      normalize3f(N);
     } else assert(0);

    return(N0);
  }

  void sett(half_edge e, gl_vertex* v1, gl_vertex* v2, gl_vertex* v3) {
      printf("sett\n");
    gl_vertex *w1 = v1, *w2 = v2, *w3 = v3;

    printf("def gl_vertex\n");
    union vecteurs N = normal(w1, w2, w3);

    printf("normal\n");
    memcpy(obj->vertices.raw + i * coord_size, w1->coord.raw, coord_size);
    memcpy(obj->vertices.raw + (i+1) * coord_size, w2->coord.raw, coord_size);
    memcpy(obj->vertices.raw + (i+2) * coord_size, w3->coord.raw, coord_size);

    memcpy(obj->normals.raw + i * coord_size, N.raw, coord_size);
    memcpy(obj->normals.raw + (i+1) * coord_size, N.raw, coord_size);
    memcpy(obj->normals.raw + (i+2) * coord_size, N.raw, coord_size);

    printf("mem_copy\n");
    
    GC_free(N.raw);
    
    printf("gc_free\n");

    switch(obj->index_type) {
    case GL_UNSIGNED_BYTE:
      obj->indices.ab[i] = (GLubyte) i;
      obj->indices.ab[i+1] = (GLubyte) i+1;
      obj->indices.ab[i+2] = (GLubyte) i+2;
      break;
    case GL_UNSIGNED_SHORT:
      obj->indices.as[i] = (GLushort) i;
      obj->indices.as[i+1] = (GLushort) i+1;
      obj->indices.as[i+2] = (GLushort) i+2;
      break;
    case GL_UNSIGNED_INT:
      obj->indices.ai[i] = (GLuint) i;
      obj->indices.ai[i+1] = (GLuint) i+1;
      obj->indices.ai[i+2] = (GLuint) i+2;

      break;
    }

    i += 3;
  }

  iter_triangles(e, sett);

  printf("fin triangulation\n");

  return(obj);

}

void glDrawObject(gl_object *obj) {

  glEnableClientState(GL_VERTEX_ARRAY);
  if (obj->normal_type) glEnableClientState(GL_NORMAL_ARRAY);

  /* On indique le pointeur sur le tableau des vextex */
  /* Il faut indiquer que chaque point a trois coordonnées */
  /* et préciser le type. Le troisième argument est là pour
     l'alignement... */
  glVertexPointer(3, obj->vertex_type, 0, obj->vertices.raw);

  /* On indique le pointeur sur le tableau des normales */
  /* Toujours trois coordonnées dans ce cas */
  if (obj->normal_type)
    glNormalPointer(obj->normal_type, 0, obj->normals.raw);

  /* On demande le dessin, ces là que l'on donne le nombre */
  /* de sommets, le type des indices et le pointeur le sur */
  /* le tableau d'indice */
  glDrawElements(GL_TRIANGLES, obj->nb_triangles * 3, obj->index_type,
		 obj->indices.raw);

  /* On déactive les deux sortes d'array */
  glDisableClientState(GL_VERTEX_ARRAY);
  if (obj->normal_type) glDisableClientState(GL_NORMAL_ARRAY);
}

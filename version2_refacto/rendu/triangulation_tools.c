#include <assert.h>
#include "triangulation_tools.h"

/* Construction d'un tétraèdre */

half_edge tetraedron(gl_vertex *v1, gl_vertex *v2, gl_vertex *v3, gl_vertex *v4) {
  half_edge e12 = create_triangle(v1, v2, v3);
  half_edge e21 = e12 -> opp;
  half_edge e13 = e12 -> next;
  half_edge e31 = e13 -> opp;
  half_edge e32 = e31 -> next;
  add_vertex_to_edge(e21,v4);
  half_edge e24 = e21 -> next;
  half_edge e42 = e24 -> opp;
  half_edge e41 = e42 -> next;
  close_triangle(e32,e24);
  half_edge e34 = e32 -> next;
  fill_triangle(e13,e34,e41);

  return e12;
}

/* fonction à itérer sur les sommets, donc v = e -> vertex
   décore v avec la normal égale à la moyen des produits 
   vectoriel des faces. Marche uniquement en dim 3 avec
   des coordonnées flottantes */
static void set_average_normal3f(half_edge e, gl_vertex* v) {
  point3f A,B,C;
  vecteur3f_cell AB, AC, V;
  A = v->coord.a3f;
  /* initialize la normale à zéro */
  GLnormal3f(v,0.0,0.0,0.0);
  
  /* on recule avec pred jusqu'à arriver au bord ou
     à faire un tour, pour être sur de regarder toutes
     les faces, même au bord */ 
  half_edge e0 = e;
  while(e0->prev && e0->prev != e) e0 = e0->prev;
    

  half_edge e1 = e0, e2 = e1 -> next;
  unsigned char first = 1;

  /* boucle parcourant toutes les faces autour de e et 
     additionnant tous les produits vectoriels des faces */
  while(e1 && e2 && (first || e1 != e0)) {
    B = e1->opp->vertex->coord.a3f;
    C = e2->opp->vertex->coord.a3f;
    vec3f(&AB,A,B);
    vec3f(&AC,A,C);
    vec_prod3f(&V,&AB,&AC);
    add3f(e->vertex->normal.a3f,&V);
    first = 0;
    e1 = e2;
    e2 = e1 -> next;
  }
  
  /* finalement, on normalize la normale */
  normalize3f(e->vertex->normal.a3f);
}

/* Similaire à la précédente avec des doubles */
static void set_average_normal3d(half_edge e, gl_vertex* v) {
  point3d A,B,C;
  vecteur3d_cell AB, AC, V;
  A = v->coord.a3d;
  GLnormal3d(v,0.0,0.0,0.0);
  
  half_edge e0 = e;
  
  while(e0->prev && e0->prev != e) e0 = e0->prev;
    
  half_edge e1 = e0, e2 = e1 -> next;

  unsigned char first = 1;

  while(e1 && e2 && (first || e1 != e0)) {
    B = e1->opp->vertex->coord.a3d;
    C = e2->opp->vertex->coord.a3d;
    vec3d(&AB,A,B);
    vec3d(&AC,A,C);
    vec_prod3d(&V,&AB,&AC);
    add3d(e->vertex->normal.a3d,&V);
    first = 0;
    e1 = e2;
    e2 = e1 -> next;
  }
  
  normalize3d(e->vertex->normal.a3d);
}

/* on fixe maitenant toutes les normales */
void set_average_normal(half_edge e) {

  assert(e -> vertex -> dim == 3);

  if (e -> vertex -> vertex_type == GL_FLOAT)
    iter_vertices(e, set_average_normal3f);
  else if ( e -> vertex -> vertex_type == GL_DOUBLE)
    iter_vertices(e, set_average_normal3d);
  else assert(0);
}
  

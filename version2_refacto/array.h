#ifndef ARRAY_H
#define ARRAY_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include "geometry.h"

/* union pour les 4 types de points, utile en OpenGL,
   pour partager du code puisqu'OpenGL prends des
   tableaux de nombres comme void* puis une valeur
   indiquant le type.

   cf man de GLVertexPointer et GLNormalPointer*/

union points {
  point2d a2d;
  point2f a2f;
  point3d a3d;
  point3f a3f;
  void* raw;
};

/* idem pour les vecteurs, dim3 uniquement, car
   on les utilisent pour les normales et les normales
   en dim2 sont rares */
union vecteurs {
  vecteur3d a3d;
  vecteur3f a3f;
  void* raw;
};

/* Structure pour représenter un sommet avec les infos
   qu'il faut pour OpenGL */
typedef struct {
  GLenum vertex_type; /* GL_FLOAT, GL_DOUBLE */
  GLenum normal_type; /* GL_FLOAT, GL_DOUBLE
                         ou 0 (pas de normale)*/
  unsigned char dim;
  size_t index; /* pour numéroter, utiliser par
                   triangulation_to_gl_object */

  union points coord;
  union vecteurs normal;

} gl_vertex;

#define VERTEX gl_vertex
#include "triangulation.h"

/* creation de la structure ci-dessus, sans normale */
gl_vertex* GLvertex2f(float x, float y);
gl_vertex* GLvertex3f(float x, float y,float z);
gl_vertex* GLvertex2d(double x, double y);
gl_vertex* GLvertex3d(double x, double y,double z);

/* affichage */
void print_vertex(gl_vertex *v);

/* ajout de la normale aux structure ci-dessus */
void GLnormal3f(gl_vertex *v, float x, float y, float z);
void GLnormal3d(gl_vertex *v, double x, double y, double z);

/* structure pour stocker une triangulation avec les
   informations pour openGL */
typedef struct {
  GLenum vertex_type; /* GL_FLOAT, GL_DOUBLE */
  GLenum normal_type; /* GL_FLOAT, GL_DOUBLE ou 0 */
  GLenum index_type; /* GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT,
                         GL_UNSIGNED_INT */
  unsigned char dim; /* Dimension des points, les normales
			c'est toujours 3 */
  size_t nb_vertices;
  size_t nb_triangles;

  /* tableaux des points et vecteurs de taille nb_vertices */
  union points vertices;
  union vecteurs normals;

  /* tableaux des indices une union pour les trois cas */
  union indices {
    GLubyte *ab;
    GLushort *as;
    GLuint *ai;
    void* raw;
  } indices;

} gl_object;

/* Transforme une triangulation en gl_object, sans normale */
gl_object* triangulation_to_gl_object(half_edge e);

/* Transforme une triangulation en gl_object, avec les
   normales des sommets = aux normales des faces (et donc
   les sommets sont copiés une fois par face) */
gl_object* triangulation_poly_to_gl_object(half_edge e);

/* envoi de l'objet à OpenGL */
void glDrawObject(gl_object *obj);

#endif

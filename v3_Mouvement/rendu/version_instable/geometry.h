#ifndef GEOMETRY_H
#define GEOMETRY_H

/* Géometrie sur les points et les vecteur */

/* Type des points en dimension 2 et 3, double et float
   - le suffixe 3d indique "dimension 3 et double".
   - le suffixe 2d indique "dimension 2 et double".
   - le suffixe 3f indique "dimension 3 et float".
   - le suffixe 2f indique "dimension 2 et float".
  
   On distingue points et vecteurs pour détecter des erreurs. */

/* les structures de données, à chaque fois, 
   une structure xxx_cell et un type de pointeur 
   dessus */

/* Remarque importante : un vecteur3d peut aussi représenter
   un tableau de vecteur3d_cell, avec toutes les coordonnées 
   à plat (sans pointeur/indirection supplémentaire). C'est
   la structure que veut OpenGL */

typedef struct {
  double x;
  double y;
  double z;
} vecteur3d_cell;
typedef vecteur3d_cell* vecteur3d;

typedef struct {
  double x;
  double y;
  double z;
} point3d_cell;
typedef point3d_cell* point3d;

typedef struct {
  float x;
  float y;
  float z;
} vecteur3f_cell;
typedef vecteur3f_cell* vecteur3f;

typedef struct {
  float x;
  float y;
  float z;
} point3f_cell;
typedef point3f_cell* point3f;

typedef struct {
  double x;
  double y;
} vecteur2d_cell;
typedef vecteur2d_cell* vecteur2d;

typedef struct {
  double x;
  double y;
} point2d_cell;
typedef point2d_cell* point2d;

typedef struct {
  float x;
  float y;
} vecteur2f_cell;
typedef vecteur2f_cell* vecteur2f;

typedef struct {
  float x;
  float y;
} point2f_cell;
typedef point2f_cell* point2f;

/* Mise à zéro d'un vecteur */
void zero3d(vecteur3d A);
void zero3f(vecteur3f A);
void zero2d(vecteur2d A);
void zero2f(vecteur2f A);

void cp_point3d(point3d R, const point3d D);
void cp_vecteur3d(vecteur3d R, const vecteur3d D);

/* Addition de vecteurs, résultat dans le premier argument */
void add3d(vecteur3d A, const vecteur3d B);
void add3f(vecteur3f A, const vecteur3f B);
void add2d(vecteur2d A, const vecteur2d B);
void add2f(vecteur2f A, const vecteur2f B);

/* Écrit dans R le vecteur AB */ 
void vec3d(vecteur3d R, const point3d A, const point3d B);
void vec3f(vecteur3f R, const point3f A, const point3f B);
void vec2d(vecteur2d R, const point2d A, const point2d B);
void vec2f(vecteur2f R, const point2f A, const point2f B);

/* Translation d'un point par un vecteur multiplié par un scalaire */
void translate3d(point3d A, double u, const vecteur3d V);
void translate3f(point3f A, double u, const vecteur3f V);
void translate2d(point2d A, double u, const vecteur2d V);
void translate2f(point2f A, double u, const vecteur2f V);

/* Barycentre de deux ou trois points */
void bary3d(point3d R, double u, point3d A, double v, const point3d B);
void bary3d3points(point3d R, double u, point3d A, double v, const point3d B, double w, const point3d C);
void moyenne3d3vecteurs(vecteur3d R, double u, vecteur3d A, double v, const vecteur3d B, double w, const vecteur3d C);
void bary3f(point3f R, double u, point3f A, double v, const point3f B);
void bary2d(point2d R, double u, point2d A, double v, const point2d B);
void bary2f(point2f R, double u, point2f A, double v, const point2f B);

/* produit scalaire */
double dot_prod3d(const vecteur3d A, const vecteur3d B);
float dot_prod3f(const vecteur3f A, const vecteur3f B);
double dot_prod2d(const vecteur2d A, const vecteur2d B);
float dot_prod2f(const vecteur2f A, const vecteur2f B);

/* produit par un scalaire (change le vecteur) */
void scal_prod3d(vecteur3d A, double l);
void scal_prod3f(vecteur3f A, float l);
void scal_prod2d(vecteur2d A, double l);
void scal_prod2f(vecteur2f A, float l);

/* changement de repere */
void chge_rep3d(vecteur3d Res, double u, const vecteur3d I, double v, const vecteur3d J, double w, const vecteur3d K);
void trouve_extremite(point3d Res, const vecteur3d V, const point3d O);

/* Multiplication d'un point par une matrice */
void mul_pt3d_matrice(point3d Res, double (*rotateMatrice)[3]);

/* carrée de la norme (norm2_xxx), norme (norm_xxx) 
   et normalisation (division par la norme */
double norm2_3d(const vecteur3d A);
double norm3d(const vecteur3d A);
void normalize3d(vecteur3d A);

float norm2_3f(const vecteur3f A);
float norm3f(const vecteur3f A);
void normalize3f(vecteur3f A);

double norm2_2d(const vecteur2d A);
double norm2d(const vecteur2d A);
void normalize2d(vecteur2d A);

float norm2_2f(const vecteur2f A);
float norm2f(const vecteur2f A);
void normalize2f(vecteur2f A);

/* product vectoriel */
void vec_prod3d(vecteur3d R, const vecteur3d A, const vecteur3d B);
void vec_prod3f(vecteur3f R, const vecteur3f A, const vecteur3f B);

/* déterminant */
double det3d(const vecteur3d A, const vecteur3d B, const vecteur3d C);
float det3f(const vecteur3f A, const vecteur3f B, const vecteur3f C);
double det2d(const vecteur2d A, const vecteur2d B);
float det2f(const vecteur2f A, const vecteur2f B);

#endif

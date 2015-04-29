#include <math.h>
#include "geometry.h"

void zero3d(vecteur3d A) {
  A->x = 0.0; A->y = 0.0; A->z = 0.0;
}
void zero3f(vecteur3f A) {
  A->x = 0.0; A->y = 0.0; A->z = 0.0;
}
void zero2d(vecteur2d A) {
  A->x = 0.0; A->y = 0.0 ;
}
void zero2f(vecteur2f A) {
  A->x = 0.0; A->y = 0.0;
}

void add3d(vecteur3d A, const vecteur3d B) {
  A->x += B->x;
  A->y += B->y;
  A->z += B->z;
}

void add3f(vecteur3f A, const vecteur3f B) {
  A->x += B->x;
  A->y += B->y;
  A->z += B->z;
}

void add2d(vecteur2d A, const vecteur2d B) {
  A->x += B->x;
  A->y += B->y;
}

void add2f(vecteur2f A, const vecteur2f B) {
  A->x += B->x;
  A->y += B->y;
}

void vec3d(vecteur3d R, const point3d A, const point3d B) {
  R->x = B->x - A->x;
  R->y = B->y - A->y;
  R->z = B->z - A->z;
}
void vec3f(vecteur3f R, const point3f A, const point3f B) {
  R->x = B->x - A->x;
  R->y = B->y - A->y;
  R->z = B->z - A->z;
}
void vec2d(vecteur2d R, const point2d A, const point2d B) {
  R->x = B->x - A->x;
  R->y = B->y - A->y;
}
void vec2f(vecteur2f R, const point2f A, const point2f B) {
  R->x = B->x - A->x;
  R->y = B->y - A->y;
}
 
void bary3d(point3d R, double u, point3d A, double v, const point3d B) {
  R->x = u * A->x + v * B -> x;
  R->y = u * A->y + v * B -> y;
  R->z = u * A->z + v * B -> z;
}

void bary3d3points(point3d R, double u, point3d A, double v, const point3d B, double w, const point3d C) {
  R->x = u * A->x + v * B -> x + w * C -> x;
  R->y = u * A->y + v * B -> y + w * C -> y;
  R->z = u * A->z + v * B -> z + w * C -> z;
}

void moyenne3d3vecteurs(vecteur3d R, double u, vecteur3d A, double v, const vecteur3d B, double w, const vecteur3d C) {
  R->x = u * A->x + v * B -> x + w * C -> x;
  R->y = u * A->y + v * B -> y + w * C -> y;
  R->z = u * A->z + v * B -> z + w * C -> z;
}

void bary3f(point3f R, double u, point3f A, double v, const point3f B) {
  R->x = u * A->x + v * B -> x;
  R->y = u * A->y + v * B -> y;
  R->z = u * A->z + v * B -> z;
}

void bary2d(point2d R, double u, point2d A, double v, const point2d B) {
  R->x = u * A->x + v * B -> x;
  R->y = u * A->y + v * B -> y;
}
void bary2f(point2f R, double u, point2f A, double v, const point2f B) {
  R->x = u * A->x + v * B -> x;
  R->y = u * A->y + v * B -> y;
}

void translate3d(point3d A, double u, const vecteur3d V) {
  bary3d(A, 1.0, A, u, (point3d) V);
}
void translate3f(point3f A, double u, const vecteur3f V) {
  bary3f(A, 1.0, A, u, (point3f) V);
}
void translate2d(point2d A, double u, const vecteur2d V) {
  bary2d(A, 1.0, A, u, (point2d) V);
}
void translate2f(point2f A, double u, const vecteur2f V) {
  bary2f(A, 1.0, A, u, (point2f) V);
}


double dot_prod3d(const vecteur3d A, const vecteur3d B) {
  return(A->x * B->x + A->y * B->y + A->z * B->z);
}
float dot_prod3f(const vecteur3f A, const vecteur3f B) {
  return(A->x * B->x + A->y * B->y + A->z * B->z);
}
double dot_prod2d(const vecteur2d A, const vecteur2d B) {
  return(A->x * B->x + A->y * B->y);
}
float dot_prod2f(const vecteur2f A, const vecteur2f B) {
  return(A->x * B->x + A->y * B->y);
}

void scal_prod3d(vecteur3d A, double l) {
  A->x *= l; A->y *= l; A->z *= l;
}
void scal_prod3f(vecteur3f A, float l) {
  A->x *= l; A->y *= l; A->z *= l;
}
void scal_prod2d(vecteur2d A, double l) {
  A->x *= l; A->y *= l;
}
void scal_prod2f(vecteur2f A, float l) {
  A->x *= l; A->y *= l;
}

double norm2_3d(const vecteur3d A) {
  return(A->x * A->x + A->y * A->y + A->z * A->z);
}
double norm3d(const vecteur3d A) {
  return(sqrt(norm2_3d(A)));
}
void normalize3d(vecteur3d A) {
  double n = norm3d(A);
  scal_prod3d(A,1.0/n);
}
float norm2_3f(const vecteur3f A) {
  return(A->x * A->x + A->y * A->y + A->z * A->z);
}
float norm3f(const vecteur3f A) {
  return(sqrt(norm2_3f(A)));
}
void normalize3f(vecteur3f A) {
  float n = norm3f(A);
  scal_prod3f(A,1.0/n);
}
double norm2_2d(const vecteur2d A) {
  return(A->x * A->x + A->y * A->y);
}
double norm2d(const vecteur2d A) {
  return(sqrt(norm2_2d(A)));
}
void normalize2d(vecteur2d A) {
  double n = norm2d(A);
  scal_prod2d(A,1.0/n);
}
float norm2_2f(const vecteur2f A) {
  return(A->x * A->x + A->y * A->y);
}
float norm2f(const vecteur2f A) {
  return(sqrt(norm2_2f(A)));
}
void normalize2f(vecteur2f A) {
  float n = norm2f(A);
  scal_prod2f(A,1.0/n);
}

void vec_prod3d(vecteur3d R, const vecteur3d A, const vecteur3d B) {
  R->x =   A->y * B->z - A->z * B->y;
  R->y = - A->x * B->z + A->z * B->x;
  R->z =   A->x * B->y - A->y * B->x;
}
void vec_prod3f(vecteur3f R, const vecteur3f A, const vecteur3f B) {
  R->x =   A->y * B->z - A->z * B->y;
  R->y = - A->x * B->z + A->z * B->x;
  R->z =   A->x * B->y - A->y * B->x;
}

double det3d(const vecteur3d A, const vecteur3d B, const vecteur3d C) {
  return(  A->x*(  B->y * C->z - B->z * C-> y)
	   - A->y*(  B->x * C->z - B->z * C-> x)
	   + A->z*(  B->x * C->y - B->y * C-> x));
}

float det3f(const vecteur3f A, const vecteur3f B, const vecteur3f C) {
  return(  A->x*(  B->y * C->z - B->z * C-> y)
	   - A->y*(  B->x * C->z - B->z * C-> x)
	   + A->z*(  B->x * C->y - B->y * C-> x));
}

double det2d(const vecteur2d A, const vecteur2d B) {
  return(  A->x*B->y - A->y*B->x );
}
float det2f(const vecteur2f A, const vecteur2f B) {
  return(  A->x*B->y - A->y*B->x );
}


#ifndef TESTS_H
#define TESTS_H

#include "triangulation.h"
#include "array.h"

typedef struct repere_cell* repere;
typedef struct repere_cell repere_cell;
struct repere_cell {
    point3d C;
    vecteur3d V;
    vecteur3d I;
    vecteur3d J;
    gl_vertex** PointsAutour;
};

void cp_point3d(point3d R, const point3d D);
void cp_vecteur3d(vecteur3d R, const vecteur3d D);

half_edge testTubeEntier(int nbPoints, repere Rep, const point3d D, const point3d A, double R, int precision);
half_edge creerVaisseau();


#endif

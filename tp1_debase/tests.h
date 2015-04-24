#ifndef TESTS_H
#define TESTS_H

#include "triangulation.h"
#include "array.h"

half_edge test_tetra();
half_edge test_cylindre(point3d D, point3d A, double R, int precision);
void testSqueletteDuTube(point3d resultats, int nbPoints, const point3d D , const point3d A , double R, int precision);


#endif

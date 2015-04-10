#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "geometry.h"
#include "array.h"
#include "triangulation_tools.h"
#include "tiny_gc.h"
#include "tests.h"
#define FALSE = 0
#define TRUE = 1

half_edge test_tetra() {
    size_t n = 4;
    gl_vertex *v = GC_malloc(n * sizeof (gl_vertex));

    v[0] = *(GLvertex3d(-1.0, -1.0, -1.0));
    v[1] = *(GLvertex3d(1.0, 0.0, 0.0));
    v[2] = *(GLvertex3d(0.0, 1.0, 0.0));
    v[3] = *(GLvertex3d(0.0, 0.0, 1.0));

    return tetraedron(&v[1], &v[0], &v[2], &v[3]);
}

void cp_point3d(point3d R, const point3d D) {
    R -> x = D -> x;
    R -> y = D -> y;
    R -> z = D -> z;
}

half_edge test_cylindre(point3d D, point3d A, double R, int precision) {

    //Trouver N
    vecteur3d N = GC_malloc(sizeof (vecteur3d));
    vec3d(N, D, A);

    assert(!(N -> x == 0 && N -> y == 0 && N -> z == 0));

    //Trouver J
    vecteur3d J = GC_malloc(sizeof (vecteur3d));
    if (abs(N -> z) < abs(N -> x) && abs(N -> z) < abs(N -> y)) {
        J -> x = N -> y;
        J -> y = -(N -> x);
        J -> z = 0;
    } else if (abs(N -> x) < abs(N -> y)) {
        J -> x = 0;
        J -> y = N -> z;
        J -> z = -(N -> y);
    } else {
        J -> x = N -> z;
        J -> y = 0;
        J -> z = -(N -> x);
    }
    normalize3d(J);

    //Trouver I = N^J
    vecteur3d I = GC_malloc(sizeof (vecteur3d));
    vec_prod3d(I, N, J);
    normalize3d(I);

    //P0 = D+RI //Pk = D + Rcos(kt)I + Rsin(kt)J
    point3d P = GC_malloc(precision * sizeof (point3d));
    point3d Q = GC_malloc(precision * sizeof (point3d));

    int k;
    double t = 2 * M_PI / precision;
    for (k = 0; k < precision; k++) {
        //P[k] = D + R*cos(t*k)I + R*sin(t*k)J
        //Qk = Translate Pk de vecteur DA
        cp_point3d(&P[k], D);

        translate3d(&P[k], R * cos(t * k), I);
        translate3d(&P[k], R * sin(t * k), J);

        cp_point3d(&Q[k], &P[k]);
        translate3d(&Q[k], 1.0, N);
    }

    gl_vertex** GP = GC_malloc(precision * sizeof (gl_vertex*));
    gl_vertex** GQ = GC_malloc(precision * sizeof (gl_vertex*));

    for (k = 0; k < precision; k++) {
        GP[k] = GLvertex3d(P[k].x, P[k].y, P[k].z);
        GQ[k] = GLvertex3d(Q[k].x, Q[k].y, Q[k].z);
    }

    //Triangles PkQkQ(k+1) et PkQ(k+1)P(k+1)
    //Premier cas
    half_edge e1 = create_triangle(GP[0], GQ[0], GQ[1]);
    add_vertex_to_edge(e1 -> next, GP[1]);
    half_edge epred = e1 -> opp -> prev -> opp;

    //Boucle
    for (k = 1; k < precision - 1; k++) {
        add_vertex_to_edge(epred, GQ[k + 1]);
        add_vertex_to_edge(epred -> next, GP[k + 1]);
        epred = epred -> opp -> prev -> opp;
    }

    //Dernier cas
    /*half_edge e2 = close_triangle(e1 -> opp,epred, );
    e2 = fill_triangle(GP[precision - 1], GQ[0], GP[0]);*/

    //Rendre le premier half_edge
    return e1 -> opp -> prev;
}
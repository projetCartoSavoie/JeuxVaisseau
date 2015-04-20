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
    vecteur3d N = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vec3d(N, D, A);

    assert(!(N -> x == 0 && N -> y == 0 && N -> z == 0));

    //Trouver J
    vecteur3d J = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
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
    vecteur3d I = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vec_prod3d(I, N, J);
    normalize3d(I);
    
    //Ici N est encore défini
    printf("N : %f %f %f \n", N -> x, N -> y, N -> z );

    //P0 = D+RI //Pk = D + Rcos(kt)I + Rsin(kt)J
    point3d P = (point3d) GC_malloc(precision * sizeof (point3d_cell));
    point3d Q = (point3d) GC_malloc(precision * sizeof (point3d_cell));
    
    //Ici N ne l'est plus si utilisation GG_BOEHM
    printf("N : %f %f %f \n", N -> x, N -> y, N -> z );

    int k;
    double t = 2 * M_PI / precision;
    for (k = 0; k < precision; k++) {
        
        //P[k] = D + R*cos(t*k)I + R*sin(t*k)J
        //Qk = Translate Pk de vecteur DA
        cp_point3d(&P[k], D);
        
        translate3d(&P[k], R * cos(t * k), I);
        translate3d(&P[k], R * sin(t * k), J);
        
        cp_point3d(&Q[k], &P[k]);
        //translate3d(&Q[k], 1.0, N);
        Q[k].x = Q[k].x + N -> x;
        Q[k].y = Q[k].y + N -> y;
        Q[k].z = Q[k].z + N -> z;
        
    }

    gl_vertex** GP = (gl_vertex**) GC_malloc(precision * sizeof (gl_vertex*));
    gl_vertex** GQ = (gl_vertex**) GC_malloc(precision * sizeof (gl_vertex*));

    for (k = 0; k < precision; k++) {
        GP[k] = GLvertex3d(P[k].x, P[k].y, P[k].z);
        GQ[k] = GLvertex3d(Q[k].x, Q[k].y, Q[k].z);
    }

    //Triangles PkQkQ(k+1) et PkQ(k+1)P(k+1)
    //Premier cas (OK)
    half_edge e1 = create_triangle(GP[0], GQ[0], GQ[1]);
    add_vertex_to_edge(e1 -> next, GP[1]);
    half_edge epred = e1 -> next -> next -> opp -> next;

    //Boucle (pas ok : un seul triangle)
    for (k = 1; k < precision - 1; k++) {
        add_vertex_to_edge(epred, GQ[k + 1]);
        add_vertex_to_edge(epred -> next, GP[k + 1]);
        epred = epred -> next -> next -> opp -> next;
    }

    //Dernier cas
    half_edge e2 = create_edge(GQ[precision - 1], GQ[0]);
    close_triangle(epred, e2);
    half_edge e3 = create_edge(GP[0],GP[precision -1]);
    fill_triangle(epred -> next, e1 -> opp, e3);

    //Rendre le premier half_edge
    return e1 -> opp -> prev;
}
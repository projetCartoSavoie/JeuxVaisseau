#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "geometry.h"
#include "array.h"
#include "triangulation_tools.h"
#include "triangulation.h"
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

//Pour faire la constante d'Euler à la fin
unsigned int numfaces = 0;
unsigned int numsommets = 0;
unsigned int numedges = 0;

void fnfaces(half_edge e, VERTEX* v1, VERTEX* v2, VERTEX* v3) {
    numfaces++;
}

void fnsommets(half_edge e, VERTEX* v1) {
    numsommets++;
}

void fnedges(half_edge e, VERTEX* v1, VERTEX* v2) {
    numedges++;
}
//

half_edge test_cylindre(point3d D, point3d A, double R, int precision) {
    
    //Trouver N
    vecteur3d V = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vec3d(V, D, A);

    assert(!(V -> x == 0 && V -> y == 0 && V -> z == 0));

    //Trouver J
    vecteur3d J = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    if (abs(V -> z) < abs(V -> x) && abs(V -> z) < abs(V -> y)) {
        J -> x = V -> y;
        J -> y = -(V -> x);
        J -> z = 0;
    } else if (abs(V -> x) < abs(V -> y)) {
        J -> x = 0;
        J -> y = V -> z;
        J -> z = -(V -> y);
    } else {
        J -> x = V -> z;
        J -> y = 0;
        J -> z = -(V -> x);
    }
    normalize3d(J);

    //Trouver I = N^J
    vecteur3d I = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vec_prod3d(I, V, J);
    normalize3d(I);

    //P0 = D+RI //Pk = D + Rcos(kt)I + Rsin(kt)J
    point3d P = (point3d) GC_malloc(precision * sizeof (point3d_cell));
    point3d Q = (point3d) GC_malloc(precision * sizeof (point3d_cell));

    int k;
    double t = 2 * M_PI / precision;
    for (k = 0; k < precision; k++) {

        //P[k] = D + R*cos(t*k)I + R*sin(t*k)J
        //Qk = Translate Pk de vecteur DA
        cp_point3d(&P[k], D);

        translate3d(&P[k], R * cos(t * k), I);
        translate3d(&P[k], R * sin(t * k), J);
        
        cp_point3d(&Q[k], A);

        translate3d(&Q[k], R * cos(t * k), I);
        translate3d(&Q[k], R * sin(t * k), J);

        /*cp_point3d(&Q[k], &P[k]);
        translate3d(&Q[k], 1.0, V);*/

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
    add_vertex_to_edge(epred, GQ[0]);
    close_triangle(epred -> next, e1 -> opp);

    //Vérification
    int cons_euler = 0;
    iter_triangles(e1, fnfaces);
    iter_edges(e1, fnedges);
    iter_vertices(e1, fnsommets);

    cons_euler = numfaces + numsommets - numedges;
    printf("constante euler : %d + %d - %d = %d \n", numfaces, numsommets, numedges, cons_euler);

    //Rendre le premier half_edge
    return e1 -> opp -> prev;
}

void testSqueletteDuTube(point3d resultats, int nbPoints, point3d D, point3d A, double R, int precision) {

    
    //Trouver N
    vecteur3d V = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d J = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d I = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d DeltaI = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vec3d(V, D, A);

    assert(!(V -> x == 0 && V -> y == 0 && V -> z == 0));

    int i = 0;

    for (i = 0; i < (nbPoints/2); i++) {

        //Trouver J
        if (abs(V -> z) < abs(V -> x) && abs(V -> z) < abs(V -> y)) {
            J -> x = V -> y;
            J -> y = -(V -> x);
            J -> z = 0;
        } else if (abs(V -> x) < abs(V -> y)) {
            J -> x = 0;
            J -> y = V -> z;
            J -> z = -(V -> y);
        } else {
            J -> x = V -> z;
            J -> y = 0;
            J -> z = -(V -> x);
        }
        //normalize3d(J);

        //Trouver I = N^J
        // I = l'accélération
        vec_prod3d(I, V, J);
        normalize3d(I);

        DeltaI -> x = 2 * (rand() / (double) RAND_MAX) - 1;
        DeltaI -> y = 2 * (rand() / (double) RAND_MAX) - 1;
        DeltaI -> z = 2 * (rand() / (double) RAND_MAX) - 1;
        while (norm3d(DeltaI) < 0.001 || norm3d(DeltaI) > 1) {
            DeltaI -> x = 2 * (rand() / (double) RAND_MAX) - 1;
            DeltaI -> y = 2 * (rand() / (double) RAND_MAX) - 1;
            DeltaI -> z = 2 * (rand() / (double) RAND_MAX) - 1;
        }

        add3d(I, DeltaI);
        add3d(V, I);
        
        translate3d(A,1,V);  
        cp_point3d(&resultats[2*i],A);
        translate3d(A,1,V);
        cp_point3d(&resultats[2*i + 1],A);
        
    }

}
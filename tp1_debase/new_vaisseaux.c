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
#include "levaisseau.h"

void creerPointsVaisseau(point3d* P) {
    
    P[0]->x = 0.0f;
    P[0]->y = 0.0f;
    P[0]->z = 0.0f;

    P[1]->x = 0.6f;
    P[1]->y = 0.0f;
    P[1]->z = 0.0f;

    P[2]->x = 0.5f;
    P[2]->y = 0.5f;
    P[2]->z = 0.0f;

    P[3]->x = 0.0f;
    P[3]->y = 0.5f;
    P[3]->z = 0.0f;

    P[4]->x = 2.5f;
    P[4]->y = 0.5f;
    P[4]->z = 0.0f;

    P[5]->x = 2.5f;
    P[5]->y = 0.0f;
    P[5]->z = 0.0f;

    P[6]->x = 3.5f;
    P[6]->y = 0.5f;
    P[6]->z = 0.0f;

    P[7]->x = 3.5f;
    P[7]->y = 0.0f;
    P[7]->z = 0.0f;

    P[8]->x = 2.2f;
    P[8]->y = -0.5f;
    P[8]->z = 0.0f;

    P[9]->x = 2.2f;
    P[9]->y = 1.0f;
    P[9]->z = 0.0f;

    P[10]->x = 0.5f;
    P[10]->y = -1.0f;
    P[10]->z = 0.0f;

    P[11]->x = 0.5f;
    P[11]->y = 1.5f;
    P[11]->z = 0.0f;

    P[12]->x = 0.0f;
    P[12]->y = 0.5f;
    P[12]->z = -0.3f;

    P[13]->x = 0.0f;
    P[13]->y = 0.0f;
    P[13]->z = -0.3f;

    P[14]->x = 3.5f;
    P[14]->y = 0.0f;
    P[14]->z = -0.15f;

    P[15]->x = 3.5f;
    P[15]->y = 0.5f;
    P[15]->z = -0.15f;
    
}

half_edge creerVaisseau() {
    
    //Point de base 1 & 2
    point3d* P = (point3d*) GC_malloc(16 * sizeof (point3d));
    int i;
    for (i = 0; i < 16; i++) {
        P[i] = (point3d) GC_malloc(sizeof (point3d_cell));
    }
    gl_vertex** GP = (gl_vertex**) GC_malloc(16 * sizeof (gl_vertex*));
    for (i = 0; i < 16; i++) {
        GP[i] = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
        GP[i] = GLvertex3d(P[i]->x, P[i]->y, P[i]->z);
    }
   
    creerPointsVaisseau(P);
    printf("fin créerPoints\n");

    
    //Chercher les repères
    //Le premier
    //V = vec(DA); J ortho à V ; I = V^J
    vecteur3d V = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d I = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d J = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));

    vec3d(V, P[0], P[1]);

    assert(!(V -> x == 0 && V -> y == 0 && V -> z == 0));
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
    vec_prod3d(I, V, J);
    normalize3d(I);
    
    //Here I have my repere
    //Create my Base on 3 points and go create other triangle
    half_edge e1 = create_triangle(GP[1], GP[2], GP[3]);

    //Base triangle create
    add_vertex_to_edge(e1->next, GP[4]);

    //Central rectangle create
    add_vertex_to_edge(e1->next->opp->next, GP[5]);
    add_vertex_to_edge(e1->next->opp->next->next->opp->next, GP[6]);

    //Tete rectangle
    add_vertex_to_edge(e1->next->opp->next->next->opp->next->next, GP[7]);
    add_vertex_to_edge(e1->next->opp->next->next->opp->next->next->next->opp->next, GP[8]);

    //Tete triangle
    add_vertex_to_edge(e1->next->opp->next->next->opp->next->next->next->opp->next->next->opp->next, GP[9]);
    add_vertex_to_edge(e1->next->opp->next->next->opp->next->next->next, GP[10]);
    
    //Corps du vraisseau
    add_vertex_to_edge(e1->opp->prev->prev->prev->opp, GP[11]);
    add_vertex_to_edge(e1->next->opp->next->next, GP[12]);

    //Arriére du vraisseau
    add_vertex_to_edge(e1->next->next, GP[13]);
    add_vertex_to_edge(e1->next->next->next, GP[14]);

    //Matiére
    close_triangle(e1->opp->prev->prev->prev->prev->opp, e1 -> opp);
    close_triangle(e1->next->next->opp->next, e1->next->opp->next->next->next);
    close_triangle(e1->prev->opp, e1 -> next->next->next->next);
    close_triangle(e1->next->next->next->opp->next, e1 -> next->next->opp->next->next);
    close_triangle(e1->opp->prev->prev->prev->opp->next, e1->next->next->next->next->opp->prev->opp);
    close_triangle(e1->next->next->next->opp->next->next, e1->next->next->next->opp->next->next->opp->next->next->next);

    //Avant du vraisseau
    add_vertex_to_edge(e1->opp->prev->opp->next->opp->next->next->next->opp->next->next, GP[15]);
    add_vertex_to_edge(e1->opp->prev->opp->next->opp->next->next->next->opp->next->next->next, GP[16]);

    //Matiére
    close_triangle(e1->opp->prev->opp->next->opp->next->next->next->opp->prev->opp, e1->opp->prev->opp->next->opp->next->next->next->opp->next->next->next->next);
    close_triangle(e1->opp->prev->opp->next->opp->next->next->next->opp->next->next->next->opp->next, e1->opp->prev->opp->next->opp->next->next->next->opp->next->next->opp->next->next);

    printf("fin création vaisseau \n");

    return e1;

}






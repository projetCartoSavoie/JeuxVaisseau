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

void transformationUtilisateur(point3d* P, double ri, double rj, double rv, double ti, double tj) {
    
}

void mettreDansTuyau(point3d* P, const point3d C, const vecteur3d I, const vecteur3d J, const vecteur3d V) {
    
    vecteur3d CP = (vecteur3d) GC_malloc(sizeof(vecteur3d_cell));
    int i;
    for (i = 0; i < 16; i++) {
        chge_rep3d(CP, P[i] -> x, I, P[i] -> y, J, P[i] -> z, V);
        trouve_extremite(P[i], CP, C);    
    }
}

half_edge creerVaisseau(double ri,double rj, double rv, double ti, double tj, const point3d C, const vecteur3d Ilocal, const vecteur3d Jlocal, const vecteur3d Vlocal) {

    //Point de base 1 & 2
    point3d* P = (point3d*) GC_malloc(16 * sizeof (point3d));
    int i;
    for (i = 0; i < 16; i++) {
        P[i] = (point3d) GC_malloc(sizeof (point3d_cell));
    }

    creerPointsVaisseau(P);
    mettreDansTuyau(P,C,Ilocal,Jlocal,Vlocal);
    
    //Chercher les repères
    //Le premier
    //V = vec(DA); J ortho à V ; I = V^J
    vecteur3d V = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d I = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d J = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));

    vec3d(V,P[0],P[1]);

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

    gl_vertex* GP1 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
    gl_vertex* GP2 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
    gl_vertex* GP3 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));

    GP1 = GLvertex3d(P[0]->x, P[0]->y,P[0]->z);
    GP2 = GLvertex3d(P[1]->x,P[1]->y,P[1]->z);
    GP3 = GLvertex3d(P[2]->x,P[2]->y,P[2]->z);

    half_edge e1 = create_triangle(GP1, GP2, GP3);

    //Base triangle create
    gl_vertex* GP4 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
    
    GP4 = GLvertex3d(P[3]->x,P[3]->y,P[3]->z);

    add_vertex_to_edge(e1->next, GP4);

    //Central rectangle create
    gl_vertex* GP5 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
    gl_vertex* GP6 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));

    GP5 = GLvertex3d(P[4]->x,P[4]->y,P[4]->z);
    GP6 = GLvertex3d(P[5]->x,P[5]->y,P[5]->z);

    add_vertex_to_edge(e1->next->opp->next, GP5);
    add_vertex_to_edge(e1->next->opp->next->next->opp->next, GP6);

    //Tete rectangle
    gl_vertex* GP7 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
    gl_vertex* GP8 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));

    GP7 = GLvertex3d(P[6]->x,P[6]->y,P[6]->z);
    GP8 = GLvertex3d(P[7]->x,P[7]->y,P[7]->z);

    add_vertex_to_edge(e1->next->opp->next->next->opp->next->next, GP7);
    add_vertex_to_edge(e1->next->opp->next->next->opp->next->next->next->opp->next, GP8);


    //Tete triangle
    gl_vertex* GP9 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
    gl_vertex* GP10 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));

    GP9 = GLvertex3d(P[8]->x,P[8]->y,P[8]->z);
    GP10 = GLvertex3d(P[9]->x,P[9]->y,P[9]->z);

    add_vertex_to_edge(e1->next->opp->next->next->opp->next->next->next->opp->next->next->opp->next, GP9);
    add_vertex_to_edge(e1->next->opp->next->next->opp->next->next->next, GP10);

    //Corps du vraisseau
    gl_vertex* GP11 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
    gl_vertex* GP12 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));

    GP11 = GLvertex3d(P[10]->x,P[10]->y,P[10]->z);
    GP12 = GLvertex3d(P[11]->x,P[11]->y,P[11]->z);

    add_vertex_to_edge(e1->opp->prev->prev->prev->opp, GP11);
    add_vertex_to_edge(e1->next->opp->next->next, GP12);

    //Arriére du vraisseau
    gl_vertex* GP13 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
    gl_vertex* GP14 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));

    GP13 = GLvertex3d(P[12]->x,P[12]->y,P[12]->z);
    GP14 = GLvertex3d(P[13]->x,P[13]->y,P[13]->z);

    add_vertex_to_edge(e1->next->next, GP13);
    add_vertex_to_edge(e1->next->next->next, GP14);

    //Matiére
    close_triangle(e1->opp->prev->prev->prev->prev->opp, e1 -> opp);
    close_triangle(e1->next->next->opp->next, e1->next->opp->next->next->next);
    close_triangle(e1->prev->opp, e1 -> next->next->next->next);
    close_triangle(e1->next->next->next->opp->next, e1 -> next->next->opp->next->next);
    close_triangle(e1->opp->prev->prev->prev->opp->next, e1->next->next->next->next->opp->prev->opp);
    close_triangle(e1->next->next->next->opp->next->next, e1->next->next->next->opp->next->next->opp->next->next->next);

    //Avant du vraisseau
    gl_vertex* GP15 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
    gl_vertex* GP16 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));

    GP15 = GLvertex3d(P[14]->x,P[14]->y,P[14]->z);
    GP16 = GLvertex3d(P[15]->x,P[15]->y,P[15]->z);

    add_vertex_to_edge(e1->opp->prev->opp->next->opp->next->next->next->opp->next->next, GP15);
    add_vertex_to_edge(e1->opp->prev->opp->next->opp->next->next->next->opp->next->next->next, GP16);
    //Matiére
    close_triangle(e1->opp->prev->opp->next->opp->next->next->next->opp->prev->opp, e1->opp->prev->opp->next->opp->next->next->next->opp->next->next->next->next);
    close_triangle(e1->opp->prev->opp->next->opp->next->next->next->opp->next->next->next->opp->next, e1->opp->prev->opp->next->opp->next->next->next->opp->next->next->opp->next->next);

    return e1;

}



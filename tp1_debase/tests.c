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

typedef struct repere_cell* repere;
typedef struct repere_cell repere_cell;
struct repere_cell {
    point3d C;
    vecteur3d V;
    vecteur3d I;
    vecteur3d J;
    gl_vertex** PointsAutour;
};

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

void cp_vecteur3d(vecteur3d R, const vecteur3d D) {
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

void creerRepere(repere Res0, repere Res1, const repere R, double rayon) {

    vecteur3d newV = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d newJ = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d newI = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d DeltaI = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    
    point3d newCentre0 = (point3d) GC_malloc(sizeof (point3d_cell));
    point3d newCentre1 = (point3d) GC_malloc(sizeof (point3d_cell));


    assert(!(R -> V -> x == 0 && R -> V -> y == 0 && R -> V -> z == 0));

    double maxiDelta = rayon / 7;
    DeltaI -> x = 2 * maxiDelta * (rand() / (double) RAND_MAX) - maxiDelta;
    DeltaI -> y = 2 * maxiDelta * (rand() / (double) RAND_MAX) - maxiDelta;
    DeltaI -> z = 2 * maxiDelta * (rand() / (double) RAND_MAX) - maxiDelta;
    while (norm3d(DeltaI) < 0.0001 || norm3d(DeltaI) > maxiDelta) {
        DeltaI -> x = 2 * maxiDelta * (rand() / (double) RAND_MAX) - maxiDelta;
        DeltaI -> y = 2 * maxiDelta * (rand() / (double) RAND_MAX) - maxiDelta;
        DeltaI -> z = 2 * maxiDelta * (rand() / (double) RAND_MAX) - maxiDelta;
    }
    
    cp_vecteur3d(newI, R -> J);
    add3d(newI, DeltaI);
    
    cp_vecteur3d(newV, R -> V);
    add3d(newV, newI);
    
    vec_prod3d(newJ, newI, newV);
    normalize3d(newJ);
    normalize3d(newI);
    
    vec_prod3d(newV, newJ, newI);
    normalize3d(newV);
    scal_prod3d(newV, rayon);
    
    cp_point3d(newCentre0, R -> C);
    translate3d(newCentre0, 1, newV);
    
    cp_point3d(newCentre1, newCentre0);
    translate3d(newCentre1, 1, newV);
    
    Res0 -> C = newCentre0;
    Res0 -> I = newJ;
    Res0 -> J = newI;
    Res0 -> V = newV;
    
    Res1 -> C = newCentre1;
    Res1 -> I = newJ;
    Res1 -> J = newI;
    Res1 -> V = newV;
}

void creerPointsAutour(repere Rep, double R, int precision) {
    Rep -> PointsAutour = (gl_vertex**) GC_malloc(precision * sizeof (gl_vertex*));
    
    point3d P = (point3d) GC_malloc(precision * sizeof (point3d_cell));

    int k;
    double t = 2 * M_PI / precision;
    for (k = 0; k < precision; k++) {

        cp_point3d(&P[k], Rep -> C);

        translate3d(&P[k], R * cos(t * k), Rep -> I);
        translate3d(&P[k], R * sin(t * k), Rep -> J);
    }

    for (k = 0; k < precision; k++) {
        Rep -> PointsAutour[k] = GLvertex3d(P[k].x, P[k].y, P[k].z);
    } 
}

half_edge raccorderDebut(gl_vertex** GP, gl_vertex** GQ, int precision) {

    //Premier cas (OK)
    half_edge e1 = create_triangle(GP[0], GQ[0], GQ[1]);
    add_vertex_to_edge(e1 -> next, GP[1]);
    half_edge epred = e1 -> next -> next -> opp -> next;

    //Boucle
    int k;
    for (k = 1; k < precision - 1; k++) {
        add_vertex_to_edge(epred, GQ[k + 1]);
        add_vertex_to_edge(epred -> next, GP[k + 1]);
        epred = epred -> next -> next -> opp -> next;
    }

    //Dernier cas
    add_vertex_to_edge(epred, GQ[0]);
    close_triangle(epred -> next, e1 -> opp);

    //Vérification
    int cons_euler = 0; numfaces = 0; numedges = 0; numsommets = 0;
    iter_triangles(e1, fnfaces);
    iter_edges(e1, fnedges);
    iter_vertices(e1, fnsommets);

    cons_euler = numfaces + numsommets - numedges;
    printf("constante euler : %d + %d - %d = %d \n", numfaces, numsommets, numedges, cons_euler);
    
    //Rendre le premier half_edge
    return e1 -> opp -> prev;
}

half_edge raccorder(half_edge e, gl_vertex** GQ, int precision) {
    
    printf("raccorder\n");
    half_edge epred, e2;
    add_vertex_to_edge(e->opp,GQ[1]);
    add_vertex_to_edge(e->prev->opp,GQ[0]);
    
    epred = e -> opp -> next;
    int k;
    for (k = 1; k < precision - 1; k++) {
        add_vertex_to_edge(epred, GQ[k + 1]);
        e2 = epred->prev;
        while(e2 -> prev != NULL){
            e2 = e2 -> prev;
        }
        close_triangle(e2 -> opp, epred -> next);
        epred = epred -> next -> next -> opp -> next;
    }
    
    add_vertex_to_edge(epred, GQ[0]);
    e2 = e -> next;
    while(e2 -> next != NULL) {
        e2 = e2 -> next;
    }
    fill_triangle(epred -> next, e -> prev -> prev -> opp, e2);
    
    //Vérification
    int cons_euler = 0; numfaces = 0; numedges = 0; numsommets = 0;
    iter_triangles(e, fnfaces);
    iter_edges(e, fnedges);
    iter_vertices(e, fnsommets);

    cons_euler = numfaces + numsommets - numedges;
    printf("constante euler : %d + %d - %d = %d \n", numfaces, numsommets, numedges, cons_euler);
    
    /*return e;*/
    return e->prev->prev->opp->prev;
}


half_edge testTubeEntier(int nbPoints, const point3d D, const point3d A, double R, int precision) {

    assert(nbPoints >= 2);
    half_edge test_tetra();
    half_edge test_cylindre(point3d D, point3d A, double R, int precision);
    void testSqueletteDuTube(point3d resultats, int nbPoints, const point3d D , const point3d A , double R, int precision);
    //Chercher les repères
    repere Rep = (repere) GC_malloc(nbPoints * sizeof (repere_cell));

    //Le premier
    //V = vec(DA); J ortho à V ; I = V^J
    vecteur3d V = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d I = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d J = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));

    vec3d(V, D, A);

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

    //Enregistrement des points D et A munis du repère I,J,V aux deux premières places
    Rep[0].C = D;
    Rep[0].I = J;
    Rep[0].J = I;
    Rep[0].V = V;

    Rep[1].C = A;
    Rep[1].I = J;
    Rep[1].J = I;
    Rep[1].V = V;

    //Les suivants
    int i;
    for (i = 1; i < (nbPoints / 2); i++) {
        creerRepere(&Rep[2 * i], &Rep[2 * i + 1], &Rep[2 * i - 1], R);
    }
    
    for (i = 0; i < nbPoints; i++) {
        printf("vérification numéro %d\n", i);
        printf("%f  %f  %f\n",dot_prod3d(Rep[i].I,Rep[i].J),dot_prod3d(Rep[i].J,Rep[i].V),dot_prod3d(Rep[i].I,Rep[i].V));
        /*assert(dot_prod3d(Rep[i].I,Rep[i].J) < 0.1);
        assert(dot_prod3d(Rep[i].J,Rep[i].V) < 0.1);
        assert(dot_prod3d(Rep[i].I,Rep[i].V) < 0.1);*/
    }
    
    
    for (i = 0; i < nbPoints; i++) {
        printf("Point %d : centre (%f , %f , %f) V = (%f , %f , %f) I = (%f , %f , %f) J = (%f , %f , %f) \n",
                i, Rep[i].C->x,  Rep[i].C->y,  Rep[i].C->z, Rep[i].V->x,  Rep[i].V->y,  Rep[i].V->z,
                Rep[i].I->x,  Rep[i].I->y,  Rep[i].I->z, Rep[i].J->x,  Rep[i].J->y,  Rep[i].J->z);
    }

    //Pour chaque repère, fabriquer les GL_Vertex autour
    for (i = 0; i < nbPoints; i++) {
        creerPointsAutour(&Rep[i], R, precision);
    }
    
    //Raccorder (différent au premier tour)
    half_edge e = raccorderDebut(Rep[0].PointsAutour, Rep[1].PointsAutour, precision );
    
    for (i = 1; i < nbPoints - 1; i++) {
        e = raccorder(e, Rep[i+1].PointsAutour, precision);
    }
    
    return e;

}


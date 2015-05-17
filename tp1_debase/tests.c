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

void creerRepereIntermediaire(repere Res, int k, int i, const repere R, double t) {

    vecteur3d newV = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d newJ = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d newI = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    point3d newCentre = (point3d) GC_malloc(sizeof (point3d_cell));

    bary3d3points(newCentre, (1 - t)*(1 - t), R[2 * k].C, 2 * t * (1 - t), R[2 * k + 1].C, t*t, R[2 * k + 2].C);
    moyenne3d3vecteurs(newI, (1 - t)*(1 - t), R[2 * k].I, 2 * t * (1 - t), R[2 * k + 1].I, t*t, R[2 * k + 2].I);
    moyenne3d3vecteurs(newJ, (1 - t)*(1 - t), R[2 * k].J, 2 * t * (1 - t), R[2 * k + 1].J, t*t, R[2 * k + 2].J);
    moyenne3d3vecteurs(newV, (1 - t)*(1 - t), R[2 * k].V, 2 * t * (1 - t), R[2 * k + 1].V, t*t, R[2 * k + 2].V);

    Res[k * 20 + i].C = newCentre;
    Res[k * 20 + i].I = newI;
    Res[k * 20 + i].J = newJ;
    Res[k * 20 + i].V = newV;
}

void creerRepere(repere Res0, repere Res1, const repere R, double rayon, vecteur3d A) {

    vecteur3d newV = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d newJ = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d newI = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d DeltaA = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));

    point3d newCentre0 = (point3d) GC_malloc(sizeof (point3d_cell));
    point3d newCentre1 = (point3d) GC_malloc(sizeof (point3d_cell));


    assert(!(R -> V -> x == 0 && R -> V -> y == 0 && R -> V -> z == 0));

    double maxiDelta = rayon / 2;
    DeltaA -> x = 2 * maxiDelta * (rand() / (double) RAND_MAX) - maxiDelta;
    DeltaA -> y = 2 * maxiDelta * (rand() / (double) RAND_MAX) - maxiDelta;
    DeltaA -> z = 2 * maxiDelta * (rand() / (double) RAND_MAX) - maxiDelta;
    while (norm3d(DeltaA) < 0.0001 || norm3d(DeltaA) > maxiDelta) {
        DeltaA -> x = 2 * maxiDelta * (rand() / (double) RAND_MAX) - maxiDelta;
        DeltaA -> y = 2 * maxiDelta * (rand() / (double) RAND_MAX) - maxiDelta;
        DeltaA -> z = 2 * maxiDelta * (rand() / (double) RAND_MAX) - maxiDelta;
    }

    //a' = a + DeltaA
    add3d(A, DeltaA);

    //v' = v + a'
    cp_vecteur3d(newV, R -> V);
    add3d(newV, A);

    //i' = j^v'
    vec_prod3d(newI, R -> J, newV);

    //j' = v'^i'
    vec_prod3d(newJ, newV, newI);

    //i' = j'^v'
    vec_prod3d(newI, newJ, newV);

    //normalisation
    normalize3d(newI);
    normalize3d(newJ);
    normalize3d(newV);
    scal_prod3d(newV, rayon);

    //centres
    cp_point3d(newCentre0, R -> C);
    translate3d(newCentre0, 1, newV);

    cp_point3d(newCentre1, newCentre0);
    translate3d(newCentre1, 1, newV);

    Res0 -> C = newCentre0;
    Res0 -> I = newI;
    Res0 -> J = newJ;
    Res0 -> V = newV;

    Res1 -> C = newCentre1;
    Res1 -> I = newI;
    Res1 -> J = newJ;
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
    int cons_euler = 0;
    numfaces = 0;
    numedges = 0;
    numsommets = 0;
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
    add_vertex_to_edge(e->opp, GQ[1]);
    add_vertex_to_edge(e->prev->opp, GQ[0]);

    epred = e -> opp -> next;
    int k;
    for (k = 1; k < precision - 1; k++) {
        add_vertex_to_edge(epred, GQ[k + 1]);
        e2 = epred->prev;
        while (e2 -> prev != NULL) {
            e2 = e2 -> prev;
        }
        close_triangle(e2 -> opp, epred -> next);
        epred = epred -> next -> next -> opp -> next;
    }

    add_vertex_to_edge(epred, GQ[0]);
    e2 = e -> next;
    while (e2 -> next != NULL) {
        e2 = e2 -> next;
    }
    fill_triangle(epred -> next, e -> prev -> prev -> opp, e2);

    //Vérification
    int cons_euler = 0;
    numfaces = 0;
    numedges = 0;
    numsommets = 0;
    iter_triangles(e, fnfaces);
    iter_edges(e, fnedges);
    iter_vertices(e, fnsommets);

    cons_euler = numfaces + numsommets - numedges;
    printf("constante euler : %d + %d - %d = %d \n", numfaces, numsommets, numedges, cons_euler);

    /*return e;*/
    return e->prev->prev->opp->prev;
}

half_edge testTubeEntier(int nbPoints, repere Rep, const point3d D, const point3d A, double R, int precision) {

    assert(nbPoints >= 2);

    //Chercher les repères

    //Le premier
    //V = vec(DA); J ortho à V ; I = V^J
    vecteur3d V = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d I = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d J = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d Acc = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));

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

    cp_vecteur3d(Acc, V);
    scal_prod3d(Acc, 0.5);

    //Les suivants
    int i;
    for (i = 1; i < (nbPoints / 2); i++) {
        creerRepere(&Rep[2 * i], &Rep[2 * i + 1], &Rep[2 * i - 1], R, Acc);
    }

    //Les repères doivent être ajustés pour suivre Bézier
    int k;
    for (k = 0; k < (nbPoints / 2) - 1; k++) {
        point3d_cell p;
        bary3d3points(&p, 0.25, Rep[2 * k].C, 0.5, Rep[2 * k + 1].C, 0.25, Rep[2 * k + 2].C);
        Rep[2 * k + 1].C -> x = p.x;
        Rep[2 * k + 1].C -> y = p.y;
        Rep[2 * k + 1].C -> z = p.z;
    }

    //Puis on doit créer des repères intermédiaires entre chaque paire de repères pour faire plus de cylindres
    repere RepIntermediaires = (repere) GC_malloc((nbPoints - 2) * 10 * sizeof (repere_cell));
    double t;
    for (k = 0; k < (nbPoints / 2) - 1; k++) {
        for (i = 0; i < 20; i++) {
            if (k > 0 || i > 10) {
                t = ((double) i) / 20;
                creerRepereIntermediaire(RepIntermediaires, k, i, Rep, t);
            }
        }
    }

    for (i = 11; i < 10 * nbPoints - 20; i++) {
        assert(dot_prod3d(RepIntermediaires[i].I, RepIntermediaires[i].J) < 0.1);
        assert(dot_prod3d(RepIntermediaires[i].J, RepIntermediaires[i].V) < 0.1);
        assert(dot_prod3d(RepIntermediaires[i].I, RepIntermediaires[i].V) < 0.1);
    }

    //Pour chaque repère, fabriquer les GL_Vertex autour
    creerPointsAutour(&Rep[0], R, precision);
    creerPointsAutour(&Rep[1], R, precision);
    for (i = 11; i < 10 * nbPoints - 20; i++) {
        creerPointsAutour(&RepIntermediaires[i], R, precision);
    }

    //Raccorder (différent au premier tour)
    half_edge e = raccorderDebut(Rep[0].PointsAutour, Rep[1].PointsAutour, precision);

    for (i = 10; i < 10 * nbPoints - 21; i++) {
        e = raccorder(e, RepIntermediaires[i + 1].PointsAutour, precision);
    }

    return e;

}

half_edge old_testTubeEntier(int nbPoints, repere Rep, const point3d D, const point3d A, double R, int precision) {

    assert(nbPoints >= 2);
    half_edge test_cylindre(point3d D, point3d A, double R, int precision);
    void testSqueletteDuTube(point3d resultats, int nbPoints, const point3d D, const point3d A, double R, int precision);

    //Chercher les repères

    //Le premier
    //V = vec(DA); J ortho à V ; I = V^J
    vecteur3d V = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d I = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d J = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d Acc = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));

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

    cp_vecteur3d(Acc, V);
    scal_prod3d(Acc, 0.5);

    //Les suivants
    int i;
    for (i = 1; i < (nbPoints / 2); i++) {
        creerRepere(&Rep[2 * i], &Rep[2 * i + 1], &Rep[2 * i - 1], R, Acc);
    }

    for (i = 0; i < nbPoints; i++) {
        assert(dot_prod3d(Rep[i].I, Rep[i].J) < 0.1);
        assert(dot_prod3d(Rep[i].J, Rep[i].V) < 0.1);
        assert(dot_prod3d(Rep[i].I, Rep[i].V) < 0.1);
    }

    for (i = 0; i < nbPoints; i++) {
        printf("Point %d : centre (%f , %f , %f) V = (%f , %f , %f) I = (%f , %f , %f) J = (%f , %f , %f) \n",
                i, Rep[i].C->x, Rep[i].C->y, Rep[i].C->z, Rep[i].V->x, Rep[i].V->y, Rep[i].V->z,
                Rep[i].I->x, Rep[i].I->y, Rep[i].I->z, Rep[i].J->x, Rep[i].J->y, Rep[i].J->z);
    }

    //Pour chaque repère, fabriquer les GL_Vertex autour
    for (i = 0; i < nbPoints; i++) {
        creerPointsAutour(&Rep[i], R, precision);
    }

    //Raccorder (différent au premier tour)
    half_edge e = raccorderDebut(Rep[0].PointsAutour, Rep[1].PointsAutour, precision);

    for (i = 1; i < nbPoints - 1; i++) {
        e = raccorder(e, Rep[i + 1].PointsAutour, precision);
    }

    return e;

}

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


half_edge creerVaisseau() {

	//Point de base 1 & 2
	point3d P1 = (point3d) GC_malloc(sizeof (point3d_cell));
	point3d P2 = (point3d) GC_malloc(sizeof (point3d_cell));
	
	P1->x = 0.0f;
    P1->y = 0.0f;
    P1->z = 0.0f;
    
    P2->x = 0.6f;
    P2->y = 0.0f;
    P2->z = 0.0f;
    
    //Chercher les repères
    //Le premier
    //V = vec(DA); J ortho à V ; I = V^J
    vecteur3d V = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d I = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d J = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));

	vec3d(V, P1, P2);

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
    
    point3d P3 = (point3d) GC_malloc(sizeof (point3d_cell));
    gl_vertex* GP1 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
    gl_vertex* GP2 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
    gl_vertex* GP3 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
    
    P3->x = 0.5f;
    P3->y = 0.5f;
    P3->z = 0.0f;

    GP1 = GLvertex3d(P1->x, P1->y, P1->z);
    GP2 = GLvertex3d(P2->x, P2->y, P2->z);
    GP3 = GLvertex3d(P3->x, P3->y, P3->z);
    
	half_edge e1 = create_triangle(GP1, GP2, GP3);
		
	//Base triangle create
	point3d P4 = (point3d) GC_malloc(sizeof (point3d_cell));
	gl_vertex* GP4 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
	
	P4->x = 0.0f;
    P4->y = 0.5f;
    P4->z = 0.0f;
    
    GP4 = GLvertex3d(P4->x, P4->y, P4->z);
    
    add_vertex_to_edge(e1->next, GP4);
    
    //Central rectangle create
    point3d P5 = (point3d) GC_malloc(sizeof (point3d_cell));
	gl_vertex* GP5 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
	point3d P6 = (point3d) GC_malloc(sizeof (point3d_cell));
	gl_vertex* GP6 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
    
    P5->x = 2.5f;
    P5->y = 0.5f;
    P5->z = 0.0f;
    
    P6->x = 2.5f;
    P6->y = 0.0f;
    P6->z = 0.0f;
    
    GP5 = GLvertex3d(P5->x, P5->y, P5->z);
    GP6 = GLvertex3d(P6->x, P6->y, P6->z);
    
    add_vertex_to_edge(e1->next->opp->next, GP5);
    add_vertex_to_edge(e1->next->opp->next->next->opp->next, GP6);
    
    //Tete rectangle
    point3d P7 = (point3d) GC_malloc(sizeof (point3d_cell));
	gl_vertex* GP7 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
	point3d P8 = (point3d) GC_malloc(sizeof (point3d_cell));
	gl_vertex* GP8 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
	
	P7->x = 3.5f;
    P7->y = 0.5f;
    P7->z = 0.0f;
    
    P8->x = 3.5f;
    P8->y = 0.0f;
    P8->z = 0.0f;
    
    GP7 = GLvertex3d(P7->x, P7->y, P7->z);
    GP8 = GLvertex3d(P8->x, P8->y, P8->z);
    
    add_vertex_to_edge(e1->next->opp->next->next->opp->next->next, GP7);
    add_vertex_to_edge(e1->next->opp->next->next->opp->next->next->next->opp->next, GP8);
    
    
    //Tete triangle
    point3d P9 = (point3d) GC_malloc(sizeof (point3d_cell));
	gl_vertex* GP9 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
	point3d P10 = (point3d) GC_malloc(sizeof (point3d_cell));
	gl_vertex* GP10 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
	
	P9->x = 2.2f;
    P9->y = -0.5f;
    P9->z = 0.0f;
    
    P10->x = 2.2f;
    P10->y = 1.0f;
    P10->z = 0.0f;
    
    GP9 = GLvertex3d(P9->x, P9->y, P9->z);
    GP10 = GLvertex3d(P10->x, P10->y, P10->z);
    
    add_vertex_to_edge(e1->next->opp->next->next->opp->next->next->next->opp->next->next->opp->next ,GP9);
    add_vertex_to_edge(e1->next->opp->next->next->opp->next->next->next, GP10);
    
    //Corps du vraisseau
    point3d P11 = (point3d) GC_malloc(sizeof (point3d_cell));
	gl_vertex* GP11 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
	point3d P12 = (point3d) GC_malloc(sizeof (point3d_cell));
	gl_vertex* GP12 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
	
	P11->x = 0.5f;
    P11->y = -1.0f;
    P11->z = 0.0f;
    
    P12->x = 0.5f;
    P12->y = 1.5f;
    P12->z = 0.0f;
    
    GP11 = GLvertex3d(P11->x, P11->y, P11->z);
    GP12 = GLvertex3d(P12->x, P12->y, P12->z);
    
    add_vertex_to_edge(e1->opp->prev->prev->prev->opp ,GP11);
    add_vertex_to_edge(e1->next->opp->next->next, GP12);
    
    //Arriére du vraisseau
    point3d P13 = (point3d) GC_malloc(sizeof (point3d_cell));
	gl_vertex* GP13 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
	point3d P14 = (point3d) GC_malloc(sizeof (point3d_cell));
	gl_vertex* GP14 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
	
	P13->x = 0.0f;
    P13->y = 0.5f;
    P13->z = -0.3f;
    
    P14->x = 0.0f;
    P14->y = 0.0f;
    P14->z = -0.3f;
    
    GP13 = GLvertex3d(P13->x, P13->y, P13->z);
    GP14 = GLvertex3d(P14->x, P14->y, P14->z);
    
    add_vertex_to_edge(e1->next->next,GP13);
    add_vertex_to_edge(e1->next->next->next, GP14);
    
    //Matiére
    close_triangle(e1->opp->prev->prev->prev->prev->opp, e1 -> opp);
    close_triangle(e1->next->next->opp->next, e1->next->opp->next->next->next);
    close_triangle(e1->prev->opp, e1 -> next->next->next->next);
	close_triangle(e1->next->next->next->opp->next, e1 -> next->next->opp->next->next);
	close_triangle(e1->opp->prev->prev->prev->opp->next,e1->next->next->next->next->opp->prev->opp);
	close_triangle(e1->next->next->next->opp->next->next,e1->next->next->next->opp->next->next->opp->next->next->next);
	
	//Avant du vraisseau
    point3d P15 = (point3d) GC_malloc(sizeof (point3d_cell));
	gl_vertex* GP15 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
	point3d P16 = (point3d) GC_malloc(sizeof (point3d_cell));
	gl_vertex* GP16 = (gl_vertex*) GC_malloc(sizeof (gl_vertex));
	
	P15->x = 3.5f;
    P15->y = 0.0f;
    P15->z = -0.15f;
    
    P16->x = 3.5f;
    P16->y = 0.5f;
    P16->z = -0.15f;
    
    GP15 = GLvertex3d(P15->x, P15->y, P15->z);
    GP16 = GLvertex3d(P16->x, P16->y, P16->z);
    
    add_vertex_to_edge(e1->opp->prev->opp->next->opp->next->next->next->opp->next->next,GP15);
    add_vertex_to_edge(e1->opp->prev->opp->next->opp->next->next->next->opp->next->next->next, GP16);
    //Matiére
    close_triangle(e1->opp->prev->opp->next->opp->next->next->next->opp->prev->opp,e1->opp->prev->opp->next->opp->next->next->next->opp->next->next->next->next);
    close_triangle(e1->opp->prev->opp->next->opp->next->next->next->opp->next->next->next->opp->next, e1->opp->prev->opp->next->opp->next->next->next->opp->next->next->opp->next->next);
	
    return e1;

}



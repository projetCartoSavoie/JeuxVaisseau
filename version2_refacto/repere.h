#ifndef REPERE_H
#define	REPERE_H

//Structure pour représenter un repère local (1 point 3 vecteur3d)
typedef struct repere_cell* repere;
typedef struct repere_cell repere_cell;
struct repere_cell {
    point3d C;
    vecteur3d V;
    vecteur3d I;
    vecteur3d J;
    gl_vertex** PointsAutour;
};

#endif	/* REPERE_H */


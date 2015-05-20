
#include "array.h"

/* Creation d'un tetraèdre (Attention à l'orientation */
half_edge tetraedron(gl_vertex *v1, gl_vertex *v2, gl_vertex *v3, gl_vertex *v4);

/* Calcul des normales en chaque sommet égale aux moyennes des produits vectoriels
   des faces, normalisé. Donne une valeur raisonnable pour les surfaces lisses, 
   mais un calcul plus fin est préférable si on connait l'équation de la surface 
   par example. */
void set_average_normal(half_edge e);

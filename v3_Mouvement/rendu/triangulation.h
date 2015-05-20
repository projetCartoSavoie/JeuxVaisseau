
#ifndef TRIANGULATION_H
#define TRIANGULATION_H

/* Structure de donnée avec demi-arête pour représenter les surfaces  *
 * orientables. (cf cours)                                            *
 *                                                                    *
 * On stocke à la fois la demi arête suivante et la précédente, cela  *
 * permet un meilleur traitement du bord:                             *
 * - e->opp : jamais null                                             *
 * - e->next ou e->prev null au bord.                                 *
 *                                                                    *
 * Les données sont de type VERTEX* ... polymorphisme du pauvre en C  *
 * Il faudra définir la macro VERTEX and le #include de ce fichier    *
 */

typedef struct half_edge_cell *half_edge;

struct half_edge_cell {
  VERTEX* vertex;
  half_edge opp;
  half_edge next;
  half_edge prev;
  unsigned int visited;
};

/* on offre aussi un type pour les listes de demi arrête             *
 */
typedef struct edge_list_cell *edge_list;

struct edge_list_cell {
  half_edge head;
  edge_list tail;
};

/* Constructeur  des listes de demi-arrêtes                          *
 */

edge_list cons_edge_list(half_edge e, edge_list l);

/* Creation d'un nouveau triangle.                                    *
 * Attention, l'ordre des sommets doit respecté l'orientation du      *
 * triangle.                                                          *
 */
half_edge create_triangle(VERTEX* v1, VERTEX* v2, VERTEX* v3);

/* Ajout d'un sommet à une arrête existante pour former un nouveau    *
 * triangle                                                           *
 */
void add_vertex_to_edge(half_edge e1, VERTEX* v3);

/* Si e1 et e2 sont deux arrêtes consécutive:
   i.e. e1->opp->vertex == e2->vertex
   cette fonction ajoute une troisième arrête pour fermer
   le triangle */
void close_triangle(half_edge e1, half_edge e2);

/* Si les trois arrêtes forment un trou triangulaire (elle forment 
   une boucle avec e1->opp->vertex == e2->vertex
                   e2->opp->vertex == e3->vertex
		   e3->opp->vertex == e1->vertex mais
   e1->next, e2->next et e3->next ne sont pas défini, alors
   cette fonction bouche le trou (défini les trois next ci-dessus
   et les prev correspondant */
void fill_triangle(half_edge e1, half_edge e2, half_edge e3);

/* Suppression d'un triangle (crée un trou) */
void remove_triangle(half_edge e1);

/* Cette fonction applique sont second argument fn une fois sur tous  *
 * les triangles accessibles depuis la demi arête e.                  *
 * les sommets sont dans l'ordre de l'orientation                     *
 */
void iter_triangles(half_edge e, void fn(half_edge, VERTEX*, VERTEX*, VERTEX*));
/* idem avec une liste de half_edge */
void iter_comp_triangles(edge_list l, void fn(half_edge, VERTEX*, VERTEX*, VERTEX*));

/* Cette fonction applique sont second argument fn une fois sur       *
 * toutes les arrêtes accessibles depuis la demi arête e.             *
 */
void iter_edges(half_edge e, void fn(half_edge, VERTEX*, VERTEX*));
/* idem avec une liste de half_edge */
void iter_comp_edges(edge_list l, void fn(half_edge, VERTEX*, VERTEX*));

/* Cette fonction applique sont second argument fn une fois sur tous  *
 * les sommets accessibles depuis la demi arête e.                    *
 */
void iter_vertices(half_edge e, void fn(half_edge, VERTEX*));
/* idem avec une liste de half_edge */
void iter_comp_vertices(edge_list l, void fn(half_edge, VERTEX*));

#endif

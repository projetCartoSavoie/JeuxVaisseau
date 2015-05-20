 _________________________________
| 																|
| Rémi Mollard  									|
| Céline de Roland								|
|																	|
| TP d'infographie, M1 STIC ISC		|
|_________________________________|

Pour voir le résultat, entrez les commandes :
---------------------------------------------

	make depend 
	make
	./main.exe

Ce que nous avons fait pour ce TP :
-----------------------------------

	** Tube **

		A partir de 2 points donnés, créer un cylindre initial
		A partir de ces 2 points, par le système de "légère variation de l'accélération" vu en cours, création d'une suite de points formant un chemin "aléatoire" pour notre tube. 
		Ce chemin est constitué de points auxquels on attache un repère local (structure "repere") dont les 3 vecteurs représentent :
			la "vitesse" (vecteur V)
			un ensemble de 2 vecteurs générateurs du plan contenant le cercle formant le tour du tube à cet endroit (vecteurs I et J)
		Chaque triplet de ces points permet de former une courbe de Bézier d'ordre 2 (on assure une trajectoire "lisse" en faisant varier l'accélération et la vitesse seulement une fois sur deux).
		Nous remplaçons l'ensemble des repères de départ par un ensemble de repères locaux qui "suivent" cette courbe de Bézier (chaque triplet de points est remplacé par 20 points).
		Ensuite on crée le "tour" du tube en placant 20 points autour de chaque centre de repère. Les couples de cercles ainsi formés sont raccordés par des triangles pour former la surface externe du tube.

	 
	** Vaisseau **

		Notre vaisseau est un ensemble de 16 points reliés par des triangles. On lui applique des transformations pour le placer à l'horizontal par rapport au tube.
		La position du vaisseau dans le tube est paramétré par un entier nommé currentStep.
		Lorsque le joueur tape sur la touche "flèche haute", on incrémente cet entier.
		De cet entier on déduit sur quelle courbe de Bézier se trouve le vaisseau et à quel "t" il est sur cette courbe. Par interpolation on construit le repère local attaché à ce point de la courbe de Bézier.
		On utilise ensuite des transformations correspondants à un changement de repère (repère scène -> repère local) pour dessiner le vaisseau à l'endroit correspondant.


Ce qui reste à faire / est en cours :
-------------------------------------

	Permettre à l'utilisateur de dévier le vaisseau de sa trajectoire et d'effectuer des rotations par l'action du joueur. Notre stratégie est d'effectuer ces transformations sur le vaisseau lorsqu'il est placé en (0,0,0) après l'avoir placé à l'horizontal par rapport au tube (étape 1 de la partie vaisseau), puis d'effectuer les étapes suivantes de la partie vaisseau.
	Calculer les collisions en 3 étapes :
		D'après la position et la longueur du vaisseau, déterminer les faces du tube avec lesquelles il est susceptible d'entrer en collision.
		Calculer les collisions d'une boite entourant le vaisseau avec les faces trouvées à l'étape précédente.
		Si pas de collision avec la boite, on ne fait rien. Si il y a collision avec la boite alors on regarde s'il y a collision avec le vaisseau en regardant chaque face du vaisseau par rapport aux faces du tube qui étaient en collision avec la boite.
	Nous avons prévu de faire revenir le joueur à 0 si il fait une collision.

	


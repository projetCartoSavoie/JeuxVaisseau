#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {// Démarrage de la SDL. Si erreur alors...
        fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError()); // Ecriture de l'erreur
        exit(EXIT_FAILURE); // On quitte le programme
    }
    int* x = GC_malloc(5*sizeof(int));
    SDL_Quit();
    return EXIT_SUCCESS;
}

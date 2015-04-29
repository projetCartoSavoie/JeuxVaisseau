/*
 * Ce code est une modification des tutoriaux OpenGL
 * Nehe [http://nehe.gamedev.net/]
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <SDL2/SDL.h>
#include <math.h>
#include <sys/time.h>
#include "tiny_gc.h"
#include "geometry.h"
#include "array.h"
#include "triangulation_tools.h"
#include "tests.h"

/* La "window SDL", c'est à dire la fenêtre où l'on dessine
 * obligatoirement une variable globale, car les "callbacks"
 * gérant par exemple les touches clavier en ont besoin et
 * ne la reçoive (malheureusement) pas en argument.
 */
SDL_Window *window;
/* et son "context" openGL */
SDL_GLContext glcontext;

/* fonction pour quitter */
void Quit(int returnCode) {
    /* clean up the window */
    SDL_Quit();

    /* and exit appropriately */
    exit(returnCode);
}

/* fonction pour rediensionner la fenêtre */
int resizeWindow(int width, int height) {
    /* Height / width ration */
    GLfloat ratio;

    /* Protect against a divide by zero */
    if (height == 0)
        height = 1;

    ratio = (GLfloat) width / (GLfloat) height;

    /* Setup our viewport. */
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);

    /* change to the projection matrix and set our viewing volume. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    /* Set our perspective */
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);

    /* Make sure we're chaning the model view and not the projection */
    glMatrixMode(GL_MODELVIEW);

    /* Reset The View */
    glLoadIdentity();

    return ( 1);
}

/* initialisation d'OpenGL */
int initGL(GLvoid) {

    /* Enable smooth shading */
    glShadeModel(GL_SMOOTH);

    /* Set the background black */
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    /* Depth buffer setup */
    glClearDepth(1.0f);

    /* Enables Depth Testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);


    glEnable(GL_LIGHTING);
    /* Enable Light One */
    glEnable(GL_LIGHT1);

    /* Ambient Light Values */
    GLfloat LightAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    /* Diffuse Light Values */
    GLfloat LightDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    /* Light Position (*/
    GLfloat LightPosition[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glDisable(GL_CULL_FACE);

    /* Really Nice Perspective Calculations */
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glPointSize(3.0);

    return ( 1);
}


/* variable globale de la scène */
half_edge cylindre_initial = NULL;
gl_object *gl_cylindre_initial = NULL;

void initGLScene() {
    point3d_cell D[1];
    point3d_cell A[1];
    /*point3d_cell Pt[6];*/
    
    double R = 3.0;
    int precision = 10;

    D -> x = 0.0;
    D -> y = 0.0;
    D-> z = -5.0;

    A -> x = 0.0;
    A -> y = 0.0;
    A -> z = 0.0;

    
    cylindre_initial = testTubeEntier(50, D, A, R, precision);

    if (cylindre_initial) {
        gl_cylindre_initial = triangulation_poly_to_gl_object(cylindre_initial);
    }
}

static unsigned int frames = 0;
static struct timeval lasttime;
static double lastframe;

double count_frames() {
    struct timeval curtime;

    frames++;
    gettimeofday(&curtime, NULL);

    double new = curtime.tv_sec + curtime.tv_usec * 1e-6;
    double delta = new - lastframe;
    lastframe = new;

    if (curtime.tv_sec - lasttime.tv_sec >= 5) {
        printf("%.1lf frames by second\n",
                (double) frames /
                ((double) curtime.tv_sec - (double) lasttime.tv_sec
                + (double) (curtime.tv_usec - lasttime.tv_usec) * 1e-6));
        lasttime = curtime;
        frames = 0;
    }

    return delta;
}

void init_count_frames() {
    frames = 0;
    gettimeofday(&lasttime, NULL);
    lastframe = lasttime.tv_sec + lasttime.tv_usec * 1e-6;
}

/* La fonction qui dessine la scène */
int drawGLScene(GLvoid) {
    /* rotational vars for the triangle and quad, respectively */
    static GLfloat rtri;

    double delta = count_frames();

    /* Clear The Screen And The Depth Buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0f, -10.0f, -50.0f);
    glRotatef(rtri, 0.0f, 1.0f, 1.0f);


    if (gl_cylindre_initial) {
        glColor3f(0.7f, 0.2f, 0.2f);
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glDrawObject(gl_cylindre_initial);
    }

    /* Draw it to the screen */
    SDL_GL_SwapWindow(window);

    /* Increase The Rotation Variable For The Triangle */

    rtri += 45.0f * (float) delta;

    return ( 1);
}

#define FULLSCREENNB 3
SDL_WindowFlags fullScreenValue[FULLSCREENNB] = {0, SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN_DESKTOP};
int currentFullscreen = 0;

/* fonstion de gestion du clavier */
void handleKeyPress(SDL_Keysym *keysym) {
    switch (keysym->sym) {
        case SDLK_q:
        case SDLK_ESCAPE:
            /* ESC key was pressed */
            Quit(0);
            break;
        case SDLK_F1:
            /* F1 key was pressed
             * this toggles fullscreen mode
             */
            currentFullscreen = (currentFullscreen + 1) % FULLSCREENNB;
            SDL_SetWindowFullscreen(window, fullScreenValue[currentFullscreen]);
            break;
        default:
            break;
    }

    return;
}

int main(int argc, char **argv) {
    GC_init(&argc, 0x1000000, 0x1000);
    GC_register_global(&gl_cylindre_initial);
    GC_register_global(&cylindre_initial);

    init_count_frames();

    /* used to collect events */
    SDL_Event event;

    /* initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Video initialization failed: %s\n",
                SDL_GetError());
        Quit(1);
    }

    /* Sets up OpenGL double buffering */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);


    /* get a SDL surface */
    window = SDL_CreateWindow("TP1-INFO821", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 848, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    /* Verify there is a surface */
    if (!window) {
        fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
        Quit(1);
    }
    glcontext = SDL_GL_CreateContext(window);

    /* initialize OpenGL */
    initGL();
    initGLScene();

    /* resize the initial window */
    resizeWindow(848, 480);


    /* wait for events */
    while (1) {
        /* handle the events in the queue */

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_WINDOWEVENT:
                    /* handle resize event */
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            SDL_SetWindowSize(window,
                                    event.window.data1,
                                    event.window.data2);
                            resizeWindow(event.window.data1,
                                    event.window.data2);
                            break;
                    }
                    break;
                case SDL_KEYDOWN:
                    /* handle key presses */
                    handleKeyPress(&event.key.keysym);
                    break;
                case SDL_QUIT:
                    /* handle quit requests */
                    Quit(0);
                    break;
                default:
                    break;
            }
        }

        drawGLScene();
    }

    /* Should never get here */
    return ( 0);
}

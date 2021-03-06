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
#include "levaisseau.h"
#define FULLSCREENNB 3
#define VAISSEAU 1
#define DANS_LE_TUBE 2
#define SCENE_ENTIERE 3



/* La "window SDL", c'est à dire la fenêtre où l'on dessine
 * obligatoirement une variable globale, car les "callbacks"
 * gérant par exemple les touches clavier en ont besoin et
 * ne la reçoive (malheureusement) pas en argument.
 */
SDL_Window *window;
/* et son "context" openGL */
SDL_GLContext glcontext;

SDL_WindowFlags fullScreenValue[FULLSCREENNB] = {0, SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN_DESKTOP};
int currentFullscreen = 0;
int currentView = SCENE_ENTIERE;
int currentStep = 10;
int transI = 0;
int transJ = 0;
int rotV = 0;
int rotI = 0;
int rotJ = 0;

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
    gluPerspective(45.0f, ratio, 0.1f, 500.0f);

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
half_edge vaisseau = NULL;
gl_object *gl_vaisseau = NULL;
repere Rep = NULL;
int nbPoints;

void initGLScene() {

    double R = 3.0;
    int precision = 20;
    nbPoints = 50;

    point3d_cell D[1];
    point3d_cell A[1];
    Rep = (repere) GC_malloc(nbPoints * sizeof (repere_cell));

    D -> x = 0.0;
    D -> y = 0.0;
    D-> z = -3.0;

    A -> x = 0.0;
    A -> y = 0.0;
    A -> z = 0.0;


    cylindre_initial = testTubeEntier(nbPoints, Rep, D, A, R, precision);
    vaisseau = creerVaisseau(rotI,rotJ,rotV,transI,transJ,Rep[1].C, Rep[1].I, Rep[1].J, Rep[1].V);

   
    if (cylindre_initial) {
        gl_cylindre_initial = triangulation_poly_to_gl_object(cylindre_initial);
    }
    if (vaisseau) {
        gl_vaisseau = triangulation_poly_to_gl_object(vaisseau);
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
    glTranslatef(0.0f, -5.0f, -250.0f);
    glRotatef(rtri, 0.0f, 1.0f, 1.0f);
    //glRotatef(50.0f, 1.0f, 1.0f, 1.0f);

    if (gl_cylindre_initial) {
        glColor3f(0.7f, 0.2f, 0.2f);
        glDrawObject(gl_cylindre_initial);
    }

    /* Draw it to the screen */
    SDL_GL_SwapWindow(window);

    /* Increase The Rotation Variable For The Triangle */

    rtri += 45.0f * (float) delta;

    return (1);
}

int drawMyVaisseau(GLvoid){
	
	static GLfloat rtri;

    double delta = count_frames();
	
	/* Clear The Screen And The Depth Buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    
    glTranslatef(0.0f, -1.0f, -10.0f);
    glRotatef(rtri, 0.0f, 1.0f, 1.0f);
    
    if (gl_vaisseau) {
        glColor3f(0.7f, 0.2f, 0.2f);
        glDrawObject(gl_vaisseau);
    }
    
    /* Draw it to the screen */
    SDL_GL_SwapWindow(window);
    
    /* Increase The Rotation Variable For The Triangle */

    rtri += 45.0f * (float) delta;
    
	return(1);
}

/* La fonction qui dessine la scène */
int drawMyScene(GLvoid) {

    /* Clear The Screen And The Depth Buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    glLoadIdentity();

    point3d Eye = (point3d) GC_malloc(sizeof (point3d_cell));
    point3d TmpEye = (point3d) GC_malloc(sizeof (point3d_cell));
    point3d Cible = (point3d) GC_malloc(sizeof (point3d_cell));
    vecteur3d Up = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d Jlocal = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));
    vecteur3d Vlocal = (vecteur3d) GC_malloc(sizeof (vecteur3d_cell));

    int k = (currentStep / 20) * 2;
    int bigT = currentStep % 20;
    double t = ((double) bigT)/20;
    printf("k = %d ; bigT = %d\n", k, bigT); 
    
    bary3d3points(Eye, (1-t)*(1-t), Rep[k].C, 2*t*(1-t), Rep[k + 1].C, t*t, Rep[k+2].C);  
    bary3d3points(Cible, (1-t)*(1-t), Rep[k+1].C, 2*t*(1-t), Rep[k + 2].C, t*t, Rep[k+3].C);  
    moyenne3d3vecteurs(Up, (1-t)*(1-t), Rep[k].I, 2*t*(1-t), Rep[k + 1].I, t*t, Rep[k+2].I); 
    moyenne3d3vecteurs(Jlocal, (1-t)*(1-t), Rep[k].J, 2*t*(1-t), Rep[k + 1].J, t*t, Rep[k+2].J); 
    moyenne3d3vecteurs(Vlocal, (1-t)*(1-t), Rep[k].V, 2*t*(1-t), Rep[k + 1].V, t*t, Rep[k+2].V); 
    //translate3d(Eye, -0.5, Up);
    cp_point3d(TmpEye, Eye);
    translate3d(TmpEye, -2, Vlocal);

    gluLookAt(
            (float) (TmpEye->x), (float) (TmpEye->y), (float) (TmpEye->z),
            (float) (Cible->x), (float) (Cible->y), (float) (Cible->z),
            (float) (Up->x), (float) (Up->y), (float) (Up->z));


    vaisseau = creerVaisseau(rotI,rotJ,rotV,transI,transJ,Eye, Up, Jlocal, Vlocal);
    if (vaisseau) {
        
        gl_vaisseau = triangulation_poly_to_gl_object(vaisseau);
        if (gl_vaisseau) {
            printf("dessin vaisseau\n");
            glColor3f(0.2f, 0.2f, 0.7f);
            glDrawObject(gl_vaisseau);
        }
    }
    if (gl_cylindre_initial) {
        glColor3f(0.7f, 0.2f, 0.2f);
        glDrawObject(gl_cylindre_initial);
    }



    /* Draw it to the screen */
    SDL_GL_SwapWindow(window);

    /* Increase The Rotation Variable For The Triangle */

    return (1);
}

/* fonction de gestion du clavier */
void handleKeyPress(SDL_Keysym *keysym) {
    switch (keysym->sym) {
        case SDLK_UP:
            if (currentStep < (nbPoints*10)-20 -1) {
                currentStep++;
                drawMyScene();
            }
            break;
        case SDLK_DOWN:
            if (currentStep > 0) {
                currentStep--;
                drawMyScene();
            }
            break;
        case SDLK_SPACE:
			currentView = DANS_LE_TUBE;
			drawMyScene();
            break;
        case SDLK_x:
			currentView = SCENE_ENTIERE;
			drawGLScene();
			break;
		case SDLK_v:
			currentView = VAISSEAU;
			drawMyVaisseau();
			break;
        case SDLK_LEFT:
			//Translation gauche sur "J"
			
			break;
		case SDLK_RIGHT:
			//Translation droite sur "J"
			break;
		case SDLK_z:
			//Translation montante sur "I"
			break;
		case SDLK_s:
			//Translation descendante sur "I"
			break;
		case SDLK_q:
			//rotation gauche sur "v"
			break;
		case SDLK_d:
			//rotation droite sur "v"
			break;
		case SDLK_j:
			//rotation gauche sur "I"
			break;
		case SDLK_l:
			//rotation droite sur "I"
			break;
		case SDLK_i:
			//rotation avant sur "J"
			break;
		case SDLK_k:
			//rotation arriére sur "J"
			break;
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
        if (currentView == SCENE_ENTIERE) {
            drawGLScene();
        }
        if (currentView == VAISSEAU) {
            drawMyVaisseau();
        }
    }

    /* Should never get here */
    return ( 0);
}

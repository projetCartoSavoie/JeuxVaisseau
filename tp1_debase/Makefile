# Décomenter une de ces trois lignes

#GC=USE_TINY    # utiliser le mini GC fourni
GC=USE_BOEHM   # utiliser le GC de Boehm si installé
#GC=NO_GC       # pas de GC (toute la mémoire ne sera pas libéré.

OSX=0 # mettre 1 sur Mac

ifeq '$(strip $(GC))' 'USE_TINY'
GC_SRC=tiny_gc.c
else
GC_SRC=
endif

ifeq '$(strip $(GC))' 'USE_BOEHM'
GC_LIB=-lgc
else
GC_LIB=
endif

SRCS=$(GC_SRC) triangulation.c geometry.c array.c triangulation_tools.c tests.c main.c
OBJS=$(SRCS:.c=.o)
INCLUDES=
CC=gcc

ifeq '$(strip $(OSX))' '0'
CFLAGS= $(INCLUDES) -Wall -g -D $(GC)
LINKFLAGS=$(GC_LIB) -lm -lSDL2 -lGL -lGLU
FRAMEWORK=
else
CFLAGS= $(INCLUDES) -Wall -g -D $(GC) -fnested-functions
LINKFLAGS=$(GC_LIB) -lm -lSDL2
FRAMEWORK = -framework GLUT
FRAMEWORK += -framework OpenGL
FRAMEWORK +=  SDLmain.m -framework SDL -framework Cocoa
endif

.PHONY: depend clean

main.exe: $(OBJS)
	gcc $(FRAMEWORK) $(CFLAGS) -o $@ $(OBJS) $(LINKFLAGS)

depend:
	gcc -M $(CFLAGS) $(SRCS) > .depend

clean:
	- rm main.exe $(OBJS) *~ #*#

include .depend

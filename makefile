test: test.o
	gcc -o test test.o -lSDL2 -lGL -lGLU -lgc

hello.o: hello.c
	gcc -o test.o test.c -Wall

clean:
	rm -rf *.o
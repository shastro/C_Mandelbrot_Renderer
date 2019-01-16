CC=gcc
CFLAGS=-W -Wall -g
SDL_CFLAGS := $(shell sdl2-config --cflags) 
SDL_LDFLAGS := $(shell sdl2-config --libs) -lm 

all: main sdl_handler.o mandelbrot.o

#sdl_handler.o: sdl_handler.c sdl.h
#	$(CC) $(CFLAGS) -c sdl_handler.c

mandelbrot.o: mandelbrot.c
	$(CC) $(CFLAGS) -c mandelbrot.c -lm

main: sdl_handler.o mandelbrot.o
	$(CC) $(CFLAGS) $(SDL_CFLAGS) main.c sdl_handler.o mandelbrot.o -o mandl $(SDL_LDFLAGS)

clean:
	rm mandl sdl_handler.o mandelbrot.o

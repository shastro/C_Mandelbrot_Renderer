CC=gcc
CFLAGS=-W -Wall -g
SDL_CFLAGS := $(shell sdl2-config --cflags) 
SDL_LDFLAGS := $(shell sdl2-config --libs) -lm -pthread

all: main mandelbrot.o

#sdl_handler.o: sdl_handler.c sdl.h
#	$(CC) $(CFLAGS) -c sdl_handler.c

mandelbrot.o: mandelbrot.c mandelbrot.h
	$(CC) $(CFLAGS) -c mandelbrot.c -lm

main: mandelbrot.o mandelbrot.h
	$(CC) $(CFLAGS) $(SDL_CFLAGS) main.c mandelbrot.o -o mandl $(SDL_LDFLAGS)

clean:
	rm mandl mandelbrot.o

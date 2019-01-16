#include <stdio.h>
#include <stdlib.h>
#ifndef SDL_H
#define SDL_H
#include "SDL2/SDL.h"


/*Struct to Hold Information needed for SDL Initialization */
struct SDL_Init_Info{

	char *title;
	unsigned int width;
	unsigned int height;
	

};			

#endif
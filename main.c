#include "sdl.h"
#include "mandelbrot.h"
#include <time.h>
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

int main(int argc, char *args[]){

	SDL_Window *window = NULL;

	if( SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("SDL Could Not Initialize! Error: %s\n", SDL_GetError());
		exit(1);
	}

	//Create Window with Some Parameters, SDL_WINDOW_SHOW Flag is set to make sure the window is shown when created
	window = SDL_CreateWindow("Mandelbrot Renderer with SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if( window == NULL){
		printf("Window Could Not Be Created!: Error: %s", SDL_GetError());
		exit(1);
	}

	//Gets creates SDL Renderer and Texture
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH,SCREEN_HEIGHT);
	
	//Pixel Buffer Declaration
	uint32_t *pixel_buffer = malloc(sizeof(uint32_t) * SCREEN_HEIGHT * SCREEN_WIDTH);

	//Initializes Pixel Values to White
	memset(pixel_buffer, 0xFF, SCREEN_WIDTH * SCREEN_HEIGHT *sizeof(uint32_t));

	//Complex Number and Data array declarations and heap allocation
	//struct Complex_n *complex_array = (struct Complex_n*)malloc(sizeof(struct Complex_n) * SCREEN_WIDTH * SCREEN_HEIGHT);
	//struct Complex_n_bin *complex_bin_array = (struct Complex_n_bin*)malloc(sizeof(struct Complex_n_bin) * SCREEN_WIDTH * SCREEN_HEIGHT);

	//Default Arg Values
	double zoomfac = 2;
	double xoff = 0; // X offset
	double yoff = 0; // Y offset
	int num_iterations = 500;
	int red_bias = 5;
	int green_bias = 5;
	int blue_bias = 5;
	//Commandline args
	//This implementation is garbage and will be updated eventually
	if(args[1])
		sscanf(args[1],"%lf", &zoomfac);
	if(args[2])
		sscanf(args[2], "%d", &num_iterations);
	if(args[3])
		sscanf(args[3],"%lf", &xoff);
	if(args[4])
		sscanf(args[4], "%lf", &yoff);
	if(args[5])
		sscanf(args[5], "%d", &red_bias);
	if(args[6])
		sscanf(args[6], "%d", &green_bias);
	if(args[7])
		sscanf(args[7], "%d", &blue_bias);

	struct Mandel_Data *man_d = malloc(sizeof(struct Mandel_Data));
		man_d -> complex_bin_array = (struct Complex_n_bin*)malloc(sizeof(struct Complex_n_bin) * SCREEN_WIDTH * SCREEN_HEIGHT);
		man_d -> complex_array = (struct Complex_n*)malloc(sizeof(struct Complex_n) * SCREEN_WIDTH * SCREEN_HEIGHT);

	
	struct Mandel_Input *man_i = malloc(sizeof(struct Mandel_Input));
		man_i->width  = SCREEN_WIDTH;
		man_i->height = SCREEN_HEIGHT;
		man_i->num_iterations = num_iterations;
		man_i->xoff = xoff;
		man_i->yoff = yoff;
		man_i->zoomfac = zoomfac;

	struct Color_Info *color_i = malloc(sizeof(struct Color_Info));
		color_i->red_bias   = red_bias;
		color_i->green_bias = green_bias;
		color_i->blue_bias  = blue_bias;


	clock_t start, end;
	start = clock();
	//Update Mandelbrot and store arraydata in Mandel_Data
	mandel_update(man_i, man_d);
	printf("Update Success\n");
	mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);

	printf("%p", pixel_buffer);
	end = clock();
	double time_taken = ((double) (end - start));
	//free(bin_cpy->sum);
	//free(bin_cpy->product);
	printf("Time Taken: %lf \n", time_taken);
	printf("Set Generated!\n");

	//*Main Loop
	int ended = 0;
	SDL_Event event; //Declare Event
	while(!ended){

		SDL_UpdateTexture(texture, NULL, pixel_buffer, SCREEN_WIDTH*sizeof(uint32_t));
		//Poll for SDL Events
		while (SDL_PollEvent(&event)) 
		{
			switch (event.type) 
			{
			case SDL_MOUSEBUTTONDOWN:
				switch(event.button.button){
					case SDL_BUTTON_LEFT:
						man_i->xoff = l_map(event.button.x, 0.0f, SCREEN_WIDTH, -1 * man_i -> zoomfac + man_i->xoff, 1 * man_i -> zoomfac + man_i->xoff);
						man_i->yoff = l_map(event.button.y, 0.0f, SCREEN_HEIGHT,-1 * man_i -> zoomfac + man_i->yoff, 1 * man_i -> zoomfac + man_i->yoff);
						mandel_update(man_i, man_d);
						mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
						break;
				}
				break;
			case SDL_KEYDOWN:
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_ESCAPE){
					ended = 1;
				}
				if(event.key.keysym.sym == SDLK_1){
					color_i->red_bias += 1;
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
				}
				if(event.key.keysym.sym == SDLK_2){
					color_i->green_bias += 1;
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
				}
				if(event.key.keysym.sym == SDLK_3){
					color_i->blue_bias += 1;
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
				}
				if(event.key.keysym.sym == SDLK_q){
					color_i->red_bias -= 1;
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
				}
				if(event.key.keysym.sym == SDLK_w){
					color_i->green_bias -= 1;
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
				}
				if(event.key.keysym.sym == SDLK_e){
					color_i->blue_bias -= 1;
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
				}
				if(event.key.keysym.sym == SDLK_RETURN){
					man_i->zoomfac *= 0.1;
					mandel_update(man_i, man_d);
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);

				}
				break;
			case SDL_QUIT:
				ended = 1;
			}
		}
		SDL_RenderClear(renderer); //Clears image
		SDL_RenderCopy(renderer, texture, NULL, NULL); //Copies Texture to the renderer
		SDL_RenderPresent(renderer); //Renders new image
	}

	/*Cleanup */

	free(man_d->complex_bin_array);
	free(man_d->complex_array);
	free(man_d);
	free(man_i);
	free(color_i);
	free(pixel_buffer);
	if(texture)
	SDL_DestroyTexture(texture);
	if(renderer)
	SDL_DestroyRenderer(renderer);
	if(window)
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
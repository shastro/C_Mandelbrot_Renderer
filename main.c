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
	uint32_t pixel_buffer[SCREEN_HEIGHT*SCREEN_WIDTH];

	//Initializes Pixel Values to White
	memset(pixel_buffer, 0xFF, SCREEN_WIDTH * SCREEN_HEIGHT *sizeof(uint32_t));

	//Complex Number and Data array declarations and heap allocation
	struct Complex_n *complex_array = (struct Complex_n*)malloc(sizeof(struct Complex_n) * SCREEN_WIDTH * SCREEN_HEIGHT);
	struct Complex_n_bin *complex_bin_array = (struct Complex_n_bin*)malloc(sizeof(struct Complex_n_bin) * SCREEN_WIDTH * SCREEN_HEIGHT);

	//Array Data Initialization
	for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++){
		complex_setval(&complex_array[i], 0, 0);
		bin_allocate(complex_bin_array + i);
		complex_bin_array[i].in_set = 0;
		complex_bin_array[i].i = 0;
	}

	//Default Arg Values
	double zoomfac = 2;
	double xoff = 0; // X offset
	double yoff = 0; // Y offset
	int num_iterations = 500;
	//struct Complex_n_bin *bin_cpy = malloc(sizeof(struct Complex_n_bin*)); //Allocate memory to store a copy of the current data
	struct Complex_n_bin *bin_cpy = NULL;
	//Commandline args
	if(args[1])
		sscanf(args[1],"%lf", &zoomfac);
	if(args[2])
		sscanf(args[2], "%d", &num_iterations);
	if(args[3])
		sscanf(args[3],"%lf", &xoff);
	if(args[4])
		sscanf(args[4], "%lf", &yoff);

	//Loop that colorizes and Calculates values for each pixel based on if it is included withing hte mandlebrot set
	//Update Mandelbrot
	clock_t start, end;
	start = clock();
	uint32_t color = 0xFFFFFFFF;
	double normalized_color = 255;
	for (int x = 0; x < SCREEN_WIDTH; x++){

		for(int y = 0; y < SCREEN_HEIGHT; y++){

			// Maps the pixel range to a much smaller range for viewing of the set, linear scaling of coordinate space
			complex_array[y * SCREEN_WIDTH + x].rc = l_map(x, 0.0f, SCREEN_WIDTH, -2 + xoff, 2 + xoff) * zoomfac;
			complex_array[y * SCREEN_WIDTH + x].ic = l_map(y, 0.0f, SCREEN_HEIGHT, -2 + yoff, 2 + yoff) * zoomfac;
			
			//Calculates fractal using set_iterate, saves in an extra variable so it can be transferred to the bin_array
			bin_cpy = set_iterate(&complex_array[y * SCREEN_WIDTH + x], num_iterations, &complex_bin_array[y * SCREEN_WIDTH + x]);
			complex_bin_array[y * SCREEN_WIDTH + x] = *bin_cpy;

			//Set Pixel Color According to index that the complex number landed on in the loop 
			normalized_color = l_map(complex_bin_array[y * SCREEN_WIDTH + x].i, 0, num_iterations, 0, 1);
			pixel_buffer[(y * SCREEN_WIDTH) + x] = l_map(normalized_color, 0, 1, 0, 0xFF0000FF);
			//Has the effect of making the central shapes of the set black in color
			if(complex_bin_array[y * SCREEN_WIDTH + x].in_set == 1){
				pixel_buffer[y * SCREEN_WIDTH + x] = 0;
			}
			
			pixel_buffer[(SCREEN_WIDTH * SCREEN_HEIGHT)/2 + SCREEN_WIDTH/2] = color;


		}

	}
	end = clock();
	double time_taken = ((double) (end - start));
	//free(bin_cpy->sum);
	//free(bin_cpy->product);
	printf("Time Taken: %lf \n", time_taken);
	printf("Set Generated!\n");

	//*Main Loop
	int ended = 1;
	SDL_Event event; //Declare Event
	while(ended){

		SDL_UpdateTexture(texture, NULL, pixel_buffer, SCREEN_WIDTH*sizeof(uint32_t));
		//Poll for SDL Events
		while (SDL_PollEvent(&event)) 
		{
			switch (event.type) 
			{
			case SDL_KEYDOWN:
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_ESCAPE){
					ended = 0;
				}
				break;
			case SDL_QUIT:
				ended = 0;
			}
		}
		SDL_RenderClear(renderer); //Clears image
		SDL_RenderCopy(renderer, texture, NULL, NULL); //Copies Texture to the renderer
		SDL_RenderPresent(renderer); //Renders new image
	}

	/*Cleanup */
	//bin_free(bin_cpy);
	for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++){
		//free(&complex_bin_array[i]);
		//free(complex_bin_array[i].product);
		//free(complex_bin_array[i].z); //Doesn't work because the address of z is identical to the sum or the product
	}
	free(complex_bin_array);
	free(complex_array);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
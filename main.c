#include "sdl.h"
#include "mandelbrot.h"
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
#define NANO_PER_SEC 1000000000.0

/* MandelBrot Set Plotter, Skyler Hughes

	@bug
		Saved images have alpha of 0 where color should be pure black, 
		occurs on kbuntu for unkown reasons

	@todo
		Help and Welcome Message!

		Implement even better color control
		Implement custom movement optimzation algorithm
		Implement 2-2 loop detection and skiping optimization
		Implement dispersion

		Update Readme with installation and other information

		Better command line arguments
		Check Memeory leaks and add more free()s
		Document Code
		Organize Code

*/


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
	long double xoff = 0; // X offset
	long double yoff = 0; // Y offset
	int num_iterations = 500;
	int red_bias = 25;
	int green_bias = 25;
	int blue_bias = 25;
	int color_coef = 50;
	//Commandline args
	//This implementation is garbage and will be updated eventually
	if(args[1])
		sscanf(args[1],"%lf", &zoomfac);
	if(args[2])
		sscanf(args[2], "%d", &num_iterations);
	if(args[3])
		sscanf(args[3],"%Lf", &xoff);
	if(args[4])
		sscanf(args[4], "%Lf", &yoff);
	if(args[5])
		sscanf(args[5], "%d", &red_bias);
	if(args[6])
		sscanf(args[6], "%d", &green_bias);
	if(args[7])
		sscanf(args[7], "%d", &blue_bias);
	if(args[8])
		sscanf(args[8], "%d", &color_coef);

	//Initializations and Allocations
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
		color_i->color_coef = color_coef;

	//Multithreading
	pthread_t  threads[N_THREADS];
	struct Thread_Args *th_args_a = malloc(sizeof(struct Thread_Args) * N_THREADS);
	for(int i = 0; i < N_THREADS; i++){
		th_args_a[i].pixel_buffer = pixel_buffer;
		th_args_a[i].len = SCREEN_WIDTH * SCREEN_HEIGHT;
		th_args_a[i].man_i = man_i;
		th_args_a[i].man_d = man_d;
		th_args_a[i].thread_id = 0;
	}

	//*******************************************

	struct timespec start, end;
	double start_sec, end_sec;
	//Create Threads and Execute processes
	
	clock_gettime(CLOCK_REALTIME, &start);
	//mandel_update(man_i, man_d); //Single Thread
	update_mandel_create_threads(th_args_a, threads); //Multi-Thread

	clock_gettime(CLOCK_REALTIME, &end);

	start_sec = start.tv_sec + start.tv_nsec/NANO_PER_SEC;
	end_sec   = end.tv_sec + end.tv_nsec/NANO_PER_SEC;

	double time_taken = end_sec - start_sec;

	printf("Time Taken: %lf seconds\n", time_taken);
	mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
	/*
	//Single Thread
	clock_t start, end;
	start = clock();
	//Update Mandelbrot and store arraydata in Mandel_Data
	mandel_update(man_i, man_d);
	mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
	end = clock();
	double time_taken = ((double) ((end - start) / CLOCKS_PER_SEC));
	*/
	printf("Set Generated!\n");
	printf("Welcome!\n");
	printf("Press h for keybinds and help\n");
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
						update_mandel_create_threads(th_args_a, threads);
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
					print_Color_Info(color_i);
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
				}

				if(event.key.keysym.sym == SDLK_2){
					color_i->green_bias += 1;
					print_Color_Info(color_i);
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
				}

				if(event.key.keysym.sym == SDLK_3){
					color_i->blue_bias += 1;
					print_Color_Info(color_i);
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
				}

				if(event.key.keysym.sym == SDLK_q){
					color_i->red_bias -= 1;
					print_Color_Info(color_i);
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
				}

				if(event.key.keysym.sym == SDLK_w){
					color_i->green_bias -= 1;
					print_Color_Info(color_i);
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
				}

				if(event.key.keysym.sym == SDLK_e){
					color_i->blue_bias -= 1;
					print_Color_Info(color_i);
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
				}

				if(event.key.keysym.sym == SDLK_4){
					color_i->color_coef += 5;
					print_Color_Info(color_i);
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);					
				}

				if(event.key.keysym.sym == SDLK_r){
					color_i->color_coef -= 5;
					print_Color_Info(color_i);
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);					
				}

				if(event.key.keysym.sym == SDLK_EQUALS){
					man_i->zoomfac *= 0.1;
					//mandel_update(man_i, man_d);
					update_mandel_create_threads(th_args_a, threads);
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);

				}
				if(event.key.keysym.sym == SDLK_MINUS){
					man_i->zoomfac *= 10;
					//mandel_update(man_i, man_d);
					update_mandel_create_threads(th_args_a, threads);
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);

				}

				if(event.key.keysym.sym == SDLK_t){
					color_i->red_bias = 25;
					color_i->green_bias = 25;
					color_i->blue_bias = 25;
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);

				}

				if(event.key.keysym.sym == SDLK_UP){
					man_i->num_iterations += 50;
					print_Mandel_Input(man_i);
					
				}

				if(event.key.keysym.sym == SDLK_DOWN){
					if(man_i->num_iterations > 50)
						man_i->num_iterations -= 50;
					print_Mandel_Input(man_i);
				}

				if(event.key.keysym.sym == SDLK_p){
					print_Color_Info(color_i);
					print_Mandel_Input(man_i);
					print_cmd(man_i, color_i);
				}

				//Screenshot and metadata
				if(event.key.keysym.sym == SDLK_s){

					//Aquiring Pixel Data
					SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 24, SDL_PIXELFORMAT_RGBA8888);
					SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888, surface->pixels, surface->pitch);
					
					//Getting Number for Filenames
					clock_gettime(CLOCK_REALTIME, &end);
					int ts = end.tv_sec + end.tv_nsec/NANO_PER_SEC;
					ts %= 10000;

				
					char filepath[256];
					char filepath_cpy[256];

					sprintf(filepath, "./images/");
					sprintf(filepath_cpy, "./images/");

					char screenshot_name[256];
					sprintf(screenshot_name, "%d", ts);
					strcat(screenshot_name, ".bmp");
					strcat(filepath, screenshot_name);

					char meta_name[256];
					sprintf(meta_name, "%d", ts);
					strcat(meta_name, ".met");
					strcat(filepath_cpy, meta_name);

					//Saving Image
					SDL_SaveBMP(surface, filepath);
					SDL_FreeSurface(surface);

					//Write Metadata to .txt file
					FILE *metadata;
					metadata = fopen(filepath_cpy, "a");
					fprintf(metadata, "Red_Bias:   %d\n", color_i->red_bias);
					fprintf(metadata, "Green_Bias: %d\n", color_i->green_bias);
					fprintf(metadata, "Blue_Bias:  %d\n", color_i->blue_bias);
					fprintf(metadata, "Color Coefficient:   %d\n", color_i->color_coef); 
					fprintf(metadata, "iterations: %d\n", man_i->num_iterations);
					fprintf(metadata, "xoff: %lf\n", man_i->xoff);
					fprintf(metadata, "yoff: %lf\n", man_i->yoff);
					fprintf(metadata, "zoomfac: %lf\n", man_i->zoomfac);	
					fprintf(metadata, "./mandl %0.17g %d %0.17g %0.17g %d %d %d %d\n", man_i->zoomfac, man_i->num_iterations, man_i->xoff, man_i->yoff,
											color_i->red_bias, color_i->green_bias, color_i->blue_bias, color_i->color_coef);
 

					printf("Screenshot Saved! as %s\n", screenshot_name);
					printf("Metadata stored as %s\n", meta_name);

					//Store command in command_repositiory
					FILE *cool_commands;
					cool_commands = fopen("cool_commands", "a");
					if(cool_commands){

						fprintf(cool_commands, "./mandl %0.17g %d %0.17g %0.17g %d %d %d %d\n", man_i->zoomfac, man_i->num_iterations, man_i->xoff, man_i->yoff,
								color_i->red_bias, color_i->green_bias, color_i->blue_bias, color_i->color_coef);
 						
 						fclose(cool_commands);
 					}
					fclose(metadata);
				}


				if(event.key.keysym.sym == SDLK_RETURN){
					update_mandel_create_threads(th_args_a, threads);
					mandel_draw(pixel_buffer, man_d->complex_bin_array, color_i, man_i);
					printf("Refreshed!\n");					
				}

				if(event.key.keysym.sym == SDLK_h){
					print_help();

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
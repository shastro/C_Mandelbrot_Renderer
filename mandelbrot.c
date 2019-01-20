#include "mandelbrot.h"

/* Functions for Handling Mandelbrot Fractal Math and Generations */

/* Linearly Maps a given value within a given range to a new value within a new range */
double l_map(double x, double input_start, double input_end, double output_start, double output_end)
{
	if (input_end == input_start){

		printf("Error in l_map(), Cannot Map a value when input_end == input_start! \n");
		return -1.0;

	}else {

		double slope = (output_end - output_start) / (input_end - input_start);
		return output_start + slope*(x - input_start);
		
	}
}

int cindex(int x, int y, int width)
{
	return ((y * width) + x);
}

struct Complex_n *complex_square(struct Complex_n *a, struct Complex_n_bin *bin)
{
	bin->product->rc = ((a->rc)*(a->rc)) + (((a->ic)*(a->ic))*-1);
	bin->product->ic = 2 * a->rc * a->ic;

	return bin->product;
}

void complex_setval(struct Complex_n *a, double rc, double ic)
{
	a->rc = rc;
	a->ic = ic;
}

struct Complex_n *complex_add(struct Complex_n *a, struct Complex_n *b, struct Complex_n_bin *bin)
{
	bin->sum->rc = a->rc + b->rc;
	bin->sum->ic = a->ic + b->ic;

	return bin->sum;
}

double complex_abs(struct Complex_n *a)
{
	//return pow((a->rc)*(a->rc) + (a->ic)*(a->ic), 0.5); //Much Slower
	return sqrt((a->rc)*(a->rc) + (a->ic)*(a->ic));
}

void bin_allocate(struct Complex_n_bin *bin)
{
	bin->sum = (struct Complex_n *)malloc(sizeof(struct Complex_n*));
	bin->product = (struct Complex_n *)malloc(sizeof(struct Complex_n*));	
}

void bin_free(struct Complex_n_bin *bin)
{
	free(bin->sum);
	free(bin->product);
	//free(bin->z);
}

/*Performs iterations of z_n+1 = z + C  to determine if a given coordinate is within the mandelbrot set */
struct Complex_n_bin *set_iterate(struct Complex_n *a, int iterations, struct Complex_n_bin *bin)
{
	int i = 0;
	//struct Complex_n *z = (struct Complex_n *)malloc(sizeof(struct Complex_n*));
	struct Complex_n *z = bin->sum;
	complex_setval(z, 0, 0);
	complex_setval(bin->sum, 0, 0);
	complex_setval(bin->product, 0, 0); 

	for(; i < iterations; i++){
		z = complex_add(complex_square(z, bin), a, bin);
		//z = complex_add(complex_square(complex_square(z, bin), bin), a, bin); //Other Func
		if((z->rc * z->rc) + (z->ic * z->ic) > 4){
			bin->in_set = 0;
			bin->i = i;
			break;
		}
		if(i == iterations - 1){
			bin->in_set = 1;
			bin->i = i;
		}
	}

	free(z);
	free(bin->product);
	return bin;
}

void mandel_update(struct Mandel_Input *man_i, struct Mandel_Data *man_d){
	
	//Array Data Initialization
	for(int i = 0; i < (man_i->width * man_i->height); i++){
		complex_setval(((man_d->complex_array) + i), 0, 0);
		bin_allocate((man_d->complex_bin_array) + i);
		(man_d->complex_bin_array)[i].in_set = 0;
		(man_d->complex_bin_array)[i].i = 0;
		//printf("%d:%d", (man_d->complex_array + i)->rc ,(man_d->complex_array + i)->ic);
	}

	struct Complex_n_bin *bin_cpy = NULL;

	for (int x = 0; x < (man_i->width); x++){

		for(int y = 0; y < (man_i->height); y++){

			// Maps the pixel range to a much smaller range for viewing of the set, linear scaling of coordinate space
			((man_d->complex_array)[cindex(x,y, man_i->width)]).rc = l_map(x, 0.0f, man_i->width , -1 , 1 ) * man_i->zoomfac + man_i->xoff;
			((man_d->complex_array)[cindex(x,y, man_i->width)]).ic = l_map(y, 0.0f, man_i->height, -1 , 1 ) * man_i->zoomfac + man_i->yoff;

			//Calculates fractal using set_iterate, saves in an extra variable so it can be transferred to the bin_array
			bin_cpy = set_iterate(&((man_d->complex_array)[cindex(x,y, man_i->width)]), man_i->num_iterations, &((man_d->complex_bin_array)[cindex(x,y, man_i->width)]));
			(man_d->complex_bin_array)[cindex(x,y, man_i->width)] = *bin_cpy;

		}

	}

}

//Multithreaded MandelCalc
void *threaded_mandel_update(void *th_args){
	
	//Simplifiying the naming
	int thread_id = ((struct Thread_Args*)th_args)->thread_id;
	int width     = ((struct Thread_Args*)th_args)->man_i->width;
	//int height    = ((struct Thread_Args*)th_args)->man_i->height;
	int len       = ((struct Thread_Args*)th_args)->len;

	struct Mandel_Data  *man_d  = ((struct Thread_Args*)th_args)->man_d;
	struct Mandel_Input *man_i = ((struct Thread_Args*)th_args)->man_i;
    //printf("Thread%d --- ALIVE!1\n", thread_id);
	//Calculate Index Start and End
	int start_index = thread_id * (len/N_THREADS);
	int end_index = start_index + ((len/N_THREADS) - 1);
	if(((len % N_THREADS) > 0) && (thread_id == (N_THREADS - 1))){
		end_index += (len % N_THREADS);
	}
	 //printf("Thread%d --- ALIVE!2\n", thread_id);
	//Array Data Initialization
	for(int i = start_index; i <= end_index; i++){
		complex_setval(((man_d->complex_array) + i), 0, 0);
		bin_allocate((man_d->complex_bin_array) + i);
		(man_d->complex_bin_array)[i].in_set = 0;
		(man_d->complex_bin_array)[i].i = 0;
		//printf("%d:%d", (man_d->complex_array + i)->rc ,(man_d->complex_array + i)->ic);
	}
	 //printf("Thread%d --- ALIVE!3\n", thread_id);
	 //printf("Thread%d ---Start:%d\n", thread_id, start_index);
	 //printf("Thread%d ---End:%d\n", thread_id, end_index);
	struct Complex_n_bin *bin_cpy = NULL;

	for(int i = start_index; i <= end_index; i++){
		 	//printf("Thread%d --- ALIVE!4\n", thread_id);
			//printf("%d\n", i);
			// Maps the pixel range to a much smaller range for viewing of the set, linear scaling of coordinate space
			((man_d->complex_array)[i]).rc = l_map((i%width), 0.0f, man_i->width , -1 , 1 ) * man_i->zoomfac + man_i->xoff;
			((man_d->complex_array)[i]).ic = l_map((i/width), 0.0f, man_i->height, -1 , 1 ) * man_i->zoomfac + man_i->yoff;

			//Calculates fractal using set_iterate, saves in an extra variable so it can be transferred to the bin_array
			bin_cpy = set_iterate(&((man_d->complex_array)[i]), man_i->num_iterations, &((man_d->complex_bin_array)[i]));
			(man_d->complex_bin_array)[i] = *bin_cpy;

	}
	printf("Thread %d Successful!\n", thread_id);

	

}

void update_mandel_create_threads(struct Thread_Args *th_args_a, pthread_t *threads)
{
	for(int i=0; i< N_THREADS; i++){
		(th_args_a[i]).thread_id = i;
		printf("\n\nSpawning Thread%d", i);
		pthread_create(&threads[i], NULL, threaded_mandel_update, (void*)&th_args_a[i]);

	}
	//Wait for Threads to Complete
	for(int i=0; i< N_THREADS; i++){
		pthread_join(threads[i], NULL);
		printf("Thread %d finished!\n", i);
	}
}

void mandel_draw(uint32_t *pixel_buffer, struct Complex_n_bin *complex_bin_array, struct Color_Info *color_i, struct Mandel_Input *man_i)
{
	double normalized_color = 0;
	uint32_t color = 255;
	for (int x = 0; x < (man_i->width); x++){

		for(int y = 0; y < (man_i->height); y++){
		//Set Pixel Color According to cindex that the complex number landed on in the loop 
		normalized_color = l_map(complex_bin_array[y * man_i->width + x].i, 0, man_i->num_iterations, 0, 1);
		
		//Assign pixel color
		color = color_calc(normalized_color, color_i->red_bias, color_i->green_bias, color_i->blue_bias, color_i->color_coef);

		//Assign Color
		pixel_buffer[(y * man_i->width) + x] = color;

		//printf("%#8x\n", pixel_buffer[y * man_i->width + x]);

		//Has the effect of making the central shapes of the set black in color
		if(complex_bin_array[(y * man_i->width) + x].in_set == 1){
			pixel_buffer[(y * man_i->width) + x] = 0;
		}
		
		//pixel_buffer[(man_i->width * man_i->height)/2 + man_i->width/2] = 0xFFFFFFFF;

		}
	}
}


//Takes in double between zero and one
uint32_t color_calc(double val, int red_bias, int green_bias, int blue_bias, int coef)
{
	if((val < 0.0) | (val > 1.0)){
		printf("Error in color_calc! Val not normalized!");
		return 1;
	}
	uint32_t red   = 0x00000000;
	uint32_t green = 0x00000000;
	uint32_t blue  = 0x00000000;
	uint32_t alpha = 0x00000000;


	red   = ((int)l_map(val, 0, 1, 0, coef * red_bias))   << (6 * 4);//Starts as 0x000000FF 
	green = ((int)l_map(val, 0, 1, 0, coef * green_bias)) << (4 * 4);//Multiply by 4 to shift by half-bytes instead of bits
	blue  = ((int)l_map(val, 0, 1, 0, coef * blue_bias))  << (2 * 4);
	alpha = 0xFF;

	/*Experimental smooth interpolation 
	red = ((int)((0.5 * sin(val) + 1) * 0xff * red_bias) << (6 * 4));
	green = ((int)((0.5 * sin(val) + 1) * 0xff * green_bias) << (4 * 4));
	blue = ((int)((0.5 * sin(val) + 1) * 0xff * blue_bias) << (2 * 4));	
	alpha = 0xff;
	*/
	//printf("%#8x\n", red);
	//printf("%#8x\n", (red + green + blue + alpha));
	return (red + green + blue + alpha);
}




//Prints Color_Info Struct
void print_Color_Info(struct Color_Info *color_i)
{
	printf("Red_Bias:   %d\n", color_i->red_bias);
	printf("Green_Bias: %d\n", color_i->green_bias);
	printf("Blue_Bias:  %d\n", color_i->blue_bias);
	printf("Color Coefficient: %d\n", color_i->color_coef);
}

//Prints Mandel_Input Struct
void print_Mandel_Input(struct Mandel_Input *man_i)
{
	printf("iterations: %d\n", man_i->num_iterations);
	printf("xoff: %lf\n", man_i->xoff);
	printf("yoff: %lf\n", man_i->yoff);
	printf("zoomfac: %lf\n", man_i->zoomfac);
}

//Prints Executable command to console
void print_cmd(struct Mandel_Input *man_i, struct Color_Info *color_i)
{
	printf("\"./mandl %0.17g %d %0.17g %0.17g %d %d %d %d\"\n\n\n", man_i->zoomfac, man_i->num_iterations, man_i->xoff, man_i->yoff, 
										color_i->red_bias, color_i->green_bias, color_i->blue_bias, color_i->color_coef);
}
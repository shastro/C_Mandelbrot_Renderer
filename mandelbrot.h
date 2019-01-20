#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#define N_THREADS 8
struct Complex_n{

	double rc; //Real Component
	double ic; //Imaginary Component
};
//Used to store the iterations it took for a given complex number to either be included or not included in the mandelbrot set
//As well as some pointers to sum, product, and z complex numbers that need to be passed by reference to the various funcs
//in mandelbrot.c
struct Complex_n_bin{

	struct Complex_n *sum;
	struct Complex_n *product;
	struct Complex_n *z;
	short int in_set;
	int i;
};

//Struct to hold all mandelbrot data for entire set
struct Mandel_Data{

	struct Complex_n_bin *complex_bin_array;
	struct Complex_n *complex_array;

};

//Holds input data for mandel_update()
struct Mandel_Input{
	int width;
	int height;
	int num_iterations;
	double xoff;
	double yoff;
	double zoomfac;

};

//Holds information that must be passed to each thread
struct Thread_Args{

	struct Mandel_Input *man_i;
	struct Mandel_Data *man_d;
	uint32_t *pixel_buffer;
	int len;
	int thread_id;
};

//Color information for rendering
struct Color_Info{

	int red_bias;
	int green_bias;
	int blue_bias;
	int color_coef;

};


//Function Declarations

//Util
double l_map(double x, double input_start, double input_end, double output_start, double output_end);
int cindex(int x, int y, int width);

//Complex Num 
void complex_setval(struct Complex_n *a, double rc, double ic);
struct Complex_n *complex_square(struct Complex_n *a, struct Complex_n_bin *bin);
struct Complex_n *complex_add(struct Complex_n *a, struct Complex_n *b, struct Complex_n_bin *bin);
double complex_abs(struct Complex_n *a);
struct Complex_n_bin *set_iterate(struct Complex_n *a, int iterations, struct Complex_n_bin *bin);

//Memory Managment
void bin_allocate(struct Complex_n_bin *bin);
void bin_free(struct Complex_n_bin *bin);

//Mandelbrot Calc
void mandel_update(struct Mandel_Input *man_i, struct Mandel_Data *man_d);
void mandel_draw(uint32_t *pixel_buffer, struct Complex_n_bin *complex_bin_array, struct Color_Info *color_i, struct Mandel_Input *man_i);

//Working with Color
uint32_t color_calc(double val, int red_bias, int green_bias, int blue_bias, int coef);

//Prints
void print_Color_Info(struct Color_Info *color_i);
void print_Mandel_Input(struct Mandel_Input *man_i);
void print_cmd(struct Mandel_Input *man_i, struct Color_Info *color_i);
void print_help(void);

//Multi-Threaded
void *threaded_mandel_update(void *th_args);
void update_mandel_create_threads(struct Thread_Args *th_args_a, pthread_t *threads);
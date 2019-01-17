#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
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


//Function Declarations
double l_map(double x, double input_start, double input_end, double output_start, double output_end);
void complex_setval(struct Complex_n *a, double rc, double ic);
struct Complex_n *complex_square(struct Complex_n *a, struct Complex_n_bin *bin);
struct Complex_n *complex_add(struct Complex_n *a, struct Complex_n *b, struct Complex_n_bin *bin);
double complex_abs(struct Complex_n *a);
struct Complex_n_bin *set_iterate(struct Complex_n *a, int iterations, struct Complex_n_bin *bin);
void bin_allocate(struct Complex_n_bin *bin);
void bin_free(struct Complex_n_bin *bin);

uint32_t color_calc(double val);
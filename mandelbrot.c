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
	for(i; i < iterations; i++){
		z = complex_add(complex_square(z, bin), a, bin);
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

//Takes in double between zero and one
uint32_t color_calc(double val)
{
	if((val < 0.0) | (val > 1.0)){
		printf("Error in color_calc! Val not normalized!");
		return 1;
	}
	uint32_t red   = 0x00000000;
	uint32_t green = 0x00000000;
	uint32_t blue  = 0x00000000;
	uint32_t alpha = 0x00000000;

	red   = ((int)l_map(val, 0, 1, 0, 0xff * 1)) << (6 * 4);//= (/*(int)l_map(val, 0, 1, 0, 255)*/0xff) << 6; //Starts as 0x000000FF 
	green = ((int)l_map(val, 0, 1, 0, 0xff * 1)) << (4 * 4);//Multiply by 4 to shift by half-bytes instead of bits
	blue  = ((int)l_map(val, 0, 1, 0, 0xff * 1)) << (2 * 4);
	alpha = 0xFF;
	//printf("%#8x\n", red);
	//printf("%#8x\n", (red + green + blue + alpha));
	return (red + green + blue + alpha);
}
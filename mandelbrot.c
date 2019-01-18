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

	//free(z);
	//free(bin->product);
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
			//printf("RC:%lf\n\n", ((man_d->complex_array)[cindex(x,y, man_i->width)]).rc);
			//printf("IC:%lf\n\n", ((man_d->complex_array)[cindex(x,y, man_i->width)]).ic);
			//printf("%lf\n", man_i->zoomfac);
			//
			//bin_cpy = set_iterate(&complex_array[y * SCREEN_WIDTH + x], num_iterations, &complex_bin_array[y * SCREEN_WIDTH + x]);
			//complex_bin_array[y * SCREEN_WIDTH + x] = *bin_cpy;

			//Calculates fractal using set_iterate, saves in an extra variable so it can be transferred to the bin_array
			bin_cpy = set_iterate(&((man_d->complex_array)[cindex(x,y, man_i->width)]), man_i->num_iterations, &((man_d->complex_bin_array)[cindex(x,y, man_i->width)]));
			(man_d->complex_bin_array)[cindex(x,y, man_i->width)] = *bin_cpy;

		}

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
		color = color_calc(normalized_color, color_i->red_bias, color_i->green_bias, color_i->blue_bias);
		//color = 0xFFFFFFFF;

		//pixel_buffer[(y * man_i->width) + x] = l_map(normalized_color, 0, 1, 0, 0xFF0000FF);

		//Assign Color
		pixel_buffer[(y * man_i->width) + x] = color;

		//printf("%#8x\n", pixel_buffer[y * man_i->width + x]);

		//Has the effect of making the central shapes of the set black in color
		if(complex_bin_array[(y * man_i->width) + x].in_set == 1){
			pixel_buffer[(y * man_i->width) + x] = 0;
		}
		
		pixel_buffer[(man_i->width * man_i->height)/2 + man_i->width/2] = 0xFFFFFFFF;

		}
	}
}


//Takes in double between zero and one
uint32_t color_calc(double val, int red_bias, int green_bias, int blue_bias)
{
	if((val < 0.0) | (val > 1.0)){
		printf("Error in color_calc! Val not normalized!");
		return 1;
	}
	uint32_t red   = 0x00000000;
	uint32_t green = 0x00000000;
	uint32_t blue  = 0x00000000;
	uint32_t alpha = 0x00000000;


	red   = ((int)l_map(val, 0, 1, 0, 0xff * red_bias))   << (6 * 4);//Starts as 0x000000FF 
	green = ((int)l_map(val, 0, 1, 0, 0xff * green_bias)) << (4 * 4);//Multiply by 4 to shift by half-bytes instead of bits
	blue  = ((int)l_map(val, 0, 1, 0, 0xff * blue_bias))  << (2 * 4);
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
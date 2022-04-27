#include <stdio.h>
#include "image_template.h"
#include "math.h"
#include <sys/time.h>

void Gaussian_Kernel(float ** G_Kernel, int *w, float Sigma){
	float a;
	float sum = 0;
	a = round(2.5*Sigma - 0.5);
	*w = 2*a + 1;
	*G_Kernel = (float *)malloc(sizeof(float) *(*w)); 
	for(int i=0; i<*w; i++){
		(*G_Kernel)[i] = exp((-1*(i-a)*(i-a))/(2*Sigma*Sigma));
		sum += (*G_Kernel)[i];
	}
	for(int i=0; i<*w; i++){
		(*G_Kernel)[i] = (*G_Kernel)[i]/sum;
	} 
}


void Gaussian_Deri_Kernel(float ** GD_Kernel, int *w, float Sigma){
	float a, sum=0;
	float temp;
	a = round(2.5*Sigma - 0.5);
	*w = 2*a+1;
	*GD_Kernel = (float*)malloc(sizeof(float)*(*w));
	for(int i=0; i<*w; i++){
		(*GD_Kernel)[i] = -1*(i-a)*exp((-1*(i-a)*(i-a))/(2*Sigma*Sigma));
		sum -= i*(*GD_Kernel)[i];
	}
	for(int i=0; i<*w; i++){
		(*GD_Kernel)[i] = (*GD_Kernel)[i]/sum;
		printf("%f\n", (*GD_Kernel)[i]);
	}
	/*Kernel flipping*/ 
	for(int i=0; i<*w/2; i++){
		temp = (*GD_Kernel)[*w-1-i];
		(*GD_Kernel)[*w-1-i] = (*GD_Kernel)[i];
		(*GD_Kernel)[i] = temp;
	}
}


void Convolve(float *image, float *I, int width, int height, int ker_h, int ker_w, float *kernel){
	int offseti, offsetj;
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			float sum=0;
			for(int k=0;k<ker_h;k++){
				for(int m=0;m<ker_w;m++){
					offseti=-1*floor(ker_h/2)+k;
					offsetj=-1*floor(ker_w/2)+m;
					//
					if ( i+offseti>= 0 && i+offseti<height && j+offsetj >=0 && j+offsetj < width){
					sum += image[(i+offseti)*width+j+offsetj] * kernel[(k*ker_w+m)];
					}
				}
			}
			/* Output in a separate image */
			I[i*width+j] = sum;
		}
	}
}


void magnitude(float * mag, float *vertical, float *horizontal, int width, int height){
	for(int i=0; i<height; i++){
		for(int j=0; j<width;j++){
			mag[(i*width+j)] = sqrt(vertical[(i*width+j)]*vertical[(i*width+j)] + horizontal[(i*width+j)]*horizontal[(i*width+j)]);	
		}
	}
}


void direction(float * dir, float *vertical, float *horizontal, int width, int height){
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			dir[(i*width+j)] = atan2(vertical[(i*width+j)], horizontal[(i*width+j)]);
		}
	}
}


int main(int argc, char ** argv){
	int width, height;
	float *p; //image
	read_image_template<float>(argv[1], &p, &width, &height);
	/* start the time counter */
	struct timeval start, end;
	gettimeofday(&start, NULL);
	/* creat the Gaussian Kernel */
	int w;
	int gpunumber=atoi(argv[3])%4;

	/*Host Pointers*/ 
	float *G_Kernel;
	float *GD_Kernel;
	float *temp_horizontal, *horizontal;
	float *temp_vertical, *vertical;
	float *mag;
	float *dir;

	cudaSetDevice(gpunumber);

	/* TBD: Declare Device Pointers for the corresponding Host Pointers */

	/* TBD: Allocate Host Pointers using malloc */
	temp_horizontal = (float *)malloc(sizeof(float)*width*height);	
	horizontal = (float *)malloc(sizeof(float)*width*height);	
	temp_horizontal = (float *)malloc(sizeof(float)*width*height);	
	temp_vertical = (float *)malloc(sizeof(float)*width*height);	
	vertical = (float *)malloc(sizeof(float)*width*height);	
	mag = (float *)malloc(sizeof(float)*width*height);	
	dir = (float *)malloc(sizeof(float)*width*height);	

	/* TBD: Allocate Device Pointers using cudaMalloc */


	Gaussian_Kernel(&G_Kernel,&w,atof(argv[2]));
	/* creat the Gaussian Derivative Kernel */
	Gaussian_Deri_Kernel( &GD_Kernel, &w, atof(argv[2])); 
	
	/* Horizontal Gradient */ 
	Convolve(p, temp_horizontal, width, height, w, 1, G_Kernel);	
	Convolve(temp_horizontal, horizontal, width, height, 1, w, GD_Kernel);
	/* Vertical Gradient */
	Convolve(p, temp_vertical, width, height, 1, w, G_Kernel);
	Convolve(temp_vertical, vertical, width, height, w, 1, GD_Kernel);
	/* Magnitude */
	magnitude(mag, vertical, horizontal,width, height);
	/* Direction */
	direction(dir, vertical, horizontal,width, height);
	/* end the time counter */
	gettimeofday(&end, NULL);
	/* print the time results million second */
	printf("%ldus\n", ((end.tv_sec * 1000000 + end.tv_usec)
		  - (start.tv_sec * 1000000 + start.tv_usec)));
	/* export the results */ 
	char mname[] ="magnitude.pgm";
	char dname[] ="direction.pgm";
	write_image_template<float>(mname, mag, width, height);
	write_image_template<float>(dname, dir, width, height);
	
  	/* free the memory */
  	free(p);
  	free(G_Kernel);
  	free(GD_Kernel);
  	free(temp_horizontal);
  	free(horizontal);
  	free(temp_vertical);
  	free(vertical);
  	free(mag);
  	free(dir);
  	return 0;
	}

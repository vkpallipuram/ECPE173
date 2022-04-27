#include <stdio.h>
#include "image_template.h"
#include "image_processing.h"
#include <sys/time.h>
int main(int argc, char** argv) {
    
	// Check that image path and sigma are provided
	if( argc != 3 ) {
    	printf("Please provide an image path and sigma value in the following format:\ncanny_edge /path/to/file 0.5\n");
    	return 1;
   	}

   	// Get image path
   	char* imagePath = argv[1];
	struct timeval start,stop;

   	// Get sigma value
   	char* sigmaConversionError;
   	char* sigmaString = argv[2];
   	double sigma = strtod(sigmaString, &sigmaConversionError);
   	if (*sigmaConversionError != 0) {
   		printf("Fatal Error: Invalid sigma value provided. Must be numerial value\n");
   		return 1;
   	}

   	// Read image
    int width = 0;
    int height = 0;
    float* image = NULL;
    read_image_template(imagePath, &image, &width, &height);

    float* magnitude = NULL;
    float* gradient = NULL;
    float* temp_horizontal = NULL;
    float* horizontal = NULL;
    float* temp_vertical = NULL;
    float* vertical = NULL;
    magnitudeGradient(image, width, height, (float)sigma, &magnitude, &gradient, &temp_horizontal, &horizontal, &temp_vertical, &vertical);

    char magnitudeName[] = "magnitude.pgm";
    write_image_template(magnitudeName, magnitude, width, height);

    char gradientName[] = "gradient.pgm";
    write_image_template(gradientName, gradient, width, height);

    return 0;
}

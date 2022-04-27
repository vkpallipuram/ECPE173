#ifndef IMAGE_PROCESSING_H_
#define IMAGE_PROCESSING_H_

void flipArray(float* array, int width);
void gaussian(float sigma, float** kernel, float* width);
void gaussianDerivative(float sigma, float** kernel, float* width);
float* convolve(float* image, int imageWidth, int imageHeight, float* kernel, int kernelWidth, int kernelHeight); 
void magnitudeGradient(float* image, int imageWidth, int imageHeight, float sigma, float** magnitude, float** gradient, float** temp_horizontal, float** horizontal, float** temp_vertical, float** vertical);

#endif 

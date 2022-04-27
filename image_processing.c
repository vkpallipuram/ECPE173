#include "image_processing.h" 
#include <stdio.h>
#include <math.h>

void flipArray(float* array, int width) {

    // loop through array
    for(int i = 0; i < (width/2); i++) {
        // Swap values
        float temp = array[width - i - 1];
        array[width - i - 1] = array[i];
        array[i] = temp;
    }
}

void gaussian(float sigma, float** kernel, float* width) {
    
    float a = roundf(2.5 * sigma - 0.5);
    float w = 2 * a + 1;
    float* G = (float*)malloc(w * sizeof(float));
    float sum = 0;

    //printf("G_w: %f\n", w);

    for(int i = 0; i < w; i++) {
        G[i] = expf(-1.0 * powf((float)(i) - a, 2.0) / (2.0 * powf(sigma, 2.0)));
        sum += G[i];
    }

    // Normalize gaussian kernel peaks
    for(int i = 0; i < w; i++) {
        G[i] = G[i] / sum;
        //printf("G[%d]: %f\n", i, G[i]);
    }

    // Set results to pointers
    *kernel = G;
    *width = w;
}

void gaussianDerivative(float sigma, float** kernel, float* width) {
    
    float a = roundf(2.5 * sigma - 0.5);
    float w = 2 * a + 1;
    float* dG = (float*)malloc(w * sizeof(float));
    float sum = 0;

    //printf("dG_w: %f\n", w);

    for(int i = 0; i < w; i++) {
        dG[i] = -1.0 * ( (float)(i + 1) - 1.0 - a) * expf(-1.0 * powf((float)(i + 1) - 1.0 - a, 2.0) / (2.0 * powf(sigma, 2.0)));
        sum = sum - (i+1) * dG[i];
    }

    // Normalize gaussian kernel peaks
    for(int i = 0; i < w; i++) {
        dG[i] = dG[i] / sum;
        //printf("dG[%d]: %f\n", i, dG[i]);
    }

    // Set results to pointers
    *kernel = dG;
    *width = w;
}

float* convolve(float* image, int imageWidth, int imageHeight, float* kernel, int kernelWidth, int kernelHeight) {

    // Check the kernel height and width are odd so that it has a single center pixel
    if(kernelWidth % 2 == 0 || kernelHeight % 2 == 0) {
        printf("Fatal Error: Invalid kernel size... exiting");
        return NULL;
    }

    // Calculate kernel constants
    int kernelCenterX = (kernelWidth / 2) + 1;
    int kernelCenterY = (kernelHeight / 2) + 1;

    // Create output image
    float* outputImage = (float*) malloc(sizeof(float) * imageWidth * imageHeight);

    // Loop through each pixel of the image
    int x, y;
    for (x = 0; x < imageWidth; x++) {
        for (y = 0; y < imageHeight; y++) {
        
            // Set initial pixel value to zero
            outputImage[y * imageWidth + x] = 0;
        
            // Loop through each pixel of the kernel
            int i, j;
            for (i = 0; i < kernelWidth; i++) {
                for (j = 0; j < kernelHeight; j++) {
                
                    // Calculate offset based on current pixel in kernel
                    int xCalculated = x + (i - kernelCenterX);
                    int yCalculated = y + (j - kernelCenterY);
                    
                    // Check that pixel is not out of bounds
                    if(xCalculated < 0 || xCalculated >= imageWidth || yCalculated < 0 || yCalculated >= imageHeight) {
                        continue;
                    }

                    // Calculate part of the convolve value based on image and kernel pixel.
                    outputImage[y * imageWidth + x] += kernel[j * kernelWidth + i] * image[yCalculated * imageWidth + xCalculated];
                }
            }
        }
    }

    return outputImage;
}

void magnitudeGradient(float* image, int imageWidth, int imageHeight, float sigma, float** magnitude, float** gradient, float** temp_horizontal, float** horizontal, float** temp_vertical, float** vertical) {
    
    // Set initial magnitude and gradient arrays
    *magnitude = (float*) malloc(sizeof(int) * imageWidth * imageHeight);
    *gradient = (float*) malloc(sizeof(int) * imageWidth * imageHeight);

    // Generate gaussian kernel
    float GWidth = 0;
    float* G = NULL;
    gaussian(sigma, &G, &GWidth);

    // Generate gaussian kernel derivative 
    float dGWidth = 0;
    float* dG = NULL;
    gaussianDerivative(sigma, &dG, &dGWidth);
    flipArray(dG, dGWidth);
    
    //% Horizontal Intensity
    *temp_horizontal = convolve(image, imageWidth, imageHeight, G, 1, GWidth);
    *horizontal = convolve(*temp_horizontal, imageWidth, imageHeight, dG, dGWidth, 1); 
    
    //% Vertical Intensity Change:
    *temp_vertical = convolve(image, imageWidth, imageHeight, G, GWidth, 1);
    *vertical = convolve(*temp_vertical, imageWidth, imageHeight, dG, 1, dGWidth);

    // Loop through each pixel of the image
    int x, y;
    for (x = 0; x < imageWidth; x++) {
        for (y = 0; y < imageHeight; y++) {
            (*magnitude)[y * imageWidth + x] = sqrtf( powf( (*vertical)[y * imageWidth + x], 2.0) + powf( (*horizontal)[y * imageWidth + x], 2.0) ); // Euclidean
            (*gradient)[y * imageWidth + x] = atan2f( (*horizontal)[y * imageWidth + x], (*vertical)[y * imageWidth + x]); // Correction
        }
    }
}


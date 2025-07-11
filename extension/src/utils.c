#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "utils.h"
#include "plant_health.h"
#include "scoring_algorithm.h"
#include "texture_analysis.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


void pixel_to_image(unsigned char* pixels, int width, int height, 
                    int image_number) {
    int channels = 3;
    int stride = width * channels;
    char filename[64];
    if (image_number == 0) {
        snprintf(filename, sizeof(filename), 
             "leaf_no_bg.jpg");
    } else {
        snprintf(filename, sizeof(filename), 
             "images/output/leaf%02d.jpg", image_number);
    }
    stbi_write_jpg(filename, width, height, channels, pixels, stride);
}

void print_pixels(unsigned char* pixels, int width, int height, int max_rows, 
                  int max_cols) {
    printf("Image: %dx%d pixels\n", width, height);
    printf("Showing first %d rows and %d columns:\n\n", 
           (max_rows < height) ? max_rows : height,
           (max_cols < width) ? max_cols : width);
    
    // Print column headers
    printf("     ");
    for (int x = 0; x < width && x < max_cols; x++) {
        printf("   Col%-3d", x);
    }
    printf("\n");
    
    // Print pixels row by row
    for (int y = 0; y < height && y < max_rows; y++) {
        printf("R%2d: ", y);
        
        for (int x = 0; x < width && x < max_cols; x++) {
            int i = y * width + x;
            unsigned char r = pixels[i * 3];
            unsigned char g = pixels[i * 3 + 1];
            unsigned char b = pixels[i * 3 + 2];
            
            printf("(%3d,%3d,%3d) ", r, g, b);
        }
        printf("\n");
    }
    
    if (height > max_rows || width > max_cols) {
        printf("\n... (showing only first %d rows and %d columns)\n", 
               max_rows, max_cols);
    }
}

// Simpler version that just shows a sample of pixels
void print_pixel_sample(unsigned char* pixels, int total_pixels) {
    printf("Pixel sample (first 10 pixels):\n");
    for (int i = 0; i < 10 && i < total_pixels; i++) {
        unsigned char r = pixels[i * 3];
        unsigned char g = pixels[i * 3 + 1];
        unsigned char b = pixels[i * 3 + 2];
        printf("Pixel %d: R=%3d G=%3d B=%3d\n", i, r, g, b);
    }
    printf("\n");
}

// Print statistics about the pixel data
void print_pixel_stats(unsigned char* pixels, int total_pixels) {
    int zero_pixels = 0;
    int min_r = 255, max_r = 0, min_g = 255, max_g = 0, min_b = 255, max_b = 0;
    long sum_r = 0, sum_g = 0, sum_b = 0;
    
    for (int i = 0; i < total_pixels; i++) {
        unsigned char r = pixels[i * 3];
        unsigned char g = pixels[i * 3 + 1];
        unsigned char b = pixels[i * 3 + 2];
        
        if (r == 0 && g == 0 && b == 0) {
            zero_pixels++;
        }
        
        if (r < min_r) {
            min_r = r;
        }
        if (r > max_r) {
            max_r = r;
        }
        if (g < min_g) {
            min_g = g;
        }
        if (g > max_g) {
            max_g = g;
        }
        if (b < min_b) {
            min_b = b;
        }
        if (b > max_b) {
            max_b = b;
        }
        
        sum_r += r;
        sum_g += g;
        sum_b += b;
    }
    
    printf("Pixel Statistics:\n");
    printf("Total pixels: %d\n", total_pixels);
    printf("Black pixels (0,0,0): %d (%.1f%%)\n", 
           zero_pixels, (float)zero_pixels/total_pixels*100);
    printf("Red   - Min: %3d, Max: %3d, Avg: %.1f\n", 
           min_r, max_r, (float)sum_r/total_pixels);
    printf("Green - Min: %3d, Max: %3d, Avg: %.1f\n", 
           min_g, max_g, (float)sum_g/total_pixels);
    printf("Blue  - Min: %3d, Max: %3d, Avg: %.1f\n", 
           min_b, max_b, (float)sum_b/total_pixels);
    printf("\n");
}

bool is_somewhat_red(HSV hsv) {
    return ((hsv.h <= 20.0f || hsv.h >= 340.0f) && 
            hsv.s >= 15.0f && 
            hsv.v >= 20.0f);
}

bool is_somewhat_blue(HSV hsv) {
    if (hsv.v < 20.0f) {  // dark blue/purple
        return (hsv.h >= 200.0f && hsv.h <= 280.0f && hsv.s >= 30.0f);  
    } else {  // lighter blue
        return (hsv.h >= 200.0f && hsv.h <= 280.0f && hsv.s >= 15.0f);
    }
}

bool is_plant_like(HSV hsv) {
    return (
        ((hsv.h >= 25.0f && hsv.h <= 160.0f) && 
        hsv.s >= 15.0f && hsv.v >= 20.0f) ||    // green/yellow
        is_somewhat_red(hsv) ||
        is_somewhat_blue(hsv)
    );
}

void remove_background(unsigned char* pixels, int width, int height) {
    unsigned char* mask = malloc(width * height);
    if (!mask) return;

    memset(mask, 0, width * height); // 0 = background, 1 = plant

    // Classify plant-like pixels
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = y * width + x;
            unsigned char r = pixels[i * 3];
            unsigned char g = pixels[i * 3 + 1];
            unsigned char b = pixels[i * 3 + 2];
            HSV hsv = rgb_to_hsv(r, g, b);

            if (is_plant_like(hsv)) {
                mask[i] = 1;
            }
        }
    }

    // Recover nearby plant edges
    for (int pass = 0; pass < 1; pass++) {
        unsigned char* temp_mask = malloc(width * height);
        memcpy(temp_mask, mask, width * height);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int i = y * width + x;
                if (mask[i] != 0) continue;

                bool near_plant = false;
                if (x > 0 && mask[i - 1]) near_plant = true;
                if (x < width - 1 && mask[i + 1]) near_plant = true;
                if (y > 0 && mask[i - width]) near_plant = true;
                if (y < height - 1 && mask[i + width]) near_plant = true;

                if (near_plant) {
                    temp_mask[i] = 1;
                }
            }
        }

        memcpy(mask, temp_mask, width * height);
        free(temp_mask);
    }

    // Apply mask
    for (int i = 0; i < width * height; i++) {
        if (mask[i] == 0) {
            pixels[i * 3 + 0] =  128;
            pixels[i * 3 + 1] =  0;
            pixels[i * 3 + 2] =  128;
        }
    }

    free(mask);
}

unsigned char *load_ppm(char *filename, int *width_out, int *height_out, 
                        int *maxval_out) {
    
    // Header file structure below:
    // P6
    // <width> <hight>
    // <max colour val>
    // <binary data>
    

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file\n");
        exit(EXIT_FAILURE);
    }

    char header[3] = {0};
    if (fscanf(fp, "%2s", header) != 1) {
        fclose(fp);
        fprintf(stderr, "Failed to read PPM header number\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(header, "P6") != 0) {
        fclose(fp);
        fprintf(stderr, "Unsupported PPM format (only P6 supported)\n");
        exit(EXIT_FAILURE);
    }

    // Skip comments and whitespace before width and height
    int c = fgetc(fp);
    while (c == '\n' || c == '\r' || c == ' ' || c == '\t') {
        c = fgetc(fp);
    }
    ungetc(c, fp);  // Put c back into fp as read an extra char in while loop

    int width = 0; 
    int height = 0;
    int maxval = 0;

    // Skip comment lines starting with #
    while (true) {
        c = fgetc(fp);
        if (c == '#') {
            while ((c = fgetc(fp)) != '\n' && c != EOF) {
                // Do nothing, wait for comment line to end so gets skipped
            }; 
        } else {
            ungetc(c, fp);
            break;
        }
    }

    if (fscanf(fp, "%d %d", &width, &height) != 2) {
        fclose(fp);
        fprintf(stderr, "Invalid PPM header (width and height)\n");
        exit(EXIT_FAILURE);
    }

    // Skip comments again before maxval
    while (true) {
        c = fgetc(fp);
        if (c == '#') {
            while ((c = fgetc(fp)) != '\n' && c != EOF) {

            };
        } else {
            ungetc(c, fp);
            break;
        }
    }

    if (fscanf(fp, "%d", &maxval) != 1) {
        fclose(fp);
        fprintf(stderr, "Invalid PPM header (maxval)\n");
        exit(EXIT_FAILURE);
    }

    if (maxval != 255) {
        fclose(fp);
        fprintf(stderr, "Unsupported maxval %d (only 255 supported)\n", maxval);
        exit(EXIT_FAILURE);
    }

    // Consume single whitespace character after maxval before pixel data
    fgetc(fp);

    unsigned char *pixels = malloc(width * height * 3);
    if (!pixels) {
        fclose(fp);
        fprintf(stderr, "Out of memory allocating pixels\n");
        exit(EXIT_FAILURE);
    }

    size_t read = fread(pixels, 1, width * height * 3, fp);
    fclose(fp);

    if (read != (size_t)(width * height * 3)) {
        free(pixels);
        fprintf(stderr, "Incomplete pixel data in PPM file\n");
        exit(EXIT_FAILURE);
    }

    *width_out = width;
    *height_out = height;
    *maxval_out = maxval;
    return pixels;
}

int get_leaf_health_score(char *filename) {
    int width, height, maxval;
    unsigned char *pixels = load_ppm(filename, &width, &height, &maxval);
    if (pixels == NULL) {
        exit(EXIT_FAILURE);
    }

    remove_background(pixels, width, height);
    pixel_to_image(pixels, width, height, 0);

    float green_percent, yellow_percent, red_percent, black_percent, 
          texture_percentage;

    set_colours(
        pixels, width * height,
        &green_percent, &yellow_percent, &red_percent, &black_percent
    );
    
    analyse_texture(
        pixels, width, height,
        &texture_percentage
    );

    int leaf_score = score(
        green_percent, yellow_percent, red_percent, black_percent, 
        texture_percentage
    );

    free(pixels);
    return leaf_score;
}

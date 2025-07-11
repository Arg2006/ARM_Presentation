#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

#include "plant_health.h"


HSV rgb_to_hsv(unsigned char r, unsigned char g, unsigned char b) {
    float rf = r / 255.0f;
    float gf = g / 255.0f;
    float bf = b / 255.0f;
    
    float cmax = fmaxf(rf, fmaxf(gf, bf));
    float cmin = fminf(rf, fminf(gf, bf));
    float delta = cmax - cmin;
    
    HSV hsv;
    hsv.v = cmax * 100.0f;
    
    if (cmax < 0.00001f) {
        hsv.s = 0.0f;
    } else {
        hsv.s = (delta / cmax) * 100.0f;
    }
    
    if (delta < 0.00001f) {
        hsv.h = 0.0f;
    } else if (cmax == rf) {
        hsv.h = fmodf(60.0f * ((gf - bf) / delta) + 360.0f, 360.0f);
    } else if (cmax == gf) {
        hsv.h = 60.0f * ((bf - rf) / delta + 2.0f);
    } else {
        hsv.h = 60.0f * ((rf - gf) / delta + 4.0f);
    }
    
    return hsv;
}

bool is_green(unsigned char r, unsigned char g, unsigned char b) {
    HSV hsv = rgb_to_hsv(r, g, b);
    return (hsv.h >= 70.0f && hsv.h <= 170.0f) &&
           (hsv.s >= 20.0f && hsv.s <= 100.0f) &&
           (hsv.v >= 10.0f && hsv.v <= 60.0f);   // Cut off bright/light greens
}

bool is_yellow(unsigned char r, unsigned char g, unsigned char b) {
    HSV hsv = rgb_to_hsv(r, g, b);
    return (hsv.h >= 28.0f && hsv.h <= 64.0f) && // Now takes more lime
           (hsv.s >= 50.0f && hsv.v >= 65.0f);   // Ensure only vivid bright
}

bool is_red(unsigned char r, unsigned char g, unsigned char b) {
    HSV hsv = rgb_to_hsv(r, g, b);
    return ((hsv.h <= 15.0f || hsv.h >= 345.0f) &&
            hsv.s >= 50.0f &&
            hsv.v >= 20.0f && hsv.v <= 85.0f);
}

bool is_black(unsigned char r, unsigned char g, unsigned char b) {
    HSV hsv = rgb_to_hsv(r, g, b);
    return (hsv.v <= 25.0f) ||
           (hsv.v <= 35.0f && hsv.s <= 30.0f) ||
           (hsv.h >= 200.0f && hsv.h <= 300.0f &&
            hsv.v <= 45.0f && hsv.s >= 25.0f);
}



void set_colours(unsigned char* pixels, int total_pixels, 
                               float *green_percent_out, 
                               float *yellow_percent_out, 
                               float *red_percent_out, 
                               float *black_percent_out) {
    int green_pixels  = 0;
    int yellow_pixels = 0;
    int red_pixels    = 0;
    int black_pixels  = 0;
    
    for (int i = 0; i < total_pixels; i++) {
        unsigned char r = pixels[i*3];
        unsigned char g = pixels[i*3+1];
        unsigned char b = pixels[i*3+2];
        
        if (r == 128 && g == 0 && b == 128) continue; // Skip background
    
        if (is_green(r, g, b)) {
            green_pixels++;
        } else if (is_yellow(r, g, b)) {
            yellow_pixels++;
        } else if (is_red(r,g,b)) {
            red_pixels++;
        }else if (is_black(r,g,b)){
            black_pixels++;
        }
    }

    int total_plant_pixels = green_pixels + yellow_pixels + red_pixels + 
                             black_pixels;
    
    if (total_plant_pixels == 0) {
        printf("Warning: No plant pixels detected\n");
        exit(EXIT_FAILURE);
    }
        
    float green_percent  = (float)green_pixels  / total_plant_pixels * 100;
    float yellow_percent = (float)yellow_pixels / total_plant_pixels * 100;
    float red_percent    = (float)red_pixels    / total_plant_pixels * 100;
    float black_percent  = (float)black_pixels  / total_plant_pixels * 100;
    
    *green_percent_out = green_percent;
    *yellow_percent_out = yellow_percent;
    *red_percent_out    = red_percent;
    *black_percent_out  = black_percent;
}
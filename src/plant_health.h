#ifndef PLANT_HEALTH_H
#define PLANT_HEALTH_H

typedef struct {
    float h; // Hue (0-360)
    float s; // Saturation (0-100)
    float v; // Value (0-100)
} HSV;

HSV rgb_to_hsv(unsigned char r, unsigned char g, unsigned char b);

void set_colours(unsigned char* pixels, int total_pixels, 
    float *green_percent_out, float *yellow_percent_out, float *red_percent_out, 
    float *black_percent_out);

#endif // PLANT_HEALTH_H

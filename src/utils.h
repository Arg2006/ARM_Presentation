#ifndef UTILS_H
#define UTILS_H

unsigned char *load_ppm(char *filename, int *width_out, int *height_out, 
                  int *maxval_out);
void remove_background(unsigned char* pixels, int width, int height);
void print_pixels(unsigned char* pixels, int width, int height, int max_rows, 
                  int max_cols);
void pixel_to_image(unsigned char* pixels, int width, int height, 
                    int image_number);
int get_leaf_health_score(char *filename);

#endif  // UTILS_H

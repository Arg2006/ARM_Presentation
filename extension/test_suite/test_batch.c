#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "test_batch.h"

#include "../src/utils.h"
#include "../src/plant_health.h"
#include "../src/texture_analysis.h"
#include "../src/scoring_algorithm.h"


static bool within_tolerance(float a, float b, float tolerance) {
    float diff = a - b;
    if (diff < 0) {
        diff = -diff;
    }
    return diff <= tolerance;
}

int main(void) {
    printf("TESTING");
    printf("Green     Tolerance: +/- %.1f%%\n", GREEN_TOLERANCE);
    printf("Yellow    Tolerance: +/- %.1f%%\n", YELLOW_TOLERANCE);
    printf("Red       Tolerance: +/- %.1f%%\n", RED_TOLERANCE);
    printf("Black     Tolerance: +/- %.1f%%\n", BLACK_TOLERANCE);
    printf("Texture   Tolerance: +/- %.1f%%\n", TEXTURE_TOLERANCE);
    printf("Score     Tolerance: +/- %d\n", SCORE_TOLERANCE);

    for (int i = 0; i < NUM_IMAGES; ++i) {
        int width = 0; 
        int height = 0;
        int maxval = 0;
        printf("----------------------------------------\n");
        printf("Testing image %s ...\n", test_images[i].filename);
        printf("----------------------------------------\n");

        unsigned char *pixels = load_ppm(test_images[i].filename, 
                                         &width, &height, &maxval);
        if (pixels == NULL) {
            fprintf(stderr, "Failed to load image: %s\n", 
                    test_images[i].filename);
            continue;
        }

        int total_pixels = width * height;
        float *green_percentage = malloc(sizeof(float));
        if (green_percentage == NULL) {
            fprintf(stderr, 
                    "Failed to allocate memory space for green percentage\n");
            exit(EXIT_FAILURE);
        }
        float *yellow_percentage = malloc(sizeof(float));
        if (yellow_percentage == NULL) {
            fprintf(stderr, 
                    "Failed to allocate memory space for yellow percentage\n");
            exit(EXIT_FAILURE);
        }
        float *red_percentage = malloc(sizeof(float));
        if (red_percentage == NULL) {
            fprintf(stderr, 
                    "Failed to allocate memory space for red percentage\n");
            exit(EXIT_FAILURE);
        }
        float *black_percentage = malloc(sizeof(float));
        if (black_percentage == NULL) {
            fprintf(stderr, 
                    "Failed to allocate memory space for blaack percentage\n");
            exit(EXIT_FAILURE);
        }

        float *texture_percentage = malloc(sizeof(float));
        if (texture_percentage == NULL) {
            fprintf(stderr, 
                    "Failed to allocate memory space for texture percentage\n");
            exit(EXIT_FAILURE);
        }
    
        remove_background(pixels, width, height);
        pixel_to_image(pixels, width, height, i+1);
        set_colours(pixels, total_pixels, green_percentage, 
                                  yellow_percentage, red_percentage, 
                                  black_percentage);
        analyse_texture(pixels, width, height, texture_percentage);
        int overall_score = score(*green_percentage, *yellow_percentage, 
                                  *red_percentage, *black_percentage, 
                                  *texture_percentage);
        printf("Computed Green    Percentage: %4.1f%% (Expected %4.1f%%)\n", 
               *green_percentage, test_images[i].expected_green_percentage);
        printf("Computed Yellow   Percentage: %4.1f%% (Expected %4.1f%%)\n", 
               *yellow_percentage, test_images[i].expected_yellow_percentage);
        printf("Computed Red      Percentage: %4.1f%% (Expected %4.1f%%)\n", 
               *red_percentage, test_images[i].expected_red_percentage);
        printf("Computed Black    Percentage: %4.1f%% (Expected %4.1f%%)\n", 
               *black_percentage, test_images[i].expected_black_percentage);
        printf("Computed Texture  Percentage: %4.1f%% (Expected %4.1f%%)\n", 
               *texture_percentage, test_images[i].expected_texture_percentage);
        printf("Computed Score    Score/10  : %2d/10 (Expected %2d/10)\n", 
               overall_score, test_images[i].expected_score_value);


        bool green_pass = within_tolerance(*green_percentage, 
            test_images[i].expected_green_percentage, GREEN_TOLERANCE);
        bool yellow_pass = within_tolerance(*yellow_percentage, 
            test_images[i].expected_yellow_percentage, YELLOW_TOLERANCE);
        bool red_pass = within_tolerance(*red_percentage, 
            test_images[i].expected_red_percentage, RED_TOLERANCE);
        bool black_pass = within_tolerance(*black_percentage, 
            test_images[i].expected_black_percentage, BLACK_TOLERANCE);
        bool texture_pass = within_tolerance(*texture_percentage, 
            test_images[i].expected_texture_percentage, TEXTURE_TOLERANCE);
        bool score_pass = within_tolerance(overall_score, 
            test_images[i].expected_score_value, SCORE_TOLERANCE);

        if (green_pass && yellow_pass && red_pass && black_pass && 
            texture_pass && score_pass) {
            printf("Result: PASS\n");
        } else {
            printf("Result: FAIL\n");
            if (green_pass == false)
                printf("    Green percentage\n");
            if (yellow_pass == false)
                printf("    Yellow percentage\n");
            if (red_pass == false)
                printf("    Red percentage\n");
            if (black_pass == false)
                printf("    Black percentage\n");
            if (texture_pass == false)
                printf("    Texture percentage\n");
            if (score_pass == false)
                printf("    Score value\n");
        }
        printf("\n");

        free(green_percentage);
        free(yellow_percentage);
        free(red_percentage);
        free(black_percentage);
        free(texture_percentage);

        free(pixels);
    }

    return 0;
}

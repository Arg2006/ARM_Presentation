#ifndef TEST_BATCH_H
#define TEST_BATCH_H

typedef struct {
    char *filename;
    float expected_green_percentage;
    float expected_yellow_percentage;
    float expected_red_percentage;
    float expected_black_percentage;
    float expected_texture_percentage;
    int expected_score_value;
} TestImage;

#define NUM_IMAGES 14
#define GREEN_TOLERANCE 10.0f     // +/- GREEN TOLERANCE %
#define YELLOW_TOLERANCE 10.0f    // +/- YELLOW TOLERANCE %
#define RED_TOLERANCE 10.0f       // +/- RED TOLERANCE %
#define BLACK_TOLERANCE 10.0f     // +/- BLACK TOLERANCE %
#define TEXTURE_TOLERANCE 10.0f   // +/- TEXTURE TOLERANCE %
#define SCORE_TOLERANCE 1         // +/- SCORE TOLERANCE

TestImage test_images[NUM_IMAGES] = {
    // Image, Green, Yellow, Red, Black, Texture, Score
    {"images/ppm/leaf01.ppm", 100.0f, 0.0f, 0.0f, 0.0f, 80.0f, 10},
    {"images/ppm/leaf02.ppm", 0.0f, 0.1f, 88.0f, 10.0f, 70.0f, 8},
    {"images/ppm/leaf03.ppm", 90.0f, 0.0f, 0.0f, 0.0f, 95.0f, 9},
    {"images/ppm/leaf04.ppm", 17.0f, 80.0f, 0.0f, 2.0f, 45.0f, 1},
    {"images/ppm/leaf05.ppm", 100.0f, 0.0f, 0.0f, 0.0f, 50.0f, 9},  
    {"images/ppm/leaf06.ppm", 5.0f, 70.0f, 0.0f, 20.0f, 40.0f, 0},
    {"images/ppm/leaf07.ppm", 0.0f, 95.0f, 0.0f, 0.0f, 40.0f, 0},
    {"images/ppm/leaf08.ppm", 17.0f, 15.0f, 2.0f, 60.0f, 50.0f, 2},
    {"images/ppm/leaf09.ppm", 65.0f, 0.0f, 0.0f, 35.0f, 80.0f, 4},
    {"images/ppm/leaf10.ppm", 0.0f, 35.0f, 60.0f, 10.0f, 60.0f, 5},
    {"images/ppm/leaf11.ppm", 50.0f, 30.0f, 0.0f, 20.0f, 80.0f, 5},
    {"images/ppm/leaf12.ppm", 40.0f, 50.0f, 0.0f, 10.0f, 50.0f, 5},
    {"images/ppm/leaf13.ppm", 10.0f, 80.0f, 0.0f, 15.0f, 70.0f, 1},
    {"images/ppm/leaf14.ppm", 5.0f, 45.0f, 0.0f, 55.0f, 40.0f, 0},
};

#endif // TEST_BATCH_H

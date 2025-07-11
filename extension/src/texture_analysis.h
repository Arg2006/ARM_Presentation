#ifndef TEXTURE_ANALYSIS_H
#define TEXTURE_ANALYSIS_H

#define DISEASE_VARIANCE_THRESHOLD 500.0f  // Clearly diseased spots

void analyse_texture(unsigned char* pixels, int width, int height, 
                     float *texture_percentage);

#endif // TEXTURE_ANALYSIS_H

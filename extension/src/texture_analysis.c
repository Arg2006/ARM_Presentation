#include <stdint.h>
#include <math.h>

#include "texture_analysis.h"


void analyse_texture(unsigned char* pixels, int width, int height, 
                     float *texture_percentage) {
    int healthy = 0, total = 0;
    const int radius = 3; // 7x7 window
    
    for (int y = radius; y < height - radius; y++) {
        for (int x = radius; x < width - radius; x++) {
            int idx = (y * width + x) * 3;
            
            // Skip background
            if (pixels[idx] == 0 && pixels[idx+1] == 0 && pixels[idx+2] == 0) {
                continue;
            }
            
            // Calculate local variance in green channel
            float mean = 0, variance = 0;
            int samples = 0;
            
            for (int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    int sample_idx = ((y+dy)*width + (x+dx)) * 3 + 1; // Green
                    float val = pixels[sample_idx];
                    mean += val;
                    samples++;
                }
            }
            mean /= samples;
            
            for (int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    int sample_idx = ((y+dy)*width + (x+dx)) * 3 + 1;
                    float val = pixels[sample_idx];
                    variance += powf(val - mean, 2);
                }
            }
            variance /= samples;
            
            // Score this pixel (higher variance = worse)
            float pixel_score = 100.0f * (1.0f - fminf(variance/
                                          DISEASE_VARIANCE_THRESHOLD, 1.0f));
            healthy += pixel_score;
            total++;
        }
    }
    *texture_percentage = (total > 0) ? (healthy / total) : 0.0f;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "scoring_algorithm.h"


int score(int green_percent, int yellow_percent, int red_percent, 
          int black_percent, int texture) {
    
    // Determine leaf type: green vs red
    bool is_green_leaf = (green_percent > red_percent);
    
    int base_score = 1;
    
    if (is_green_leaf) {
        // Green leaf type
        base_score += (green_percent * 8) / 100;  // 0-8 points from green
        
        // Detract ALL other colors from green leaves
        base_score -= (red_percent * 2) / 100; 
        base_score -= (yellow_percent * 3) / 100;   
        base_score -= (black_percent * 6) / 100; 
        
    } else {
        // Red leaf type  
        base_score += (red_percent * 8) / 100;   // 0-8 points from red
        
        // Only detract yellow and black from red leaves
        base_score -= (yellow_percent * 3) / 100;
        base_score -= (black_percent * 6) / 100; 
        // Green doesn't detract from red leaves
    }
    
    // Texture bonus (reduced weight: 0-2 points)
    int texture_bonus = (texture * 2)/100 + 1;
    
    // Combine scores
    int final_score = base_score + texture_bonus;
    
    // Clamp to 0-10 range
    if (final_score > 10) final_score = 10;
    if (final_score < 0) final_score = 0;
    
    return final_score;
}
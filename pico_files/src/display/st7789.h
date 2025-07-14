#ifndef ST7789_H
#define ST7789_H

#include <stdint.h>

// Function prototypes
void st7789_init(void);
void st7789_set_rotation(uint8_t r);
void st7789_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void st7789_fill_screen(uint16_t color);
void st7789_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void st7789_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size);
void st7789_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size);
void draw_potted_plant_right_half();

#endif
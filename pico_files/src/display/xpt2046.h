#ifndef XPT2046_H
#define XPT2046_H

#include <stdint.h>
#include <stdbool.h>

// Function prototypes
void xpt2046_init(void);
bool xpt2046_is_touched(void);
void xpt2046_get_coordinates(uint16_t *x, uint16_t *y);

#endif
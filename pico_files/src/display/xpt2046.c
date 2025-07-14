#include "xpt2046.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "../config.h"

static uint16_t xpt2046_read_channel(uint8_t channel) {
    uint8_t cmd = 0x80 | (channel << 4);
    uint8_t buffer[3] = {cmd, 0x00, 0x00};
    uint8_t result[3];
    
    gpio_put(TP_CS, 0);
    spi_write_read_blocking(SPI_PORT, buffer, result, 3);
    gpio_put(TP_CS, 1);
    
    return ((result[1] << 8) | result[2]) >> 3;
}

void xpt2046_init(void) {
    // Touch CS pin
    gpio_init(TP_CS);
    gpio_set_dir(TP_CS, GPIO_OUT);
    gpio_put(TP_CS, 1);
    
    // Touch IRQ pin (optional)
    gpio_init(TP_IRQ);
    gpio_set_dir(TP_IRQ, GPIO_IN);
    gpio_pull_up(TP_IRQ);
}

bool xpt2046_is_touched(void) {
    // IRQ is low when touched
    return !gpio_get(TP_IRQ);
}

void xpt2046_get_coordinates(uint16_t *x, uint16_t *y) {
    if (!xpt2046_is_touched()) {
        *x = 0;
        *y = 0;
        return;
    }
    
    // Read multiple samples and average
    uint32_t x_sum = 0, y_sum = 0;
    int valid_samples = 0;
    
    for (int i = 0; i < 8; i++) {
        if (xpt2046_is_touched()) {
            uint16_t x_raw = xpt2046_read_channel(1); // X channel
            uint16_t y_raw = xpt2046_read_channel(5); // Y channel
            
            x_sum += x_raw;
            y_sum += y_raw;
            valid_samples++;
        }
        sleep_us(100);
    }
    
    if (valid_samples > 0) {
        uint16_t x_avg = x_sum / valid_samples;
        uint16_t y_avg = y_sum / valid_samples;
        
        // Convert to screen coordinates (adjust these values for your specific display)
        *x = (x_avg - 300) * DISPLAY_WIDTH / (3700 - 300);
        *y = (y_avg - 300) * DISPLAY_HEIGHT / (3700 - 300);
        
        // Clamp to screen bounds
        if (*x > DISPLAY_WIDTH) *x = DISPLAY_WIDTH;
        if (*y > DISPLAY_HEIGHT) *y = DISPLAY_HEIGHT;
    } else {
        *x = 0;
        *y = 0;
    }
}
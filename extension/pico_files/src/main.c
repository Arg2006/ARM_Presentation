#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "display/st7789.h"
#include "display/xpt2046.h"
#include "display/fonts.h"
#include "config.h"
#include "network/wifi_handler.h"
#include "network/udp_handler.h"


// Define constants for display
char humidity[16] = "-";
char temperature[16] = "-";
char moisture[16] = "-";
char light[16] = "-";
char health[16] = "-";

// Display buffer for text
static char display_buffer[10][40]; // 10 lines, 40 chars each
static int current_line = 0;
static bool display_updated = false;

void handle_udp_message(const unsigned char *data, unsigned int len,
                        const struct ip4_addr *addr, uint16_t port) {
    if (len > 39) len = 39;

    char message[40] = {0};
    strncpy(message, (const char *)data, len);
    message[len] = '\0';

    char *sep = strchr(message, ':');
    if (sep) {
        *sep = '\0';
        const char *key = message;
        const char *value = sep + 1;

        if (strcasecmp(key, "MOISTURE") == 0) {
            strncpy(moisture, value, sizeof(moisture) - 1);
        } else if (strcasecmp(key, "TEMP") == 0) {
            strncpy(temperature, value, sizeof(temperature) - 1);
        } else if (strcasecmp(key, "HUMIDITY") == 0) {
            strncpy(humidity, value, sizeof(humidity) - 1);
        } else if (strcasecmp(key, "HEALTH") == 0) {
            strncpy(health, value, sizeof(health) - 1);
        } else if (strcasecmp(key, "LIGHT") == 0) {
            strncpy(light, value, sizeof(light) - 1);
        }
    }

    display_updated = true;
}

void update_display() {
    if (!display_updated) return;

    st7789_fill_screen(BLACK);
    draw_potted_plant_right_half();

    // Header
    st7789_draw_string(60, 5, "Plant Monitor", WHITE, BLACK, 2);
    st7789_draw_line(10, 30, 310, 30, WHITE);  // header underline

    // Display area settings
    int startY = 40;
    int spacing = 35;

    // Labels + Data

    st7789_draw_string(20, startY + spacing * 0, 
                       "Light Level:", GREEN, BLACK, 1);
    st7789_draw_string(140, startY + spacing * 0, light, WHITE, BLACK, 1);

    st7789_draw_string(20, startY + spacing * 1, 
                       "Moisture:", MAGENTA, BLACK, 1);
    st7789_draw_string(140, startY + spacing * 1, moisture, WHITE, BLACK, 1);

    st7789_draw_string(20, startY + spacing * 2, 
                       "Temperature:", YELLOW, BLACK, 1);
    st7789_draw_string(140, startY + spacing * 2, temperature, WHITE, BLACK, 1);

    st7789_draw_string(20, startY + spacing * 3, "Humidity:", CYAN, BLACK, 1);
    st7789_draw_string(140, startY + spacing * 3, humidity, WHITE, BLACK, 1);

    st7789_draw_string(20, startY + spacing * 4, 
                       "Plant Health:", RED, BLACK, 1);
    st7789_draw_string(140, startY + spacing * 4, health, WHITE, BLACK, 1);


    // Transmission info
    char status[60];
    snprintf(status, sizeof(status), "IP: %s | Port: %d", wifi_get_ip_address(), 
             UDP_PORT);
    st7789_draw_string(10, 220, status, GREEN, BLACK, 1);

    display_updated = false;
}

void draw_initial_screen() {

    // Blank screen
    st7789_fill_screen(BLACK);

    // Header
    st7789_draw_string(60, 10, "Plant Monitor", WHITE, BLACK, 2);
    st7789_draw_line(10, 35, 310, 35, WHITE);

    // Placeholder readouts
    st7789_draw_string(20, 50, "Light Level:", GREEN, BLACK, 1);
    st7789_draw_string(140, 50, "--", WHITE, BLACK, 1);

    st7789_draw_string(20, 85, "Moisture:", MAGENTA, BLACK, 1);
    st7789_draw_string(140, 85, "--", WHITE, BLACK, 1);

    st7789_draw_string(20, 120, "Temperature:", YELLOW, BLACK, 1);
    st7789_draw_string(140, 120, "--", WHITE, BLACK, 1);

    st7789_draw_string(20, 155, "Humidity:", CYAN, BLACK, 1);
    st7789_draw_string(140, 155, "--", WHITE, BLACK, 1);

    st7789_draw_string(20, 190, "Plant Health:", RED, BLACK, 1);
    st7789_draw_string(140, 190, "--", WHITE, BLACK, 1);

    // Footer: IP address and port
    char ip_str[60];
    snprintf(ip_str, sizeof(ip_str), "IP: %s | Port: %d", wifi_get_ip_address(), 
             UDP_PORT);
    st7789_draw_string(10, 220, ip_str, GREEN, BLACK, 1);
}

// SDK version
void print_sdk_version() {
    printf("Pico SDK Version: %s\n", PICO_SDK_VERSION_STRING);
    // 1.5.0-1.5.1 had auth parsing bugs
}

int main() {
    stdio_init_all();
    sleep_ms(5000);
    printf("Pico Display Demo Starting...\n");
    print_sdk_version();
    
    // Initialize WiFi
    if (wifi_init() != 0) return -1;
    if (wifi_connect(NULL, NULL) != 0) {
        wifi_deinit();
        return -1;
    }

    printf("IP Address: %s\n", wifi_get_ip_address());
    printf("%s\n", wifi_get_status_string());

    // Initialize display
    st7789_init();
    st7789_set_rotation(1); // Landscape
    xpt2046_init();
    memset(display_buffer, 0, sizeof(display_buffer));
    draw_initial_screen();

    // Initialize UDP server
    if (udp_server_init(handle_udp_message) != 0) {
        printf("Failed to start UDP server\n");
        wifi_deinit();
        return -1;
    }


    printf("System ready - waiting for UDP messages...\n");

    // Main loop
    while (1) {
        update_display();
        // Touchscreen: working program but we broke the screen :(
        // if (xpt2046_is_touched()) {
        //     uint16_t x, y;
        //     xpt2046_get_coordinates(&x, &y);
        //     printf("Touch at: %d, %d\n", x, y);
        // }
        sleep_ms(10);
    }

    // Cleanup - halts server in event of WiFi loss or loop break
    udp_deinit();
    wifi_deinit();
    return 0;
}

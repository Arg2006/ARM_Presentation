#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration - override using compile flags
#ifndef WIFI_SSID
#define WIFI_SSID "DefaultNetworkName"
#endif

#ifndef WIFI_PASSWORD  
#define WIFI_PASSWORD "DefaultPassword"
#endif

// Network Configuration
#define UDP_PORT 8888

// Actual values from board:
// SDIO_CLK = GP5
// LCD_DC = GP8
// LCD_CS = GP9
// CLK = GP10
// MOSI = GP11 
// MISO = GP12
// LCD_BL = GP13
// LCD_RST = GP15
// SD_CS/D3 = GP22
// D2 = GP21
// D1 = GP20
// DO = GP19
// SDIO_CMD = GP18
// TP_IRQ = GP17
// TP_CS = GP16

// Display Configuration
// (Waveshare 2.8" - ST7789 NOT ILI9341)
#define PIN_MOSI 11   // MOSI (SPI data to display)
#define PIN_SCK  10   // CLK (SPI clock)
#define PIN_CS   9    // LCD_CS
#define PIN_DC   8    // LCD_DC
#define PIN_RST  15   // LCD_RST

// Backlight control
#define PIN_BL   13   // LCD_BL (Backlight, if controllable)

// Touch Configuration (XPT2046)
#define TP_CS    16    // Touch Chip Select
#define TP_IRQ   17    // Touch Interrupt

// SPI Configuration
#define SPI_PORT spi1
#define SPI_BAUDRATE 10000000  // 10MHz

// Display Colors (RGB565)
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Display Dimensions
#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240

#endif
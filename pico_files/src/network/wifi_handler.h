#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <stdbool.h>

// WiFi initialization and management functions
int wifi_init(void);
int wifi_connect(const char* ssid, const char* password);
void wifi_disconnect(void);
void wifi_deinit(void);

// WiFi status functions
bool wifi_is_connected(void);
const char* wifi_get_status_string(void);
const char* wifi_get_ip_address(void);

// WiFi maintenance functions
int wifi_check_and_reconnect(void);
void wifi_update_led(void);
void wifi_poll(void);

#endif // WIFI_HANDLER_H

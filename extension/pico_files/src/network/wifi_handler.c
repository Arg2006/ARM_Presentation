#include "wifi_handler.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/netif.h"
#include <string.h>
#include <stdio.h>
#include "lwip/dhcp.h"
#include "lwip/ip4_addr.h"

// Connection timeout in ms
#define WIFI_CONNECT_TIMEOUT_MS 30000

static bool wifi_connected = false;
static absolute_time_t connection_start_time;

// Initialize WiFi hardware
int wifi_init(void) {
    // Initialize the CYW43 driver
    if (cyw43_arch_init()) {
        printf("Failed to initialize CYW43 driver\n");
        return -1;
    }

    // Enable station mode
    cyw43_arch_enable_sta_mode();
    
    printf("WiFi initialized successfully\n");
    return 0;
}

#define WIFI_CONNECT_TIMEOUT_MS 30000

int wifi_connect(const char *ssid, const char *password) {
    if (!ssid || !password) {
        ssid = WIFI_SSID;   
        password = WIFI_PASSWORD;
    }

    printf("Connecting to WiFi network: %s\n", ssid);

    cyw43_wifi_pm(&cyw43_state, CYW43_PERFORMANCE_PM);

    int ssid_len = strlen(ssid);

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD,
         CYW43_AUTH_WPA2_AES_PSK, WIFI_CONNECT_TIMEOUT_MS)) {
        printf("failed to connect.\n");
        return 1;
    } else {
        printf("Connected.\n");
        wifi_connected = true;
        wifi_update_led();
    }

    printf("WiFi link is up, starting client...\n");


    printf("DHCP assigned IP: %s\n", ip4addr_ntoa(
                                        netif_ip4_addr(netif_default)));
    printf("Netmask: %s\n", ip4addr_ntoa(netif_ip4_netmask(netif_default)));
    printf("Gateway: %s\n", ip4addr_ntoa(netif_ip4_gw(netif_default)));

    return 0;
}

// Disconnect from WiFi
void wifi_disconnect(void) {
    if (wifi_connected) {
        cyw43_arch_wifi_connect_async("", "", CYW43_AUTH_OPEN);
        wifi_connected = false;
        wifi_update_led();
        printf("WiFi disconnected\n");
    }
}

// Check WiFi connection status
bool wifi_is_connected(void) {
    // Check hardware link status
    int link_status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
    
    if (link_status == CYW43_LINK_UP) {
        wifi_connected = true;
        wifi_update_led();
        return true;
    } else {
        wifi_connected = false;
        wifi_update_led();
        return false;
    }
}

// Get WiFi connection status string
const char* wifi_get_status_string(void) {
    int link_status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
    
    switch (link_status) {
        case CYW43_LINK_DOWN:
            return "Link Down";
        case CYW43_LINK_JOIN:
            return "Joining";
        case CYW43_LINK_NOIP:
            return "Connected (No IP)";
        case CYW43_LINK_UP:
            return "Connected";
        case CYW43_LINK_FAIL:
            return "Connection Failed";
        case CYW43_LINK_NONET:
            return "No Network";
        case CYW43_LINK_BADAUTH:
            return "Bad Authentication";
        default:
            return "Unknown";
    }
}

// Get IP address as string
const char* wifi_get_ip_address(void) {
    if (wifi_connected) {
        return ip4addr_ntoa(netif_ip4_addr(netif_default));
    }
    printf("Not connected.");
    return "0.0.0.0";
}

// Attempt to reconnect if connection is lost
int wifi_check_and_reconnect(void) {
    if (!wifi_is_connected()) {
        printf("WiFi connection lost, attempting to reconnect...\n");
        return wifi_connect(NULL, NULL);
    }
    wifi_update_led();
    return 0;
}

// Clean up WiFi resources
void wifi_deinit(void) {
    wifi_disconnect();
    wifi_update_led();
    cyw43_arch_deinit();
    printf("WiFi deinitialized\n");
}

// Set WiFi LED based on connection status
void wifi_update_led(void) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, wifi_connected ? 1 : 0);
}

// Periodic WiFi maintenance (call from main loop)
void wifi_poll(void) {
    // Update LED status
    wifi_update_led();
    
    // Check connection periodically
    static absolute_time_t last_check = 0;
    absolute_time_t now = get_absolute_time();
    
    if (absolute_time_diff_us(last_check, now) > 5000000) {
        // Check every 5 seconds
        wifi_check_and_reconnect();
        last_check = now;
    }
    
    // Poll CYW43 driver
    cyw43_arch_poll();
}

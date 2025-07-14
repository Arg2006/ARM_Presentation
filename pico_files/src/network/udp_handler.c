#include "udp_handler.h"
#include "wifi_handler.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include <string.h>
#include <stdio.h>

// Default UDP port
#ifndef UDP_PORT    
#define UDP_PORT 8888
#endif

// Maximum message size
#define MAX_UDP_MSG_SIZE 1024

// UDP control block
static struct udp_pcb *udp_pcb = NULL;
static uint16_t listen_port = UDP_PORT;
static udp_data_callback_t data_callback = NULL;
static bool udp_server_active = false;

// Buffer for incoming data
static uint8_t rx_buffer[MAX_UDP_MSG_SIZE];

// Callback function for receiving UDP data
static void udp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p, 
                              const ip_addr_t *addr, u16_t port) {
    if (p != NULL) {
        // Copy data to buffer
        size_t data_len = p->tot_len;
        if (data_len > MAX_UDP_MSG_SIZE) {
            data_len = MAX_UDP_MSG_SIZE;
        }
        
        // Copy pbuf data to our buffer
        pbuf_copy_partial(p, rx_buffer, data_len, 0);
        
        printf("UDP received %d bytes from %s:%d\n", data_len, 
               ip4addr_ntoa(addr), port);
        
        // Call user callback if registered
        if (data_callback) {
            data_callback(rx_buffer, data_len, addr, port);
        }
        
        // Free the pbuf
        pbuf_free(p);
    }
}

// Initialize UDP handler
int udp_server_init(udp_data_callback_t callback) {
    printf("Initializing UDP handler...\n");
    
    if (!callback) {
        printf("Error: NULL callback passed to udp_server_init\n");
        return -1;
    }

    data_callback = callback; // Save the app-level callback
    return udp_server_start(UDP_PORT, callback); // Start server via saved port
}


// Start UDP server on specified port
int udp_server_start(uint16_t port, udp_data_callback_t callback) {
    if (udp_server_active) {
        printf("UDP server already active\n");
        return -1;
    }

    // Create PCB
    udp_pcb = udp_new();
    if (!udp_pcb) {
        printf("Failed to create UDP PCB\n");
        return -1;
    }

    if (udp_bind(udp_pcb, IP_ADDR_ANY, port) != ERR_OK) {
        printf("Failed to bind UDP port %d\n", port);
        udp_remove(udp_pcb);
        udp_pcb = NULL;
        return -1;
    }

    // Register internal receive callback
    udp_recv(udp_pcb, udp_recv_callback, NULL);

    listen_port = port;
    udp_server_active = true;

    printf("UDP server started on port %d\n", port);
    return 0;
}


// Stop UDP server
void udp_server_stop(void) {
    if (udp_pcb) {
        udp_remove(udp_pcb);
        udp_pcb = NULL;
    }
    
    udp_server_active = false;
    data_callback = NULL;
    printf("UDP server stopped\n");
}

// Send UDP data to specified address and port
int udp_send_data(const ip_addr_t *dest_addr, uint16_t dest_port, 
                  const uint8_t *data, size_t data_len) {
    
    if (data_len > MAX_UDP_MSG_SIZE) {
        printf("Data too large for UDP packet\n");
        return -1;
    }
    
    // Create a temporary PCB for sending if no server running
    struct udp_pcb *send_pcb = udp_pcb;
    bool temp_pcb = false;
    
    if (!send_pcb) {
        send_pcb = udp_new();
        if (!send_pcb) {
            printf("Failed to create UDP PCB for sending\n");
            return -1;
        }
        temp_pcb = true;
    }
    
    // Allocate pbuf for data
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, data_len, PBUF_RAM);
    if (!p) {
        printf("Failed to allocate pbuf\n");
        if (temp_pcb) {
            udp_remove(send_pcb);
        }
        return -1;
    }
    
    // Copy data to pbuf
    memcpy(p->payload, data, data_len);
    
    // Send the data
    err_t err = udp_sendto(send_pcb, p, dest_addr, dest_port);
    
    // Free pbuf
    pbuf_free(p);
    
    // Clean up temporary PCB
    if (temp_pcb) {
        udp_remove(send_pcb);
    }
    
    if (err != ERR_OK) {
        printf("Failed to send UDP data, error: %d\n", err);
        return -1;
    }
    
    printf("UDP sent %d bytes to %s:%d\n", data_len, ip4addr_ntoa(dest_addr), 
           dest_port);
    return 0;
}

// Send UDP data to IP address string
int udp_send_to_ip_string(const char *ip_str, uint16_t dest_port, 
                          const uint8_t *data, size_t data_len) {
    ip_addr_t dest_addr;
    
    if (!ip4addr_aton(ip_str, &dest_addr)) {
        printf("Invalid IP address: %s\n", ip_str);
        return -1;
    }
    
    return udp_send_data(&dest_addr, dest_port, data, data_len);
}

// Get current listen port
uint16_t udp_get_listen_port(void) {
    return listen_port;
}

// Check if UDP server is active
bool udp_is_server_active(void) {
    return udp_server_active;
}

// Process UDP (call from main loop)
void udp_poll(void) {
    // lwIP polling is handled by cyw43_arch_poll() in wifi_handler
    // This function can be used for any UDP-specific maintenance
    
    // Check if we need to restart server after WiFi reconnection
    if (!udp_server_active && wifi_is_connected() && data_callback) {
        printf("WiFi reconnected, restarting UDP server...\n");
        udp_server_start(listen_port, data_callback);
    }
}

// Cleanup UDP resources
void udp_deinit(void) {
    udp_server_stop();
    printf("UDP handler deinitialized\n");
}
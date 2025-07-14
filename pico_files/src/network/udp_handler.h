#ifndef UDP_HANDLER_H
#define UDP_HANDLER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "lwip/ip_addr.h"

// Callback function type for UDP data reception
typedef void (*udp_data_callback_t)(const uint8_t *data, size_t data_len, const ip_addr_t *src_addr, uint16_t src_port);

// UDP initialization and cleanup
int udp_server_init(udp_data_callback_t callback);
void udp_deinit(void);

// UDP server functions
int udp_server_start(uint16_t port, udp_data_callback_t callback);
void udp_server_stop(void);
bool udp_is_server_active(void);
uint16_t udp_get_listen_port(void);

// UDP client functions (for sending data)
int udp_send_data(const ip_addr_t *dest_addr, uint16_t dest_port, 
                  const uint8_t *data, size_t data_len);
int udp_send_to_ip_string(const char *ip_str, uint16_t dest_port, 
                          const uint8_t *data, size_t data_len);

// UDP maintenance
void udp_poll(void);

#endif // UDP_HANDLER_H

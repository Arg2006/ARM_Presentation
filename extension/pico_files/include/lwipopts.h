#ifndef LWIPOPTS_H
#define LWIPOPTS_H

#define NO_SYS                          1
#define LWIP_SOCKET                     0
#define LWIP_NETCONN                    0
#define LWIP_DNS                        0
#define LWIP_ICMP                       1
#define LWIP_IGMP                       0
#define LWIP_IPV6                       0
#define LWIP_HAVE_LOOPIF                1

#define LWIP_DHCP                       1

#define MEM_ALIGNMENT                   4
#define MEM_SIZE                        4096

#define MEMP_NUM_PBUF                   8
#define MEMP_NUM_SYS_TIMEOUT            5
#define MEMP_NUM_TCP_SEG                8

#define PBUF_POOL_SIZE                  8
#define PBUF_POOL_BUFSIZE               600

#define LWIP_TCP                        0

#define LWIP_DBG_TYPES_ON               LWIP_DBG_ON

#endif /* LWIPOPTS_H */

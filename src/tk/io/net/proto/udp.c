/**
*******************************************************************************
* @file udp.c
* @author Keidan
* @date 11/10/2013
* @par Project
* tk
*
* @par Copyright
* Copyright 2011-2014 Keidan, all right reserved
*
* This software is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY.
*
* Licence summary : 
*    You can modify and redistribute the sources code and binaries.
*    You can send me the bug-fix
*
* Term of the licence in in the file licence.txt.
*
*******************************************************************************
*/
#include <tk/io/net/proto/udp.h>
#include <tk/io/net/nettools.h>
#include <tk/sys/log.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ether.h>
#include <linux/if_packet.h>

#define UDP_MAGIC 0xC007ED
#define UDP_H_ETH_SET 0x02
#define UDP_H_IP_SET 0x04
#define UDP_H_UDP_SET 0x08

#define create_ptr(local, param) struct udp_s *local = (struct udp_s*)param
#define test_ptr(ptr) (ptr && ptr->magic == UDP_MAGIC)

struct udp_s {
    int magic;
    int fd;
    int flags;
    struct ethhdr eth;
    struct iphdr ip;
    struct udphdr udp;
    struct net_payload_s payload;
};

/**
 * @fn udp_t udp_new()
 * @brief Open an UDP connection.
 * @return The UDP context else NULL on error.
 */
udp_t udp_new() {
  struct udp_s *udp = NULL;
  if((upd = mallc(sizeof(struct udp_s))) == NULL) {
    logger(LOG_ERR, "Unable to alloc a memory for the UDP object.\n");
    return NULL;
  }
  memset(upd, 0, sizeof(struct udp_s));
  udp->magic = UDP_MAGIC;
  udp->fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if(udp->fd == -1) {
    logger(LOG_ERR, "Unable to open the RAW UDP socket: (%d) %s\n", errno, strerror(errno)); 
    udp_delete(udp);
    return NULL;
  }
  return udp;
}

/**
 * @fn void udp_delete(udp_t udp)
 * @brief release the udp context.
 * @param udp The context.
 */
void udp_delete(udp_t udp) {
  create_ptr(u, udp);
  if(test_ptr(udp)) {
    if(u->fd > 2) close(u->fd), u->fd = -1;
    if(u->payload.buffer) free(u->payload.buffer), u->payload.buffer = NULL;
    u->magic = 0;
    free(u);
  }
}


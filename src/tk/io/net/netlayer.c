/**
*******************************************************************************
* @file netlayer.c
* @author Keidan
* @date 18/07/2014
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
#include <tk/io/net/netlayer.h>
#include <tk/io/net/nettools.h>
#include <tk/sys/log.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/if_packet.h>
#include <netdb.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <tk/utils/bytebuffer.h>

struct netlayer_s {
    bytebuffer_t buffer;
    netiface_bmac_t hwr_src;
    netiface_bmac_t hwr_dst;
    uint32_t iface_idx;
};
#define create_ptr(local, param) struct netlayer_s *local = (struct netlayer_s*)param

/**
 * @fn uint16_t netlayer_cksum(uint16_t *buf, uint32_t nbytes)
 * @brief Generic checksum calculation.
 * @param buf The buffer to calculate;
 * @param nbytes The buffer len.
 * @return The checksum.
 */
uint16_t netlayer_cksum(uint16_t *buf, uint32_t nbytes) {
  uint16_t *b = buf;
  uint32_t sum=0, len = nbytes;
  for ( sum = 0; len > 1; len -= 2 ) sum += *b++;
  if ( len == 1 ) sum += *(unsigned char*)b;
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  return (uint16_t) ~sum;
}

/**
 * @fn netlayer_t netlayer_new()
 * @brief Allocate a new netlayer
 * @return netlayer
 */
netlayer_t netlayer_new() {
  struct netlayer_s* nl;
  nl = malloc(sizeof(struct netlayer_s));
  if(!nl) {
    logger(LOG_ERR, "Unable to alloc a memory for the netlayer context!\n");
    return NULL;    
  }
  bzero(nl, sizeof(struct netlayer_s));
  nl->buffer = bytebuffer_new();
  bytebuffer_set_capacity(nl->buffer, 2048);
  return nl;
}

/**
 * @fn void netlayer_delete(netlayer_t nlayer)
 * @brief Release the allocated netlayer
 * @param nlayer netlayer pointer
 */
void netlayer_delete(netlayer_t nlayer) {
  create_ptr(nl, nlayer);
  if(nl->buffer) bytebuffer_delete(nl->buffer), nl->buffer = NULL; 
  free(nl);
}

/**
 * @fn void netlayer_clear(netlayer_t nlayer)
 * @brief Clear the internal buffer.
 * @param nlayer netlayer pointer
 */
void netlayer_clear(netlayer_t nlayer) {
  create_ptr(nl, nlayer);
  if(nl->buffer) bytebuffer_clear(nl->buffer); 
}


/**
 * @fn void netlayer_ethernet(netlayer_t nlayer, netiface_mac_t hwr_src, netiface_mac_t hwr_dst, uint32_t iface_idx, int next_eth_p)
 * @brief Build the ethernet header
 * @param nlayer netlayer pointer
 * @param hwr_src The source MAC address
 * @param hwr_dst The destination MAC address
 * @param iface_idx The interface idx to use
 * @param next_eth_p The next packet type (eg: ETH_P_IP)
 */
void netlayer_ethernet(netlayer_t nlayer, netiface_mac_t hwr_src, netiface_mac_t hwr_dst, uint32_t iface_idx, int next_eth_p) {
  create_ptr(nl, nlayer);
  nl->iface_idx = iface_idx;
  memset(&nl->hwr_src, 0, sizeof(netiface_bmac_t));
  memset(&nl->hwr_dst, 0, sizeof(netiface_bmac_t));
  nettools_str2mac(hwr_src, nl->hwr_src);
  nettools_str2mac(hwr_dst, nl->hwr_dst);

  struct ether_header eh;
  memset(&eh, 0, sizeof(struct ether_header));
  memcpy(eh.ether_shost, nl->hwr_src, sizeof(netiface_bmac_t));
  memcpy(eh.ether_dhost, nl->hwr_dst, sizeof(netiface_bmac_t));
  eh.ether_type = htons(next_eth_p);//ETH_P_IP
  /* add the header to the buffer */
  bytebuffer_copy(nl->buffer, (char*)&eh, sizeof(struct ether_header));
}

/**
 * @fn void netlayer_ip4(netlayer_t nlayer, uint8_t tos, uint8_t ttl, char* src_ip, char* dst_ip, uint8_t next_ipproto)
 * @brief Build the ip v4 header
 * @param nlayer netlayer pointer
 * @param tos The TOS value (eg: NETLAYER_DEFAULT_IP4_TOS)
 * @param ttl The TTL value (eg: NETLAYER_DEFAULT_IP4_TTL)
 * @param src_ip The source IPv4 address
 * @param dst_ip The destination IPv4 address
 * @param next_ipproto The next IP protocol (eg: IPPROTO_ICMP)
 */
void netlayer_ip4(netlayer_t nlayer, uint8_t tos, uint8_t ttl, char* src_ip, char* dst_ip, uint8_t next_ipproto) {
  create_ptr(nl, nlayer);
  struct iphdr iph;
  memset(&iph, 0, sizeof(struct iphdr));
  /* IP Header */
  iph.ihl = 5;
  iph.version = 4;
  iph.tos = tos; // 16 - Low delay
  iph.id = htons(54321);
  iph.ttl = ttl; // hops
  iph.protocol = next_ipproto;
  /* Source IP address, can be spoofed */
  iph.saddr = inet_addr(src_ip);
  /* Destination IP address */
  iph.daddr = inet_addr(dst_ip);
  /* add the header to the buffer */
  bytebuffer_append(nl->buffer, (char*)&iph, sizeof(struct iphdr));
}


/**
 * @fn void netlayer_payload(netlayer_t nlayer, uint8_t* buffer, uint32_t length)
 * @brief Add extra payload (stored in call orders)
 * @param nlayer netlayer pointer
 * @param buffer The buffer to add
 * @param length The buffer length
 */
void netlayer_payload(netlayer_t nlayer, uint8_t* buffer, uint32_t length) {
  create_ptr(nl, nlayer);
  bytebuffer_append(nl->buffer, (char*)buffer, length);
}

/**
 * @fn int netlayer_finish(netlayer_t nlayer, int fd)
 * @brief Finish the packets and send ti the FD.
 * @param nlayer netlayer pointer
 * @param fd The socket FD
 * @return The result of sendto (man sendto)
 */
int netlayer_finish(netlayer_t nlayer, int fd) {
  create_ptr(nl, nlayer);
  bytebuffer_trim_to_size(nl->buffer);
  uint8_t* buf = (uint8_t*)bytebuffer_to_bytes(nl->buffer);
  uint32_t length = bytebuffer_length(nl->buffer);
  struct ether_header *eh = (struct ether_header *)buf;
  if(eh->ether_type == htons(ETH_P_IP)) {
    struct iphdr *iph = (struct iphdr *)(buf + sizeof(struct ether_header));
    /* Length of IP payload and header */
    iph->tot_len = htons(length - sizeof(struct ether_header));
    /* Calculate IP checksum on completed header */
    iph->check = netlayer_cksum((uint16_t *)(buf+sizeof(struct ether_header)), sizeof(struct iphdr)/2);
  }
  /* socket builder */
  /* Destination address */
  struct sockaddr_ll socket_address;
  /*prepare sockaddr_ll*/
  /*RAW communication*/
  socket_address.sll_family   = PF_PACKET;
  /*we don't use a protocoll above ethernet layer
    ->just use anything here*/
  socket_address.sll_protocol = eh->ether_type; //htons(ETH_P_IP);
  /*index of the network device
    see full code later how to retrieve it*/
  socket_address.sll_ifindex  = nl->iface_idx;
  /*ARP hardware identifier is ethernet*/
  socket_address.sll_hatype   = ARPHRD_ETHER;
  socket_address.sll_pkttype  = PACKET_OTHERHOST;
  /*address length*/
  socket_address.sll_halen    = ETH_ALEN;
  /*MAC - begin*/
  memcpy(socket_address.sll_addr, nl->hwr_dst, ETH_ALEN);
  /*MAC - end*/
  socket_address.sll_addr[6]  = 0x00;/*not used*/
  socket_address.sll_addr[7]  = 0x00;/*not used*/
  /* Send packet */
  return sendto(fd, buf, length, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll));
}


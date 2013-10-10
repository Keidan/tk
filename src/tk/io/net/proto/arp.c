/**
*******************************************************************************
* @file arp.c
* @author Keidan
* @date 07/10/2013
* @par Project
* tk
*
* @par Copyright
* Copyright 2011-2013 Keidan, all right reserved
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
#include <tk/io/net/proto/arp.h>
#include <tk/io/net/nettools.h>
#include <tk/utils/string.h>
#include <tk/sys/log.h>
#include <tk/sys/log.h>
#include <netinet/if_ether.h>
#include <linux/if_packet.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

typedef struct {
    uint8_t dst[6];
    uint8_t src[6];
    uint8_t proto[2];
} ethhdr_t;

/**
 * @fn static void arp_preare_buffer(arp_buffer_t buffer)
 * @brief Prepare the ARP buffer.
 * @param buffer The buffer to prepare.
 */
static void arp_preare_buffer(arp_buffer_t buffer) {
  memset(buffer, 0, sizeof(arp_buffer_t));
  ethhdr_t *eth = (ethhdr_t*)buffer;
  struct  ether_arp *earp = (struct  ether_arp*)(buffer + sizeof(ethhdr_t));
  eth->proto[0] = 0x08; eth->proto[0] = 0x06;
  earp->ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
  earp->ea_hdr.ar_pro = htons(ETH_P_IP);
  earp->ea_hdr.ar_hln = 6;
  earp->ea_hdr.ar_pln = 4;
  earp->ea_hdr.ar_op = 0x00;
}

/**
 * @fn int arp_send_request(int fd, struct sockaddr_ll *ndev, struct netiface_info_s me, arp_buffer_t buffer, netiface_ip4_t dest, _Bool debug)
 * @brief Send an ARP request.
 * @param fd The RAW socket FD.
 * @param ndev The sock struct used to send this packet(can be NULL)
 * @param me The local config.
 * @param buffer The buffer (initialized by this function).
 * @param dest The destination IP address.
 * @param debug Debug mode.
 * @return -1 on error else 0.
 */
int arp_send_request(int fd, struct sockaddr_ll *ndev, struct netiface_info_s me, arp_buffer_t buffer, netiface_ip4_t dest, _Bool debug) {
  ethhdr_t *eth = (ethhdr_t*)buffer;
  struct ether_arp *earp = (struct  ether_arp*)(buffer + sizeof(ethhdr_t));  
  struct sockaddr_ll nic_dev;
  struct sockaddr_in n;
  //Initialize
  memset(&nic_dev, 0, sizeof(struct sockaddr_ll));
  nic_dev.sll_ifindex = me.index;
  nic_dev.sll_hatype =  ARPHRD_ETHER;
  nic_dev.sll_family = PF_PACKET;
  nic_dev.sll_halen = ETH_ALEN;
  nic_dev.sll_protocol = htons(ETH_P_ARP);
  nic_dev.sll_pkttype = PACKET_OTHERHOST;
  arp_preare_buffer(buffer);

  memset(eth->dst, 0x00, ETH_ALEN); //BCast
  nettools_str2mac(me.mac, eth->src);

  earp->ea_hdr.ar_op = htons(ARPOP_REQUEST);   /*  1 == Request  */
  nettools_str2mac(me.mac, earp->arp_sha);
  inet_aton((char*)me.ip4, &n.sin_addr);
  memcpy(earp->arp_spa, &n.sin_addr, 4);
  memset(earp->arp_tha, 0x00, ETH_ALEN);
  inet_aton((char*)dest, &n.sin_addr);
  memcpy(earp->arp_tpa, &n.sin_addr, 4);

  if(sendto(fd, buffer, sizeof(arp_buffer_t), 0, (struct sockaddr const*)&nic_dev, sizeof(nic_dev)) == -1) {
    logger(LOG_ERR, "sendto failed: (%d) %s\n", errno, strerror(errno));
    return -1;
  }
  if(debug) logger(LOG_DEBUG, "Sending ARP Request ...  ARP Header SrcIP: %s, DstIP: %s, SrcHw: %02X:%02X:%02X:%02X:%02X:%02X, DstHw: %02X:%02X:%02X:%02X:%02X:%02X ...\n",
		   me.ip4, dest,
		   earp->arp_sha[0], earp->arp_sha[1], earp->arp_sha[2],
		   earp->arp_sha[3], earp->arp_sha[4], earp->arp_sha[5],
		   earp->arp_tha[0], earp->arp_tha[1], earp->arp_tha[2],
		   earp->arp_tha[3], earp->arp_tha[4], earp->arp_tha[5]);
  if(ndev)
    memcpy(ndev, &nic_dev, sizeof(struct sockaddr_ll));
  return 0;
}

/**
 * @fn int arp_is_arp_frame(arp_buffer_t frame, unsigned int l)
 * @brief Test if the current frame is a vaid ARP frame. 
 * @param frame The frame to test.
 * @param l The frame length.
 * @return -1 = non arp, 0 = arp and 1 = arp reply.
 */
int arp_is_arp_frame(arp_buffer_t frame, unsigned int l) {
  //returns : -1 = non arp, 0 = arp, 1 = arp reply
  if(l < sizeof(arp_buffer_t))
    return -1;
  ethhdr_t *eth = (ethhdr_t*)frame;
  struct ether_arp *earp = (struct ether_arp*)(frame + sizeof(ethhdr_t));  
  if(eth->proto[0] == 0x08 && eth->proto[1] == 0x06) {
    if(earp->ea_hdr.ar_op == htons(ARPOP_REPLY))
      return 1;
    else
      return 0;
  }
  return -1; //securite
}

/**
 * @fn static unsigned long arp_msectime();
 * @brief Get the current time in ms.
 * @return The current time.
 */
static unsigned long arp_msectime() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return ((t.tv_sec) * 1000 + t.tv_usec/1000);
}

/**
 * @fn static int arp_recvfrom_timeout(int fd, long sec)
 * @brief Wait for input datas.
 * @param fd The RAW socket FD.
 * @param sec The seconds nb before timeout.
 * @return -1 on error, 0 on timeout else >=1
 */
static int arp_recvfrom_timeout(int fd, long sec) {
  // Setup timeval variable
  struct timeval timeout;
  timeout.tv_sec = sec;
  timeout.tv_usec = 0;
  // Setup fd_set structure
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  return select(fd+1, &fds, 0, 0, &timeout);
}


/**
 * @fn int arp_resolve_ip(struct arpcfg_s cfg, struct netiface_info_s me, netiface_ip4_t dest_ip, netiface_mac_t *dest_mac)
 * @brief Resolve the ARP request.
 * @param cfg ARP config.
 * @param me Internal iface config.
 * @param dest_ip The dest ip4 to resolve.
 * @param dest_mac The resolved mac.
 * @retrn -1 on error else 0.
 */
int arp_resolve_ip(struct arpcfg_s cfg, struct netiface_info_s me, netiface_ip4_t dest_ip, netiface_mac_t *dest_mac) {
  struct sockaddr_ll nic_dev = {0};
  nic_dev.sll_family   = PF_PACKET;
  nic_dev.sll_protocol = htons(ETH_P_ALL);
  nic_dev.sll_pkttype  = PACKET_OTHERHOST;
  nic_dev.sll_addr[6]  = 0x00;
  nic_dev.sll_addr[7]  = 0x00;
  arp_buffer_t buffer;
  int fd, ret;
  unsigned int i;
  _Bool end = 0;
  fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if(fd == -1) {
    logger(LOG_ERR, "Socket open failed: (%d) %s\n", errno, strerror(errno));
    return -1;
  }
  __u8 iip[4];
  char* temp = strdup(dest_ip);
  char* bck = temp;
  for(i = 0; i < 4; i++)
    iip[i] = string_parse_int(strsep(&temp, "."), 0);
  free(bck);
  memset(buffer, 0, sizeof(arp_buffer_t));
  arp_preare_buffer(buffer);
  for(i = 0; ((i<cfg.max_attempts) && !end ); ++i) {
    ret = 0;
    unsigned long sentat = arp_msectime();
    if(cfg.debug) logger(LOG_DEBUG, "Resolving %s, attempt %d\n", dest_ip, i+1);
    if(arp_send_request(fd, &nic_dev, me, buffer, dest_ip, cfg.debug) == -1) {
      close(fd);
      return -1;
    }
    while(( arp_msectime() < (sentat + cfg.timeout)) && !end) {
      memset(buffer, 0, sizeof(arp_buffer_t));
      socklen_t msize = sizeof(nic_dev);
      switch(arp_recvfrom_timeout(fd, cfg.timeout)) {//Data received ?
	case -1:
	  if(cfg.debug) logger(LOG_ERR, "Error while reading socket\n");
	  break;
	case 0:
	  if(cfg.debug) logger(LOG_ERR, "Can't resolve IP %s\n", dest_ip);
	  break;
	default: //Read datas!
	  ret = recvfrom(fd, buffer, sizeof(arp_buffer_t), 0, (struct sockaddr*)&nic_dev, &msize);
	  if(cfg.debug) logger(LOG_DEBUG, "ARP: Read %d bytes\n", ret);
	  _Bool isreply=0;
	  switch(arp_is_arp_frame(buffer, ret)) {
	    case 1:
	      if(cfg.debug) logger(LOG_DEBUG, "ARP Reply\n");
	      isreply = 1;
	      break;
	    case 0:
	      if(cfg.debug) logger(LOG_DEBUG, "ARP Request\n");
	      break;
	    case -1:
	      if(cfg.debug) logger(LOG_DEBUG, "Other packet...\n");
	      break;
	  }
	  if(isreply) {
	    struct ether_arp *earp = (struct  ether_arp*)(buffer + sizeof(ethhdr_t));  
	    if(cfg.debug) logger(LOG_DEBUG, "Reply... Compare IP %d.%d.%d.%d with ip %s\n",
			     iip[0], iip[1], iip[2], iip[3], dest_ip);
	    if(memcmp(earp->arp_spa, iip, 4) == 0) {
	      netiface_bmac_t b;
	      memcpy(b, earp->arp_sha, 6);
	      nettools_mac2str(b, *dest_mac);
	      if(cfg.debug) logger(LOG_DEBUG, "Reply is for me, got MAC Address: %s\n", dest_mac);
	      end = 1;
	      close(fd);
	      return 1;
	    } else {
	      if(cfg.debug) logger(LOG_DEBUG, "Other reply for MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
				   earp->arp_sha[0], earp->arp_sha[1], earp->arp_sha[2],
				   earp->arp_sha[3], earp->arp_sha[4], earp->arp_sha[5]);
	    }
	  }
      }
    } 
  } 
  close(fd);
  return 0;
}

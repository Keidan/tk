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
#include <tk/sys/systools.h>
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
#include <sys/ioctl.h>
#include <unistd.h>

typedef struct {
    uint8_t dst[6];
    uint8_t src[6];
    uint8_t proto[2];
} ethhdr_t;

/**
 * @fn int arp_find_from_table(char* ip, struct arp_entry_s *entry)
 * @brief Search an arp entry from the system table.
 * @param ip The ip or the hostname to search.
 * @param entry The result entry (only available if this function return 1).
 * @return -1: error or not found, 0: found
 */
int arp_find_from_table(char* ip, struct arp_entry_s *entry) {
  char** keys;
  int count, i, fd;
  htable_t ifaces;
  netiface_t iface;
  _Bool found = 0;
  struct arpreq arpreq;
  struct sockaddr_in *sin;

  if(!entry) {
    logger(LOG_ERR, "Null entry pointer!\n");
    return -1;
  }
  memset(entry, 0, sizeof(struct arp_entry_s));
  memset(&arpreq, 0, sizeof(arpreq));
  
  sin = (struct sockaddr_in *) &arpreq.arp_pa;
  if(nettools_ip_to_sockaddr(ip, sin) == -1) return -1;

  ifaces = netiface_list_new(NETIFACE_LVL_UDP, NETIFACE_KEY_NAME);
  count = htable_get_keys(ifaces, &keys);
  for(i = 0; i < count; i++) {
    found = 0;
    iface = netiface_list_get(ifaces, keys[i]);
    netiface_get_fd(iface, &fd);
    netiface_get_name(iface, arpreq.arp_dev);
    if (ioctl(fd, SIOCGARP, &arpreq) < 0) {
      logger(LOG_ERR, "SIOCGARP: (%d) %s\n", errno, strerror(errno));
      continue;
    }
    if(!strcmp(ip, inet_ntoa(sin->sin_addr))) {
      found = 1;
      break;
    }
  }
  netiface_list_delete(ifaces);
  if(!found) {
    logger(LOG_ERR, "%s was not found!\n", ip);
    return -1;
  }
  strcpy(entry->name, arpreq.arp_dev);
  strcpy(entry->ip, inet_ntoa(sin->sin_addr));
  entry->flags = arpreq.arp_flags;
  if (entry->flags & ATE_COM) {
    strcpy(entry->name, arpreq.arp_dev);
    strcpy(entry->ip, inet_ntoa(sin->sin_addr));
    entry->flags = arpreq.arp_flags;
    memcpy(entry->bmac, (unsigned char *) &arpreq.arp_ha.sa_data[0], 6);
    nettools_mac2str(entry->bmac, entry->mac);
  } else {
    logger(LOG_ERR, "SIOCGARP failed: *** INCOMPLETE ***\n");
    return -1;
  }
  return 0;
}

/**
 * @fn int arp_add_in_table(netiface_name_t name, const char *ip, netiface_mac_t mac)
 * @brief Add a new entry into the ARP table.
 * @param name The interface name.
 * @param ip The ip or the hostname to add.
 * @param mac The mac to add.
 * @return -1 on error else 0;
 */
int arp_add_in_table(netiface_name_t name, const char *ip, netiface_mac_t mac) {
  int fd;
  struct arpreq arpreq;
  struct sockaddr_in *sin;

  memset(&arpreq, 0, sizeof(arpreq));
  
  sin = (struct sockaddr_in *) &arpreq.arp_pa;
  if(nettools_ip_to_sockaddr(ip, sin) == -1) return -1;
  strcpy(arpreq.arp_dev, name);
  arpreq.arp_flags = ATE_COM;
  nettools_str2mac(mac, (unsigned char *) &arpreq.arp_ha.sa_data[0]);
  if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
    logger(LOG_DEBUG, "socket failed: (%d) %s'\n", errno, strerror(errno));
    return -1;
  } 
  if (ioctl(fd, SIOCSARP, &arpreq) < 0) {
    close(fd);
    logger(LOG_ERR, "SIOCGARP: (%d) %s\n", errno, strerror(errno));
    return -1;
  }
  close(fd);
  return 0;
}

/**
 * @fn _Bool arp_entry_is_inuse(const struct arp_entry_s *entry)
 * @brief Test if the ARP entry contains the flag inuse.
 * @param entry The entry to test.
 * @return 0 if the entry does not contains the flag else 1.
 */
_Bool arp_entry_is_inuse(const struct arp_entry_s *entry) {
  return entry && entry->flags & ATE_INUSE;
}

/**
 * @fn _Bool arp_entry_is_completed(const struct arp_entry_s *entry)
 * @brief Test if the ARP entry contains the flag completed.
 * @param entry The entry to test.
 * @return 0 if the entry does not contains the flag else 1.
 */
_Bool arp_entry_is_completed(const struct arp_entry_s *entry) {
  return entry && entry->flags & ATE_COM;
}

/**
 * @fn _Bool arp_entry_is_permanent(const struct arp_entry_s *entry) 
 * @brief Test if the ARP entry contains the flag permanent.
 * @param entry The entry to test.
 * @return 0 if the entry does not contains the flag else 1.
 */
_Bool arp_entry_is_permanent(const struct arp_entry_s *entry) {
  return entry && entry->flags & ATE_PERM;
}

/**
 * @fn _Bool arp_entry_is_published(const struct arp_entry_s *entry)
 * @brief Test if the ARP entry contains the flag published.
 * @param entry The entry to test.
 * @return 0 if the entry does not contains the flag else 1.
 */
_Bool arp_entry_is_published(const struct arp_entry_s *entry) {
  return entry && entry->flags & ATE_PUBL;
}

/**
 * @fn _Bool arp_entry_is_trailers(const struct arp_entry_s *entry)
 * @brief Test if the ARP entry contains the flag trailers.
 * @param entry The entry to test.
 * @return 0 if the entry does not contains the flag else 1.
 */
_Bool arp_entry_is_trailers(const struct arp_entry_s *entry) {
  return entry && entry->flags & ATE_USETRAILERS;
}

/**
 * @fn _Bool arp_entry_is_proxy(const struct arp_entry_s *entry)
 * @brief Test if the ARP entry contains the flag proxy.
 * @param entry The entry to test.
 * @return 0 if the entry does not contains the flag else 1.
 */
_Bool arp_entry_is_proxy(const struct arp_entry_s *entry) {
  return entry && entry->flags & ATE_PROXY;
}


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
 * @fn int arp_resolve_ip(struct arpcfg_s cfg, struct netiface_info_s me, netiface_ip4_t dest_ip, netiface_mac_t *dest_mac)
 * @brief Resolve the ARP request.
 * @param cfg ARP config.
 * @param me Internal iface config.
 * @param dest_ip The dest ip4 to resolve.
 * @param dest_mac The resolved mac.
 * @retrn -1 on error else 0.
 */
int arp_resolve_ip(struct arpcfg_s cfg, struct netiface_info_s me, netiface_ip4_t dest_ip, netiface_mac_t *dest_mac) {
  struct arp_entry_s entry;
  struct sockaddr_ll nic_dev = {0};
  arp_buffer_t buffer;
  int fd, ret;
  unsigned int i;
  _Bool end = 0;
  memset(*dest_mac, 0, sizeof(netiface_mac_t));
  /* check if the mac is prensent */
  if(!arp_find_from_table(dest_ip, &entry)) {
    strcpy(*dest_mac, entry.mac);
    return 0;
  }
  
  nic_dev.sll_family   = PF_PACKET;
  nic_dev.sll_protocol = htons(ETH_P_ALL);
  nic_dev.sll_pkttype  = PACKET_OTHERHOST;
  nic_dev.sll_addr[6]  = 0x00;
  nic_dev.sll_addr[7]  = 0x00;

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
    unsigned long sentat = systools_msectime();
    if(cfg.debug) logger(LOG_DEBUG, "Resolving %s, attempt %d\n", dest_ip, i+1);
    if(arp_send_request(fd, &nic_dev, me, buffer, dest_ip, cfg.debug) == -1) {
      close(fd);
      return -1;
    }
    while(( systools_msectime() < (sentat + cfg.timeout)) && !end) {
      memset(buffer, 0, sizeof(arp_buffer_t));
      socklen_t msize = sizeof(nic_dev);
      switch(nettools_recvfrom_timeout(fd, cfg.timeout, 0)) {//Data received ?
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
	      arp_add_in_table(me.name, dest_ip, *dest_mac); 
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

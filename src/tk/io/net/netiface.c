/**
*******************************************************************************
* @file netiface.c
* @author Keidan
* @date 23/09/2013
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
#include <tk/io/net/netiface.h>
#include <tk/sys/log.h>
#include <tk/utils/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <unistd.h>

#define NETIFACE_MAGIC 0xC0071ACE
#define create_ptr(local, param) struct netiface_s *local = (struct netiface_s*)param
#define test_ptr(ptr) (ptr && ptr->magic == NETIFACE_MAGIC)

struct netiface_s {
    __u32 magic;
    netiface_name_t name;
    __u32 index;
    int fd;
};



/**
 * @fn htable_t netiface_list_new(netiface_sock_level level, netiface_key_type type)
 * @brief List all available interfaces.
 * @param level Socket level.
 * @param type The table key type
 * @return The table list (key:see key type, value:netiface_t)
 */
htable_t netiface_list_new(netiface_sock_level level, netiface_key_type type) {
  htable_t table = htable_new();
  struct netiface_s iface;
  int i;
  struct ifreq ifr;

  memset(&ifr, 0, sizeof(ifr));

  /* List all network devices */
  struct if_nameindex *nameindex = if_nameindex();
  if(nameindex == NULL){
    logger(LOG_ERR, "if_nameindex: (%d) %s.\n", errno, strerror(errno));
    htable_delete(table);
    return NULL;
  }

  /* loop for each interfaces */
  i = 0; /* init */
  while(1){
    if(!nameindex[i].if_name) break;
    /* Get the iface name */
    memset(&iface, 0, sizeof(struct netiface_s));
    strncpy(iface.name, nameindex[i++].if_name, sizeof(netiface_name_t));
    
    /* Create a socket*/
    /* Socket raw by default*/
    if(level == NETIFACE_LVL_TCP)
      iface.fd = socket(AF_INET, SOCK_STREAM, 0);
    else if(level == NETIFACE_LVL_UDP)
      iface.fd = socket(AF_INET, SOCK_DGRAM, 0);
    else
      iface.fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(iface.fd < 0) {
      if_freenameindex(nameindex);
      htable_delete(table);
      logger(LOG_ERR, "socket failed: (%d) %s.\n", errno, strerror(errno));
      return NULL;
    }

    /* Get the iface index */
    strncpy((char *)ifr.ifr_name, iface.name, IF_NAMESIZE);
    if((ioctl(iface.fd, SIOCGIFINDEX, &ifr)) == -1) {
      htable_delete(table);
      if_freenameindex(nameindex);
      close(iface.fd);
      logger(LOG_ERR, "get index failed: (%d) %s.\n", errno, strerror(errno));
      return NULL;
    }
    iface.index = ifr.ifr_ifindex;
    iface.magic = NETIFACE_MAGIC;
    if(type == NETIFACE_KEY_FD)
      htable_add(table, (char*)string_convert(iface.fd, 10), &iface, sizeof(struct netiface_s));
    else if(type == NETIFACE_KEY_INDEX)
      htable_add(table, (char*)string_convert(iface.index, 10), &iface, sizeof(struct netiface_s));
    else /* key name by default */
      htable_add(table, iface.name, &iface, sizeof(struct netiface_s));
  }
  /* Release the pointer */
  if_freenameindex(nameindex);
  return table;
}

/**
 * @fn void netiface_list_delete(htable_t table)
 * @brief Clear the iface list.
 * @param table The table to clear.
 */
void netiface_list_delete(htable_t table) {
  if(!table) return;
  struct netiface_s *iface;
  char** keys = NULL;
  __u32 i, count = htable_get_keys(table, &keys);
  for(i = 0; i < count; i++) {
    iface = htable_lookup(table, keys[i]);
    if(iface) {
      if(iface->fd) close(iface->fd);
      bzero(iface, sizeof(struct netiface_s));
    }
  }
  htable_delete(table);
}


/**
 * @fn int netiface_bind(netiface_t iface)
 * @brief Bind to the iface.
 * @param iface The iface.
 * @return -1 on error else 0.
 */
int netiface_bind(netiface_t iface) {
  create_ptr(iff, iface);
  if(!test_ptr(iff)) return -1;
  struct sockaddr_ll sll;

  memset(&sll, 0, sizeof(sll));

  sll.sll_family = PF_PACKET;
  sll.sll_ifindex = iff->index;
  sll.sll_protocol = htons(ETH_P_ALL); /* listen all packets */
  /* Bind*/
  if((bind(iff->fd, (struct sockaddr *)&sll, sizeof(sll))) == -1) {
    logger(LOG_ERR, "bind failed: (%d) %s.\n", errno, strerror(errno));
    return -1;
  }
  return 0;
}

/**
 * @fn int netiface_read(const netiface_t iface, netiface_info_t info)
 * @brief Read some informations from the iface.
 * @param iface The iface handle.
 * @param info The iface informations (no pointer allocated here).
 * @return -1 on error or if the iface is not found else 0 on success.
 */
int netiface_read(const netiface_t iface, netiface_info_t info) {
  struct ifreq devea;
  struct sockaddr_in *sa;
  create_ptr(iff, iface);
  if(!test_ptr(iff)) return -1;
  bzero(info, sizeof(struct netiface_info_s));

  strcpy(info->name, iff->name);

  // Get the IPv4 address
  strcpy(devea.ifr_name, iff->name);
  if(ioctl(iff->fd, SIOCGIFADDR, &devea) == 0) {
    sa = (struct sockaddr_in *)&devea.ifr_addr;
    strcpy(info->ip4, inet_ntoa(sa->sin_addr));
  } else
    logger(LOG_ERR, "Unable to get the ipv4 address: (%d) %s\n", errno, strerror(errno));


  // Get the sub netmask address
  strcpy(devea.ifr_name, iff->name);
  if (ioctl(iff->fd, SIOCGIFNETMASK, &devea) == 0) {
    sa = (struct sockaddr_in*) &devea.ifr_netmask;
    strcpy(info->mask, inet_ntoa(sa->sin_addr));
  } else
    logger(LOG_ERR, "Unable to get the netmask address: (%d) %s\n", errno, strerror(errno));

  // Get the broadcast address
  strcpy(devea.ifr_name, iff->name);
  if (ioctl(iff->fd, SIOCGIFBRDADDR, &devea) == 0) {
    struct sockaddr_in *sbcast = (struct sockaddr_in *)&devea.ifr_broadaddr;
    strcpy(info->bcast, inet_ntoa(sbcast->sin_addr));
  } else
    logger(LOG_ERR, "Unable to get the broad cast address: (%d) %s\n", errno, strerror(errno));

  // Get the mac address and familly
  strcpy(devea.ifr_name, iff->name);
  if (ioctl(iff->fd, SIOCGIFHWADDR, &devea) == 0) {
    info->family = devea.ifr_hwaddr.sa_family;
    sprintf(info->mac, "%02x:%02x:%02x:%02x:%02x:%02x",
	   devea.ifr_hwaddr.sa_data[0]&0xFF,
	   devea.ifr_hwaddr.sa_data[1]&0xFF,
	   devea.ifr_hwaddr.sa_data[2]&0xFF,
	   devea.ifr_hwaddr.sa_data[3]&0xFF,
	   devea.ifr_hwaddr.sa_data[4]&0xFF,
	   devea.ifr_hwaddr.sa_data[5]&0xFF);
  } else
    logger(LOG_ERR, "Unable to get the mac address: (%d) %s\n", errno, strerror(errno));
					
  // Get the flags list
  strcpy(devea.ifr_name, iff->name);
  if (ioctl(iff->fd, SIOCGIFFLAGS, &devea) == 0)
    info->flags = devea.ifr_flags;
  else
    logger(LOG_ERR, "Unable to get the iface flags: (%d) %s\n", errno, strerror(errno));

  // Get the metric
  strcpy(devea.ifr_name, iff->name);
  if (ioctl(iff->fd, SIOCGIFMETRIC, &devea) == 0) {
    info->metric = devea.ifr_metric;
    if(!info->metric) info->metric++;
  }
  else
    logger(LOG_ERR, "Unable to get the iface metric: (%d) %s\n", errno, strerror(errno));

  // Get the MTU
  strcpy(devea.ifr_name, iff->name);
  if (ioctl(iff->fd, SIOCGIFMTU, &devea) == 0)
    info->mtu = devea.ifr_mtu;
  else
    logger(LOG_ERR, "Unable to get the iface mtu: (%d) %s\n", errno, strerror(errno));

  return 0;
}

/**
 * @fn int netiface_write(const netiface_t iface, const netiface_info_t info)
 * @brief Write some informations from the iface.
 * @param iface The iface handle.
 * @param info The iface informations.
 * @return -1 on error or if the iface is not found else 0 on success.
 */
int netiface_write(const netiface_t iface, const netiface_info_t info) {
  create_ptr(iff, iface);
  if(!test_ptr(iff)) return -1;
  struct netiface_info_s local;
  if(netiface_read(iface, &local) == -1) return -1;
  return -1;
}

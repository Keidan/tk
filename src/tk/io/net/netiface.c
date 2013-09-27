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
 * @fn int netiface_get_index(netiface_t iface, int *index)
 * @brief Get the internal iface index.
 * @param iface The iface.
 * @param index The result index.
 * @return 0 on success else -1.
 */
int netiface_get_index(netiface_t iface, int *index) {
  create_ptr(iff, iface);
  if(!test_ptr(iff) || !index) return -1;
  *index = iff->index;
  return 0;
}

/**
 * @fn int netiface_get_fd(netiface_t iface, int *fd)
 * @brief Get the internal iface fd.
 * @param iface The iface.
 * @param fd The result fd.
 * @return 0 on success else -1.
 */
int netiface_get_fd(netiface_t iface, int *fd) {
  create_ptr(iff, iface);
  if(!test_ptr(iff) || !fd) return -1;
  *fd = iff->fd;
  return 0;
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

  // Get the IRQ MAP
  strcpy(devea.ifr_name, iff->name);
  if (ioctl(iff->fd, SIOCGIFMAP, &devea) == 0) {
    struct ifmap m = devea.ifr_map;
    info->map.dma_channel = m.dma;
    info->map.interrupt = m.irq;
    info->map.base_address = m.base_addr;
    info->map.memory_end = m.mem_end;
    info->map.memory_start = m.mem_start;
    info->map.port = m.port;
  }

  return 0;
}

/**
 * @fn _Bool netiface_device_is_up(const netiface_t iface)
 * @brief Test if the current device is up.
 * @param fd Device FD.
 * @return 1 if up else 0..
 */
_Bool netiface_device_is_up(const netiface_t iface) {
  struct ifreq ifr;
  create_ptr(iff, iface);
  if(!test_ptr(iff)) return -1;
  memset(&ifr, 0, sizeof(ifr));
  strncpy((char *)ifr.ifr_name, iff->name, IF_NAMESIZE);
  int ret = ioctl(iff->fd, SIOCGIFFLAGS, &ifr);
  if (ret == -1) {
    logger(LOG_ERR, "flags: (%d) %s.\n", errno, strerror(errno));
    return ret;
  }
  return !!(ifr.ifr_flags & IFF_UP);
}

/**
 * @fn __u32 netiface_datas_available(int fd)
 * @brief Get the number of available datas to be read.
 * @param fd Socket FD.
 * @return Available datas.
 */
__u32 netiface_datas_available(const netiface_t iface) {
  __u32 available = 0;
  create_ptr(iff, iface);
  if(!test_ptr(iff)) return 0;
  int ret = ioctl(iff->fd, FIONREAD, &available);
  if (ret == -1) {
    logger(LOG_ERR, "available: (%d) %s.\n", errno, strerror(errno));
    return 0;
  }
  return available;
}

/**
 * @fn void netiface_print(FILE* out, struct netiface_info_s info)
 * @brief Print iface informations.
 * @param out The output stream.
 * @param info Iface informations.
 */
void netiface_print(FILE* out, struct netiface_info_s info) {
  fprintf(out, "%s\t", info.name);
  fprintf(out, "Link encap: ");
  switch(info.family) {
    case ARPHRD_NETROM:             fprintf(out, "NETROM ");             break;
    case ARPHRD_ETHER:              fprintf(out, "ETHER ");              break;
    case ARPHRD_EETHER:             fprintf(out, "EETHER ");             break;
    case ARPHRD_AX25:               fprintf(out, "AX25 ");               break;
    case ARPHRD_PRONET:             fprintf(out, "PRONET ");             break;
    case ARPHRD_CHAOS:              fprintf(out, "CHAOS ");              break;
    case ARPHRD_IEEE802:            fprintf(out, "IEEE802 ");            break;
    case ARPHRD_ARCNET:             fprintf(out, "ARCNET ");             break;
    case ARPHRD_APPLETLK:           fprintf(out, "APPLETLK ");           break;
    case ARPHRD_DLCI:               fprintf(out, "DLCI ");               break;
    case ARPHRD_ATM:                fprintf(out, "ATM ");                break;
    case ARPHRD_METRICOM:           fprintf(out, "METRICOM ");           break;
    case ARPHRD_IEEE1394:           fprintf(out, "IEEE1394 ");           break;
    case ARPHRD_EUI64:              fprintf(out, "EUI64 ");              break;
    case ARPHRD_INFINIBAND:         fprintf(out, "INFINIBAND ");         break;
    case ARPHRD_SLIP:               fprintf(out, "SLIP ");               break;
    case ARPHRD_CSLIP:              fprintf(out, "CSLIP ");              break;
    case ARPHRD_SLIP6:              fprintf(out, "SLIP6 ");              break;
    case ARPHRD_CSLIP6:             fprintf(out, "CSLIP6 ");             break;
    case ARPHRD_RSRVD:              fprintf(out, "RSRVD ");              break;
    case ARPHRD_ADAPT:              fprintf(out, "ADAPT ");              break;
    case ARPHRD_ROSE:               fprintf(out, "ROSE ");               break;
    case ARPHRD_X25:                fprintf(out, "X25 ");                break;
    case ARPHRD_HWX25:              fprintf(out, "HWX25 ");              break;
    case ARPHRD_PPP:                fprintf(out, "PPP ");                break;
    case ARPHRD_CISCO:              fprintf(out, "CISCO ");              break;
    case ARPHRD_LAPB:               fprintf(out, "LAPB ");               break;
    case ARPHRD_DDCMP:              fprintf(out, "DDCMP ");              break;
    case ARPHRD_RAWHDLC:            fprintf(out, "RAWHDLC ");            break;
    case ARPHRD_TUNNEL:             fprintf(out, "TUNNEL ");             break;
    case ARPHRD_TUNNEL6:            fprintf(out, "TUNNEL6 ");            break;
    case ARPHRD_FRAD:               fprintf(out, "FRAD ");               break;
    case ARPHRD_SKIP:               fprintf(out, "SKIP ");               break;
    case ARPHRD_LOOPBACK:           fprintf(out, "LOOPBACK ");           break;
    case ARPHRD_LOCALTLK:           fprintf(out, "LOCALTLK ");           break;
    case ARPHRD_FDDI:               fprintf(out, "FDDI ");               break;
    case ARPHRD_BIF:                fprintf(out, "BIF ");                break;
    case ARPHRD_SIT:                fprintf(out, "SIT ");                break;
    case ARPHRD_IPDDP:              fprintf(out, "IPDDP ");              break;
    case ARPHRD_IPGRE:              fprintf(out, "IPGRE ");              break;
    case ARPHRD_PIMREG:             fprintf(out, "PIMREG ");             break;
    case ARPHRD_HIPPI:              fprintf(out, "HIPPI ");              break;
    case ARPHRD_ASH:                fprintf(out, "ASH ");                break;
    case ARPHRD_ECONET:             fprintf(out, "ECONET ");             break;
    case ARPHRD_IRDA:               fprintf(out, "IRDA ");               break;
    case ARPHRD_FCPP:               fprintf(out, "FCPP ");               break;
    case ARPHRD_FCAL:               fprintf(out, "FCAL ");               break;
    case ARPHRD_FCPL:               fprintf(out, "FCPL ");               break;
    case ARPHRD_FCFABRIC:           fprintf(out, "FCFABRIC ");           break;
    case ARPHRD_IEEE802_TR:         fprintf(out, "IEEE802_TR ");         break;
    case ARPHRD_IEEE80211:          fprintf(out, "IEEE80211 ");          break;
    case ARPHRD_IEEE80211_PRISM:    fprintf(out, "IEEE80211_PRISM ");    break;
    case ARPHRD_IEEE80211_RADIOTAP: fprintf(out, "IEEE80211_RADIOTAP "); break;
    case ARPHRD_VOID:               fprintf(out, "VOID ");               break;
    case ARPHRD_NONE:
    default:
      fprintf(out, "NONE ");               break;
      break;
  }
  if(!IFACE_IS_LOOPBACK(info.flags))
    fprintf(out, "HWaddr %s", info.mac);
  fprintf(out, "\n");
  if(IFACE_IS_UP(info.flags)) {
    if(strlen(info.ip4) || strlen(info.bcast) || strlen(info.mask)) {
      fprintf(out, "\t");
      if(strlen(info.ip4))
	fprintf(out, "inet adr:%s ", info.ip4);
      if(strlen(info.bcast))
	fprintf(out, "Bcast:%s ", info.bcast);
      if(strlen(info.mask))
	fprintf(out, "Mask:%s ", info.mask);
      fprintf(out, "\n");
    }
  }
  fprintf(out, "\t");
  fprintf(out, "%s", IFACE_IS_UP(info.flags) ? "UP " : "DOWN "); 
  if(IFACE_IS_LOOPBACK(info.flags))    fprintf(out, "LOOPBACK ");
  if(IFACE_IS_BROADCAST(info.flags))   fprintf(out, "BROADCAST ");
  if(IFACE_IS_RUNNING(info.flags))     fprintf(out, "RUNNING ");
  if(IFACE_IS_MULTICAST(info.flags))   fprintf(out, "MULTICAST ");
  if(IFACE_IS_PROMISC(info.flags))     fprintf(out, "PROMISC ");
  if(IFACE_IS_NOTRAILERS(info.flags))  fprintf(out, "NOTRAILERS ");
  if(IFACE_IS_DEBUG(info.flags))       fprintf(out, "DEBUG ");
  if(IFACE_IS_MASTER(info.flags))      fprintf(out, "MASTER ");
  if(IFACE_IS_SLAVE(info.flags))       fprintf(out, "SLAVE ");
  if(IFACE_IS_PORTSEL(info.flags))     fprintf(out, "PORTSEL ");
  if(IFACE_IS_AUTOMEDIA(info.flags))   fprintf(out, "AUTOMEDIA ");
  if(IFACE_IS_DYNAMIC(info.flags))     fprintf(out, "DYNAMIC ");
  if(IFACE_IS_POINTOPOINT(info.flags)) fprintf(out, "POINTOPOINT ");
  if(IFACE_IS_NOARP(info.flags))       fprintf(out, "NOARP ");
  fprintf(out, " MTU:%d ", info.mtu);
  fprintf(out, " Metric:%d ", info.metric);
  fprintf(out, "\n");
  if(info.map.interrupt || info.map.base_address || info.map.memory_start || info.map.dma_channel) {
    fprintf(out, "\t");
    if (info.map.interrupt)
      fprintf(out, "Interrupt:%d ", ((int)info.map.interrupt));
    if (info.map.base_address >= 0x100)	// Only print devices using it for I/O maps
      fprintf(out, "Base address:%#x ", info.map.base_address);
    if (info.map.memory_start)
      fprintf(out, "Memory:%x-%x ", (unsigned int)info.map.memory_start, (unsigned int)info.map.memory_end);
    if (info.map.dma_channel)
      fprintf(out, "DMA chan:%d ", info.map.dma_channel);
    if (info.map.port)
      fprintf(out, "Port:%d ", (int)info.map.port);
    fprintf(out, "\n");
  }
  fprintf(out, "\n");
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
  struct ifreq devea;


  if(strlen(info->mac) && strcmp(local.mac, info->mac)) {
    int tmp [6], i;
    sscanf(info->mac, "%x:%x:%x:%x:%x:%x", 
	   (&tmp[0]), (&tmp[1]), (&tmp[2]),
	   (&tmp[3]),(&tmp[4]), (&tmp[5]));
    for(i = 0; i < 6; ++i) 
      devea.ifr_hwaddr.sa_data[i] = (unsigned char) tmp[i];
    devea.ifr_hwaddr.sa_family = info->family;
    if (ioctl(iff->fd,SIOCSIFHWADDR, &devea) < 0) {
      logger(LOG_ERR, "Unable to update the iface mac: (%d) %s\n", errno, strerror(errno));
      return -1;
    }
  }
					
  if(local.flags != info->flags) {
    devea.ifr_flags = info->flags;
    if (ioctl(iff->fd, SIOCSIFFLAGS, &devea) < 0) {
      logger(LOG_ERR, "Unable to update the iface flags: (%d) %s\n", errno, strerror(errno));
      return -1;
    }
  }
	
  if(strlen(info->ip4) && strcmp(local.ip4, info->ip4)) {
    struct sockaddr_in *sa = (struct sockaddr_in *)&devea.ifr_addr; 
    sa->sin_family = AF_INET;
    sa->sin_addr.s_addr = inet_addr(info->ip4);
    if (ioctl(iff->fd, SIOCSIFADDR, &devea) < 0) {
      logger(LOG_ERR, "Unable to update the iface ip: (%d) %s\n", errno, strerror(errno));
      return -1;;
    }
  }

  if(strlen(info->mask) && strcmp(local.mask, info->mask)) {
    struct sockaddr_in *sa = (struct sockaddr_in *)&devea.ifr_netmask; 
    sa->sin_family = AF_INET;
    sa->sin_addr.s_addr = inet_addr(info->mask);
    if (ioctl(iff->fd, SIOCSIFNETMASK, &devea) < 0) {
      logger(LOG_ERR, "Unable to update the iface net mask: (%d) %s\n", errno, strerror(errno));
      return -1;
    }
  }

  if(strlen(info->bcast) && strcmp(local.bcast, info->bcast)) {
    struct sockaddr_in *sa = (struct sockaddr_in *)&devea.ifr_broadaddr; 
    sa->sin_family = AF_INET;
    sa->sin_addr.s_addr = inet_addr(info->bcast);
    if (ioctl(iff->fd, SIOCSIFBRDADDR, &devea) < 0) {
      logger(LOG_ERR, "Unable to update the iface bcast address: (%d) %s\n", errno, strerror(errno));
      return -1;
    }
  }

  if(info->mtu != local.mtu) {
    devea.ifr_mtu = info->mtu;
    if (ioctl(iff->fd, SIOCSIFMTU, &devea) < 0) {
      logger(LOG_ERR, "Unable to update the iface mtu: (%d) %s\n", errno, strerror(errno));
      return -1;
    }
  }

  if(info->metric != local.metric) {
    devea.ifr_metric = info->metric;
    if (ioctl(iff->fd, SIOCSIFMETRIC, &devea) < 0) {
      logger(LOG_ERR, "Unable to update the iface metric: (%d) %s\n", errno, strerror(errno));
      return -1;
    }
  }
							
  if(local.map.base_address != info->map.base_address || 
     local.map.dma_channel != info->map.dma_channel ||
     local.map.interrupt != info->map.interrupt ||
     local.map.memory_end != info->map.memory_end ||
     local.map.memory_start != info->map.memory_start ||
     local.map.port != info->map.port) {
    struct ifmap *imap = (struct ifmap *)&devea.ifr_map;
    imap->irq = local.map.interrupt;
    imap->base_addr = local.map.base_address;
    imap->mem_end = local.map.memory_end;
    imap->mem_start = local.map.memory_start;
    imap->dma = local.map.dma_channel;
    imap->port = local.map.port;
    if(local.map.base_address != info->map.base_address)
      imap->base_addr = info->map.base_address;
    if(local.map.dma_channel != info->map.dma_channel)
      imap->dma =  info->map.dma_channel;
    if(local.map.interrupt != info->map.interrupt)
      imap->irq = info->map.interrupt;
    if(local.map.memory_end != info->map.memory_end)
      imap->mem_end = info->map.memory_end;
    if(local.map.memory_start != info->map.memory_start)
      imap->mem_start = info->map.memory_start;
    if(local.map.port != info->map.port)
      imap->port = info->map.port;
    if (ioctl(iff->fd,SIOCSIFMAP, &devea) < 0) {
      logger(LOG_ERR, "Unable to update the iface map: (%d) %s\n", errno, strerror(errno));
      return -1;
    }
  }
  return 0;
}

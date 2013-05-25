/**
 *******************************************************************************
 * @file netutils.c
 * @author Keidan
 * @date 03/01/2013
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <linux/if_packet.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <tk/net/netutils.h>
#include <tk/sys/log.h>

/**
 * @def NETUTILS_PCAP_VERSION_MAJOR
 * @brief Version major du fichier pcap.
 * @see netdump_utils_pcap_global_hdr
 */
#define NETUTILS_PCAP_VERSION_MAJOR     2
/**
 * @def NETUTILS_PCAP_VERSION_MINOR
 * @brief Version minor du fichier pcap.
 * @see netdump_utils_pcap_global_hdr
 */
#define NETUTILS_PCAP_VERSION_MINOR     4
/**
 * @def NETUTILS_PCAP_MAGIC_NATIVE
 * @brief Magic du fichier pcap.
 * @see netdump_utils_pcap_global_hdr
 */
#define NETUTILS_PCAP_MAGIC_NATIVE      0xa1b2c3d4
/**
 * @def NETUTILS_PCAP_LINKTYPE_ETHERNET
 * @brief Type de capture.
 * @see netdump_utils_pcap_global_hdr
 */
#define NETUTILS_PCAP_LINKTYPE_ETHERNET 1
/**
 * @def NETUTILS_PCAP_SNAPLEN
 * @brief Taille de la capture.
 * @see netdump_utils_pcap_global_hdr
 */
#define NETUTILS_PCAP_SNAPLEN           65535


/**
 * Liste toutes les interfaces et les ajoutent a la liste (IMPORTANT: apres appel de cette methode des sockets sont ouverts).
 * @param ifaces Liste des interfaces (la taille vaut 1 ou 0 si iname n'est pas vide).
 * @param maxfd Utilise pour le select.
 * @param rset fd_set utilise pour le select.
 * @param iname Demande la configuration d'une interface.
 * @return -1 en cas d'erreur sinon 0.
 */
int netutils_prepare_ifaces(struct iface_s *ifaces, int *maxfd, fd_set *rset, const char iname[IF_NAMESIZE]) {
  int i;
  struct ifreq ifr;
  struct sockaddr_ll sll;
  char *name;
  int fd, family;

  memset(&sll, 0, sizeof(sll));
  memset(&ifr, 0, sizeof(ifr));

  /* Liste toutes les cartes reseaux du PC */
  struct if_nameindex *nameindex = if_nameindex();
  if(nameindex == NULL){
    logger(LOG_ERR, "if_nameindex: (%d) %s.\n", errno, strerror(errno));
    return -1;
  }

  /* init de la liste */
  INIT_LIST_HEAD(&(ifaces->list));

  /* boucle sur les interfaces */
  i = 0; /* init */
  while(1){
    if(!nameindex[i].if_name) break;
    /* Recuperation du nom qui sera utilise plus bas. */
    name = nameindex[i++].if_name;
    if(iname[0] && strncmp(iname, name, IF_NAMESIZE) != 0) continue;

    /* Creation d'un socket qui sera utilise pour l'ecoute + les ios*/
    /* Socket raw */
    fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(fd < 0) {
      if_freenameindex(nameindex);
      logger(LOG_ERR, "socket failed: (%d) %s.\n", errno, strerror(errno));
      return -1;
    }

    /* Pas d'ajout de l'interface si elle n'est pas UP */
    if(!netutils_device_is_up(fd, name)) {
      close(fd);
      continue;
    }
      
    /* set du fd_set + calcul du maxfd */
    if(fd > *maxfd) *maxfd = fd;
    FD_SET(fd, rset);

    /* Recuperation de l'index correspondant a l'interface reseau  */
    strncpy((char *)ifr.ifr_name, name, IF_NAMESIZE);
    if((ioctl(fd, SIOCGIFINDEX, &ifr)) == -1) {
      if_freenameindex(nameindex);
      close(fd);
      logger(LOG_ERR, "get index failed: (%d) %s.\n", errno, strerror(errno));
      return -1;
    }

    /* Init de la structure sockaddr_ll */
    sll.sll_family = PF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_ALL); /* On veut ecouter tous les paquets */

    /* recuperation de la famille de l'interface. */
    if(ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
      family = ifr.ifr_hwaddr.sa_family;
    } else
      family = -1;

    /* Bind sur l'interface*/
    if((bind(fd, (struct sockaddr *)&sll, sizeof(sll))) == -1) {
      if_freenameindex(nameindex);
      close(fd);
      logger(LOG_ERR, "bind failed: (%d) %s.\n", errno, strerror(errno));
      return -1;
    }

    /* ajout de l'interface */
    netutils_add_iface(ifaces, name, ifr.ifr_ifindex, fd, family);
  }

  /* Liberation des ressources */
  if_freenameindex(nameindex);
  return 0;
}

/**
 * @fn void netutils_add_iface(struct iface_s* list, char name[IF_NAMESIZE], int index, int fd, int family)
 * @brief Ajout d'un interface a la liste.
 * @param list Liste d'interfaces.
 * @param name Nom de l'interface.
 * @param index Index de l'interface.
 * @param fd FD du socket utilise.
 * @param family Famille de l'interface.
 */
void netutils_add_iface(struct iface_s* list, char name[IF_NAMESIZE], int index, int fd, int family) {
  struct iface_s* node;
  node = (struct iface_s*)malloc(sizeof(struct iface_s));
  if(!node) {
    logger(LOG_ERR, "if_nameindex: (%d) %s.\n", errno, strerror(errno));
    return;
  }
  /* init + ajout de l'element */
  strncpy(node->name, name, IF_NAMESIZE);
  node->fd = fd;
  node->index = index;
  node->family = family;
  list_add_tail(&(node->list), &(list->list));
}

/**
 * @fn void netutils_clear_ifaces(struct iface_s* ifaces)
 * @brief Suppression des elements de la liste.
 * @param ifaces Liste a vider.
 */
void netutils_clear_ifaces(struct iface_s* ifaces) {
  struct iface_s* iter;
  while(!list_empty(&ifaces->list) ) {
    iter = list_entry(ifaces->list.next, struct iface_s, list);
    close(iter->fd); /* close du socket */
    list_del(&iter->list); /*delete de l'item dans la liste */
    free(iter);
  }
}

/**
 * @fn _Bool netutils_device_is_up(int fd, char name[IF_NAMESIZE])
 * @brief Effectue un test pour savoir si le device est up
 * @param fd FD pour l'ioctl.
 * @param name Nom du device.
 * @return Vrai si up.
 */
_Bool netutils_device_is_up(int fd, char name[IF_NAMESIZE]) {
  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  /* copy du nom de l'interface */
  strncpy((char *)ifr.ifr_name, name, IF_NAMESIZE);
  /* demande la liste des flags */
  int ret = ioctl(fd, SIOCGIFFLAGS, &ifr);
  if (ret == -1) {
    logger(LOG_ERR, "flags: (%d) %s.\n", errno, strerror(errno));
    return ret;
  }
  /* 1 si le flag up est positionne */
  return !!(ifr.ifr_flags & IFF_UP);
}

/**
 * @fn __u32 netutils_datas_available(int fd)
 * @brief Recuperation du nombre de donnees a lire.
 * @param fd fd a tester.
 * @return Nb donnees a lire. 
 */
__u32 netutils_datas_available(int fd) {
  __u32 available = 0;
  /* demande le nombre d'octets quipeuvent etre lues */
  int ret = ioctl(fd, FIONREAD, &available);
  if (ret == -1) {
    logger(LOG_ERR, "available: (%d) %s.\n", errno, strerror(errno));
    return ret; /* ... */
  }
  return available;
}

/**
 * @fn int netutils_is_ipv4(const char* ip)
 * @brief Test si l'adresse ip est valide.
 * @param ip Adresse IP.
 * @return -1 si erreur, 0 si non match, 1 si match.
 */
int netutils_is_ipv4(const char* ip) {    
  struct in_addr i_addr;
  int ret = 0;
  ret = inet_pton(AF_INET, ip, &i_addr);
  if(ret == 1) return 1;
  else if(ret == 0) return 0;
  return -1;
}
 
/**
 * @fn int netutils_hostname_to_ip(const char *hostname, char* ip)
 * @brief Recuperation de l'adresse ip en fonction du nom de host.
 * @param hostname Nom du host.
 * @param ip Adresse IP.
 * @return -1 si erreur sinon 0.
 */
int netutils_hostname_to_ip(const char *hostname, char* ip) {
  struct hostent *he;
  struct in_addr **addr_list;
  int i;
  if((he = gethostbyname(hostname)) == NULL) {
    logger(LOG_ERR, "gethostbyname: (%d) %s.\n", errno, strerror(errno));
    return -1;
  }
  addr_list = (struct in_addr **) he->h_addr_list;
  for(i = 0; addr_list[i] != NULL; i++) {
    strcpy(ip , inet_ntoa(*addr_list[i]) );
    return 0;
  }
  return -1;
}

/**
 * @fn void netutils_print_hex(FILE* std, char* buffer, int len, _Bool print_raw)
 * @brief Affichage d'un packet (wireshark like).
 * @param std Flux de sortie.
 * @param buffer Packet.
 * @param len Taille du packet.
 * @param print_raw Affichage en raw mode.
 */
void netutils_print_hex(FILE* std, char* buffer, int len, _Bool print_raw) {
  int i = 0, max = PRINT_HEX_MAX_PER_LINES, loop = len;
  __u8 *p = (__u8 *)buffer;
  char line [max + 3]; /* spaces + \0 */
  memset(line, 0, sizeof(line));
  while(loop--) {
    __u8 c = *(p++);
    if(!print_raw) {
      fprintf(std, "%02x ", c);
      /* uniquement les espaces et les char visibles */
      if(c >= 0x20 && c <= 0x7e) line[i] = c;
      /* sinon on masque avec un '.' */
      else line[i] = 0x2e; /* . */
    } else fprintf(std, "%02x", c);
    /* on passe a la ligne suivante */
    if(i == max) {
      if(!print_raw)
	fprintf(std, "  %s\n", line);
      else fprintf(std, "\n");
      /* re init */
      i = 0;
      memset(line, 0, sizeof(line));
    }
    /* sinon suivant */
    else i++;
    /* espace a la moitie */
    if(i == max / 2 && !print_raw) {
      fprintf(std, " ");
      line[i++] = 0x20;
    }
  }
  /* Cette etape permet d'aligner 'line'*/
  if(i != 0 && (i < max || i <= len) && !print_raw) {
    while(i++ <= max) fprintf(std, "   "); /* comble avec 3 espaces ex: "00 " */
    fprintf(std, "  %s\n", line);
  }
  fprintf(std, "\n");
}

/**
 * @fn unsigned int netutils_ip_to_long(const char* s)
 * @brief Transforme une adresse IP en long.
 * @param s IP a transformer.
 * @return Long.
 */
const char* netutils_long_to_ip(unsigned int v)  {
  struct in_addr x;
  x.s_addr = htonl(v);
  return inet_ntoa(x);
}

/**
 * @fn unsigned int netutils_ip_to_long(const char* s)
 * @brief Transforme une adresse IP en long.
 * @param s IP a transformer.
 * @return Long.
 */
unsigned int netutils_ip_to_long(const char* s) {
  struct sockaddr_in n;
  inet_aton(s,&n.sin_addr);
  return ntohl(n.sin_addr.s_addr);
}


/**
 * @fn pcap_hdr_t netutils_pcap_global_hdr(void)
 * @brief Construction du main header du fichier.
 * @return pcap_hdr_t
 */
pcap_hdr_t netutils_pcap_global_hdr(void) {
  pcap_hdr_t hdr;
  memset(&hdr, 0, sizeof(pcap_hdr_t));
  hdr.magic_number = NETUTILS_PCAP_MAGIC_NATIVE;
  hdr.version_major = NETUTILS_PCAP_VERSION_MAJOR;
  hdr.version_minor = NETUTILS_PCAP_VERSION_MINOR;  
  tzset(); /* force le set de la variable timezone */
  hdr.thiszone = timezone;
  hdr.sigfigs = 0;
  hdr.snaplen = NETUTILS_PCAP_SNAPLEN;
  hdr.network = NETUTILS_PCAP_LINKTYPE_ETHERNET;
  return hdr;
}

/**
 * @fn pcap_hdr_t netutils_pcap_packet_hdr(__u32 incl_len, __u32 ori_len)
 * @brief Construction du header par paquets.
 * @return pcaprec_hdr_t.
 */
pcaprec_hdr_t netutils_pcap_packet_hdr(__u32 incl_len, __u32 ori_len) {
  pcaprec_hdr_t hdr;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  hdr.ts_sec = tv.tv_sec;
  hdr.ts_usec = tv.tv_usec;
  hdr.incl_len = incl_len;
  hdr.orig_len = ori_len;
  return hdr;
}

/**
 * @fn void netutils_write_pcap_packet(const FILE* output, const char* buffer, size_t a_length, size_t r_length, _Bool *first)
 * @brief Ecriture des headers pcap et du buffer dans le fichier specifie.
 * Source: http://wiki.wireshark.org/Development/LibpcapFileFormat
 * Packet structure:
 * -----------------------------------------------------------------------------------------------------------------
 * | Global Header | Packet Header | Packet Data | Packet Header | Packet Data | Packet Header | Packet Data | ... |
 * -----------------------------------------------------------------------------------------------------------------
 * @param output Fichier de sortie.
 * @param buffer Buffer d'entree.
 * @param a_length Taille demandee a l'appel de recvfrom.
 * @param r_length Taille recuperee apres l'appel de recvfrom.
 * @param first Cette variable permet l'ecriture du header global, en debut de fichier uniquement.
 */
void netutils_write_pcap_packet(FILE* output, const char* buffer, size_t a_length, size_t r_length, _Bool *first) {
  if(*first) {
    pcap_hdr_t ghdr = netutils_pcap_global_hdr();
    fwrite(&ghdr, 1, sizeof(pcap_hdr_t), output);
    *first = 0;
  }
  pcaprec_hdr_t phdr = netutils_pcap_packet_hdr(r_length, a_length);
  fwrite(&phdr, 1, sizeof(pcaprec_hdr_t), output);
  fwrite(buffer, 1, r_length, output);
  fflush(output);
}


/**
 * @fn int netutils_decode_buffer(const char* buffer, __u32 length, struct netutils_headers_s *net, netutils_convert_et convert)
 * @brief Decodage des paquets en fonction du buffer.
 * @param buffer Buffer de donnee.
 * @param length Taille du buffer.
 * @param net Liste des entetes.
 * @param convert Conversion de certains champs des differentes entetes.
 * @return -1 sur erreur sinon la taill de la payload (peut etre 0).
 */
int netutils_decode_buffer(const char* buffer, __u32 length, struct netutils_headers_s *net, netutils_convert_et convert) {
  __u32 offset = sizeof(struct ethhdr);
  memset(net, 0, sizeof(struct netutils_headers_s));
  struct ethhdr *eth = (struct ethhdr *)buffer;
  net->eth = (struct ethhdr *)malloc(sizeof(struct ethhdr));
  if(!net->eth) {
    logger(LOG_ERR, "Unable to alloc memory for eth header!\n");
    return -1;
  }
  memcpy(net->eth, eth, sizeof(struct ethhdr));
  if(convert == NETUTILS_CONVERT_NET2HOST)
    net->eth->h_proto = ntohs(net->eth->h_proto);
  else if(convert == NETUTILS_CONVERT_HOST2NET)
    net->eth->h_proto = htons(net->eth->h_proto);
  if(net->eth->h_proto == ETH_P_IP || net->eth->h_proto == ETH_P_IPV6) {
    struct iphdr *ip4 = (struct iphdr*)(buffer + offset);

    __u8 protocol = 0;
    if(ip4->version == 4) {
      net->ipv4 = (struct iphdr *)malloc(sizeof(struct iphdr));
      if(!net->ipv4) {
	netutils_release_buffer(net);
	logger(LOG_ERR, "Unable to alloc memory for ipv4 header!\n");
	return -1;
      }
      memcpy(net->ipv4, ip4, sizeof(struct iphdr));
      offset += sizeof(struct iphdr);
      if(convert == NETUTILS_CONVERT_NET2HOST) {
        net->ipv4->tot_len = ntohs(ip4->tot_len);
        net->ipv4->tos = ntohs(ip4->tos); /* pas certains */
        //net->ipv4->ihl = /*ntohl(*/ip4->ihl/*)*/;
        net->ipv4->frag_off = ntohs(ip4->frag_off);
      } else if(convert == NETUTILS_CONVERT_HOST2NET) {
        net->ipv4->tot_len = htons(ip4->tot_len);
        net->ipv4->tos = htons(ip4->tos); /* pas certains */
        //net->ipv4->ihl = /*htonl(*/ip4->ihl/*)*/;
        net->ipv4->frag_off = htons(ip4->frag_off);
      }
      protocol = net->ipv4->protocol;
    }
    if(protocol == IPPROTO_TCP) {
      union tcp_word_hdr *utcp = (union tcp_word_hdr*)(buffer + offset);
      struct tcphdr *tcp = &utcp->hdr;     
      net->tcp = (struct tcphdr *)malloc(sizeof(struct tcphdr));
      if(!net->tcp) {
	netutils_release_buffer(net);
	logger(LOG_ERR, "Unable to alloc memory for tcp header!\n");
	return -1;
      }
      memcpy(net->tcp, tcp, sizeof(struct tcphdr));
      offset += sizeof(union tcp_word_hdr);
      if(convert == NETUTILS_CONVERT_NET2HOST) {
        net->tcp->source = ntohs(net->tcp->source);
        net->tcp->dest = ntohs(net->tcp->dest);
        net->tcp->seq = ntohs(net->tcp->seq);
        net->tcp->ack_seq = ntohs(net->tcp->ack_seq);
        net->tcp->check = ntohs(net->tcp->check);
      } else if(convert == NETUTILS_CONVERT_HOST2NET) {
        net->tcp->source = htons(net->tcp->source);
        net->tcp->dest = htons(net->tcp->dest);
        net->tcp->seq = htons(net->tcp->seq);
        net->tcp->ack_seq = htons(net->tcp->ack_seq);
        net->tcp->check = htons(net->tcp->check);
      }
      if(!net->tcp->psh && !net->tcp->syn && (length - offset)) {
	fprintf(stderr, "TCP Trailer: Not supported (%d bytes)\n", (length - offset));
	offset += (length - offset);
      }
    } else if(protocol == IPPROTO_UDP) {
      struct udphdr *udp = (struct udphdr*)(buffer + offset);
      net->udp = (struct udphdr *)malloc(sizeof(struct udphdr));
      if(!net->udp) {
	netutils_release_buffer(net);
	logger(LOG_ERR, "Unable to alloc memory for udp header!\n");
	return -1;
      }
      memcpy(net->udp, udp, sizeof(struct udphdr));
      offset += sizeof(struct udphdr);
      if(convert == NETUTILS_CONVERT_NET2HOST) {
        net->udp->source = ntohs(net->udp->source);
        net->udp->dest = ntohs(net->udp->dest);
        net->udp->check = ntohs(net->udp->check);
        net->udp->len = ntohs(net->udp->len);
      } else if(convert == NETUTILS_CONVERT_HOST2NET) {
        net->udp->source = htons(net->udp->source);
        net->udp->dest = htons(net->udp->dest);
        net->udp->check = htons(net->udp->check);
        net->udp->len = htons(net->udp->len);
      }
    } else if(protocol == IPPROTO_ICMP) {
      fprintf(stderr, "***ICMPv4 UNSUPPORTED ***\n");
    } else if(protocol == IPPROTO_ICMPV6) {
      fprintf(stderr, "***ICMPv6 UNSUPPORTED ***\n");
    }
  } else if(net->eth->h_proto == ETH_P_ARP) {
    struct arphdr *arp = (struct arphdr*)(buffer + offset);
    net->arp = (struct arphdrs *)malloc(sizeof(struct arphdrs));
    if(!net->arp) {
      netutils_release_buffer(net);
      logger(LOG_ERR, "Unable to alloc memory for arp header!\n");
      return -1;
    }
    memset(net->arp, 0, sizeof(struct arphdrs));
    net->arp->arp1 = (struct arphdr *)malloc(sizeof(struct arphdr));
    if(!net->arp->arp1) {
      netutils_release_buffer(net);
      logger(LOG_ERR, "Unable to alloc memory for arp1 header!\n");
      return -1;
    }
    memcpy(net->arp->arp1, arp, sizeof(struct arphdr));
    offset += sizeof(struct arphdr);  
    if(convert == NETUTILS_CONVERT_NET2HOST) {
      net->arp->arp1->ar_op = ntohs(net->arp->arp1->ar_op);
      net->arp->arp1->ar_hrd = ntohs(net->arp->arp1->ar_hrd);
    } else if(convert == NETUTILS_CONVERT_HOST2NET) {
      net->arp->arp1->ar_op = htons(net->arp->arp1->ar_op);
      net->arp->arp1->ar_hrd = htons(net->arp->arp1->ar_hrd);
    }  
    /* part 2 */
    if((net->arp->arp1->ar_op == 1 || net->arp->arp1->ar_op == 2) && net->arp->arp1->ar_pln == 4) {
      struct arphdr2 *p2 = (struct arphdr2*)(buffer + offset);
      net->arp->arp2 = (struct arphdr2 *)malloc(sizeof(struct arphdr2));   
      if(!net->arp->arp2) {
	netutils_release_buffer(net);
	logger(LOG_ERR, "Unable to alloc memory for arp2 header!\n");
	return -1;
      }
      memcpy(net->arp->arp2, p2, sizeof(struct arphdr2));
      offset += sizeof(struct arphdr2);
    }
    if((length - offset)) {
      fprintf(stderr, "ARP Trailer: Not supported (%d bytes)\n", (length - offset));
      offset += (length - offset);
    }
  }
  return offset;
}

/**
 * @fn void netutils_release_buffer(struct netutils_headers_s *net)
 * @brief Liberation des ressources allouee par decode_network_buffer.
 * @param net Liste des entetes a liberer.
 */
void netutils_release_buffer(struct netutils_headers_s *net) {
  if(net->eth) free(net->eth), net->eth = NULL;
  if(net->arp) {
    if(net->arp->arp1) free(net->arp->arp1), net->arp->arp1 = NULL;
    if(net->arp->arp2) free(net->arp->arp2), net->arp->arp2 = NULL;
    free(net->arp), net->arp = NULL;
  }
  if(net->ipv4) free(net->ipv4), net->ipv4 = NULL;
  if(net->udp) free(net->udp), net->udp = NULL;
  if(net->tcp) free(net->tcp), net->tcp = NULL;
}

/**
 * @fn _Bool netutils_match_from_simple_filter(struct netutils_headers_s *net, struct netutils_filter_s filter)
 * @brief Test si le regle matche ou non.
 * @param net entetes.
 * @param filter Filtre a tester.
 * @return Retourne 1 si match.
 */
_Bool netutils_match_from_simple_filter(struct netutils_headers_s *net, struct netutils_filter_s filter) {
  _Bool ip_found = 0, port_found = 0;
  if(net->eth->h_proto == ETH_P_IP || net->eth->h_proto == ETH_P_IPV6) {
    if(net->ipv4) {
      if(filter.ip) {
	char src [INET_ADDRSTRLEN], dst [INET_ADDRSTRLEN];
	memset(dst, 0, sizeof(dst));
	memset(src, 0, sizeof(src));
	inet_ntop(AF_INET, &net->ipv4->saddr, src, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &net->ipv4->daddr, dst, INET_ADDRSTRLEN);
	if(filter.ip == netutils_ip_to_long(src))
	  ip_found = 1;
	else if(filter.ip == netutils_ip_to_long(dst))
	  ip_found = 1;
	if(!ip_found) return 0;
      } else ip_found = 1;

      if(net->tcp) {
	if(filter.port > 0) {
	  if(filter.port == net->tcp->source) port_found = 1;
	  else if(filter.port == net->tcp->dest) port_found = 1;
	} else port_found = 1;
      } else if(net->udp) {
	if(filter.port > 0) {
	  if(filter.port == net->udp->source) port_found = 1;
	  else if(filter.port == net->udp->dest) port_found = 1;
	} else port_found = 1;
      }
    }
  } else if(net->eth->h_proto == ETH_P_ARP) {
    if(net->arp && net->arp->arp2) {
      if(filter.ip) {
	char ip[17];
	bzero(ip, 17);
	sprintf(ip, "%03d.%03d.%03d.%03d", net->arp->arp2->sip[0], net->arp->arp2->sip[1], net->arp->arp2->sip[2], net->arp->arp2->sip[3]);
	
	if(filter.ip == netutils_ip_to_long(ip))
	  port_found = ip_found = 1;
	else {
	  bzero(ip, 17);
	  sprintf(ip, "%03d.%03d.%03d.%03d", net->arp->arp2->tip[0], net->arp->arp2->tip[1], net->arp->arp2->tip[2], net->arp->arp2->tip[3]);	
	  if(filter.ip == netutils_ip_to_long(ip))
	    port_found = ip_found = 1;
	}
      } else port_found = ip_found = 1;
    }
  }
  return ip_found && port_found;
}

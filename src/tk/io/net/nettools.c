/**
*******************************************************************************
* @file nettools.c
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
#include <regex.h>
#include <tk/io/net/nettools.h>
#include <tk/utils/string.h>
#include <tk/sys/log.h>
#include <netdb.h>
#include <netinet/in.h>


static struct nettools_subnet_s subnet_table [] = {
  {1, 31, "128.0.0.0"},
  {2, 30, "192.0.0.0"},
  {3, 29, "224.0.0.0"},
  {4, 28, "240.0.0.0"},
  {5, 27, "248.0.0.0"},
  {6, 26, "252.0.0.0"},
  {7, 25, "254.0.0.0"},
  {8, 24, "255.0.0.0"},
  {9, 23, "255.128.0.0"},
  {10, 22, "255.192.0.0"},
  {11, 21, "255.224.0.0"},
  {12, 20, "255.240.0.0"},
  {13, 19, "255.248.0.0"},
  {14, 18, "255.252.0.0"},
  {15, 17, "255.254.0.0"},
  {16, 16, "255.255.0.0"},
  {17, 15, "255.255.128.0"},
  {18, 14, "255.255.192.0"},
  {19, 13, "255.255.224.0"},
  {20, 12, "255.255.240.0"},
  {21, 11, "255.255.248.0"},
  {22, 10, "255.255.252.0"},
  {23, 9, "255.255.254.0"},
  {24, 8, "255.255.255.0"},
  {25, 7, "255.255.255.128"},
  {26, 6, "255.255.255.192"},
  {27, 5, "255.255.255.224"},
  {28, 4, "255.255.255.240"},
  {29, 3, "255.255.255.248"},
  {30, 2, "255.255.255.252"},
  {31, 1, "255.255.255.254"},
  {32, 0, "255.255.255.255"}
};



/**
 * @fn int nettools_prepare_ifaces(htable_t *ifaces, int *maxfd, fd_set *rset, const netiface_name_t iname)
 * @brief List all network interfaces, configures and adds into the list (CAUTION: after the call of this function a socket is opened).
 * @param ifaces Interfaces list (the list key == fd).
 * @param maxfd Used by select function.
 * @param rset fd_set Used by select function.
 * @param iname The interface name.
 * @return -1 on error else 0.
 */
int nettools_prepare_ifaces(htable_t *ifaces, int *maxfd, fd_set *rset, const netiface_name_t iname) {
  
  char** keys;
  int i, count, fd;
  netiface_t iface;
  *ifaces = netiface_list_new(NETIFACE_LVL_RAW, NETIFACE_KEY_FD);
  count = htable_get_keys(*ifaces, &keys);
  for(i = 0; i < count; i++) {
    iface = htable_lookup(*ifaces, keys[i]);
    if(netiface_get_fd(iface, &fd) == -1) return -1;
    if(fd > *maxfd) *maxfd = fd;
    FD_SET(fd, rset);
    if(netiface_bind(iface) == -1) return -1;
  }
  return 0;
}

/**
 * @fn _Bool nettools_device_is_up(int fd, char name[IF_NAMESIZE])
 * @brief Test if the current device is up.
 * @param fd Device FD.
 * @param name Device name.
 * @return 1 if up else 0..
 */
_Bool nettools_device_is_up(int fd, char name[IF_NAMESIZE]) {
  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  strncpy((char *)ifr.ifr_name, name, IF_NAMESIZE);
  int ret = ioctl(fd, SIOCGIFFLAGS, &ifr);
  if (ret == -1) {
    logger(LOG_ERR, "flags: (%d) %s.\n", errno, strerror(errno));
    return ret;
  }
  return !!(ifr.ifr_flags & IFF_UP);
}

/**
 * @fn __u32 nettools_datas_available(int fd)
 * @brief Get the number of available datas to be read.
 * @param fd Socket FD.
 * @return Available datas.
 */
__u32 nettools_datas_available(int fd) {
  __u32 available = 0;
  int ret = ioctl(fd, FIONREAD, &available);
  if (ret == -1) {
    logger(LOG_ERR, "available: (%d) %s.\n", errno, strerror(errno));
    return ret; /* ... */
  }
  return available;
}

/**
 * @fn int nettools_is_ipv4(const char* ip)
 * @brief Test if the input string is an ipv4.
 * @param ip IP address.
 * @return -1 on error, 0 not match, 1 match.
 */
int nettools_is_ipv4(const char* ip) {    
  struct in_addr i_addr;
  int ret = 0;
  ret = inet_pton(AF_INET, ip, &i_addr);
  if(ret == 1) return 1;
  else if(ret == 0) return 0;
  return -1;
}
 
/**
 * @fn int nettools_hostname_to_ip(const char *hostname, char* ip)
 * @brief Convert a hostname to an ip.
 * @param hostname Name of the host.
 * @param ip IP address.
 * @return -1 on error else 0.
 */
int nettools_hostname_to_ip(const char *hostname, char* ip) {
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
 * @fn void nettools_print_hex(FILE* std, net_buffer_t buffer, int len, _Bool print_raw)
 * @brief Print the packet in hexa (wireshark like).
 * @param std Output stream.
 * @param buffer Packet.
 * @param len Packet length.
 * @param print_raw Display in raw mode.
 */
void nettools_print_hex(FILE* std, net_buffer_t buffer, int len, _Bool print_raw) {
  int i = 0, max = PRINT_HEX_MAX_PER_LINES, loop = len;
  __u8 *p = buffer;
  char line [max + 3]; /* spaces + \0 */
  memset(line, 0, sizeof(line));
  while(loop--) {
    __u8 c = *(p++);
    if(!print_raw) {
      fprintf(std, "%02x ", c);
      /* only the visibles char */
      if(c >= 0x20 && c <= 0x7e) line[i] = c;
      /* else mask with '.' */
      else line[i] = 0x2e; /* . */
    } else fprintf(std, "%02x", c);
    /* next line */
    if(i == max) {
      if(!print_raw)
	fprintf(std, "  %s\n", line);
      else fprintf(std, "\n");
      /* re init */
      i = 0;
      memset(line, 0, sizeof(line));
    }
    /* next */
    else i++;
    /* add a space in the midline */
    if(i == max / 2 && !print_raw) {
      fprintf(std, " ");
      line[i++] = 0x20;
    }
  }
  /* align 'line'*/
  if(i != 0 && (i < max || i <= len) && !print_raw) {
    while(i++ <= max) fprintf(std, "   "); /* 3 spaces ex: "00 " */
    fprintf(std, "%s\n", line);
  }
  fprintf(std, "\n");
}

/**
 * @fn const char* nettools_long_to_ip(unsigned int v)
 * @brief Convert a long value to an IP address.
 * @param v Long value.
 * @return IP addr.
 */
const char* nettools_long_to_ip(unsigned int v)  {
  struct in_addr x;
  x.s_addr = htonl(v);
  return inet_ntoa(x);
}

/**
 * @fn unsigned int nettools_ip_to_long(const char* s)
 * @brief Convert an IP address to a long value.
 * @param s IP address
 * @return Long value.
 */
unsigned int nettools_ip_to_long(const char* s) {
  struct sockaddr_in n;
  inet_aton(s,&n.sin_addr);
  return ntohl(n.sin_addr.s_addr);
}

/**
 * @fn int nettools_ip_to_sockaddr(const char* ip, struct sockaddr_in *sin)
 * @brief Convert an ascii ip (or host name) to a sockaddr_in.
 * @param ip The ip (or hostname) to convert.
 * @param sin The output.
 * @return -1 on error else 0 on success.
 */
int nettools_ip_to_sockaddr(const char* ip, struct sockaddr_in *sin) {
  struct hostent *hp;
  if(!sin) return -1;
  sin->sin_family = AF_INET;
  sin->sin_addr.s_addr = inet_addr((char*)ip);
  if(sin->sin_addr.s_addr == -1){
    if(!(hp = gethostbyname(ip))){
      logger(LOG_ERR, "gethostbyname failed: (%d) %s\n", h_errno, hstrerror(h_errno));
      return -1;
    }
    bcopy((char *)hp->h_addr, (char *)&sin->sin_addr, sizeof(sin->sin_addr));
    logger(LOG_DEBUG, "Host name '%s' resolved '%s'\n", ip, inet_ntoa(sin->sin_addr));
  }
  return 0;
}

/**
 * @fn pcap_hdr_t nettools_pcap_global_hdr(void)
 * @brief Build the main header of the pcap file.
 * @param link Data link type.
 * @return pcap_hdr_t
 */
pcap_hdr_t nettools_pcap_global_hdr(__u32 link) {
  pcap_hdr_t hdr;
  memset(&hdr, 0, sizeof(pcap_hdr_t));
  hdr.magic_number = NETTOOLS_PCAP_MAGIC_NATIVE;
  hdr.version_major = NETTOOLS_PCAP_VERSION_MAJOR;
  hdr.version_minor = NETTOOLS_PCAP_VERSION_MINOR;  
  tzset(); /* reload the timezone field */
  hdr.thiszone = timezone;
  hdr.sigfigs = 0;
  hdr.snaplen = NETTOOLS_PCAP_SNAPLEN;
  hdr.network = link;
  return hdr;
}
/**
 * @fn pcap_hdr_t nettools_pcap_packet_hdr(__u32 incl_len, __u32 ori_len)
 * @brief Build the packet header of the pcap file.
 * @return pcaprec_hdr_t.
 */
pcaprec_hdr_t nettools_pcap_packet_hdr(__u32 incl_len, __u32 ori_len) {
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
 * @fn void nettools_write_pcap_packet(const FILE* output, const net_buffer_t buffer, size_t a_length, size_t r_length, _Bool *first)
 * @brief Writes all pcap headers and the packet buffer into the specified file.
 * Source: http://wiki.wireshark.org/Development/LibpcapFileFormat
 * Packet structure:
 * -----------------------------------------------------------------------------------------------------------------
 * | Global Header | Packet Header | Packet Data | Packet Header | Packet Data | Packet Header | Packet Data | ... |
 * -----------------------------------------------------------------------------------------------------------------
 * @param output Output file
 * @param link Data link type.
 * @param buffer Input buffer .
 * @param a_length Size before the call of the recvfrom function.
 * @param r_length Size after the call of the recvfrom function.
 * @param first Memorize if we need to write the first packet header.
 */
void nettools_write_pcap_packet(FILE* output, __u32 link, const net_buffer_t buffer, size_t a_length, size_t r_length, _Bool *first) {
  if(*first) {
    pcap_hdr_t ghdr = nettools_pcap_global_hdr(link);
    fwrite(&ghdr, 1, sizeof(pcap_hdr_t), output);
    *first = 0;
  }
  pcaprec_hdr_t phdr = nettools_pcap_packet_hdr(r_length, a_length);
  fwrite(&phdr, 1, sizeof(pcaprec_hdr_t), output);
  fwrite(buffer, 1, r_length, output);
  fflush(output);
}

/**
 * @fn int nettools_decode_buffer(const net_buffer_t buffer, __u32 length, struct nettools_headers_s *net, bns_packet_convert_et convert)
 * @brief Decode the packets in terms of the input buffer.
 * @param buffer The buffer datas.
 * @param length The buffer length.
 * @param net Headers list.
 * @param convert Convert required fields.
 * @return -1 on error else the payload length (can be equals to 0).
 */
int nettools_decode_buffer(const net_buffer_t buffer, __u32 length, struct nettools_headers_s *net, nettools_convert_et convert) {
  __u32 offset = sizeof(struct ethhdr);
  memset(net, 0, sizeof(struct nettools_headers_s));
  struct ethhdr *eth = (struct ethhdr *)buffer;
  net->eth = (struct ethhdr *)malloc(sizeof(struct ethhdr));
  if(!net->eth) {
    logger(LOG_ERR, "Unable to alloc memory for eth header!\n");
    return -1;
  }
  memcpy(net->eth, eth, sizeof(struct ethhdr));
  if(convert == NETTOOLS_CONVERT_NET2HOST)
    net->eth->h_proto = ntohs(net->eth->h_proto);
  else if(convert == NETTOOLS_CONVERT_HOST2NET)
    net->eth->h_proto = htons(net->eth->h_proto);
  if(net->eth->h_proto == ETH_P_IP || net->eth->h_proto == ETH_P_IPV6) {
    struct iphdr *ip4 = (struct iphdr*)(buffer + offset);

    __u8 protocol = 0;
    if(ip4->version == 4) {
      net->ipv4 = (struct iphdr *)malloc(sizeof(struct iphdr));
      if(!net->ipv4) {
	nettools_release_buffer(net);
	logger(LOG_ERR, "Unable to alloc memory for ipv4 header!\n");
	return -1;
      }
      memcpy(net->ipv4, ip4, sizeof(struct iphdr));
      offset += sizeof(struct iphdr);
      if(convert == NETTOOLS_CONVERT_NET2HOST) {
        net->ipv4->tot_len = ntohs(ip4->tot_len);
        net->ipv4->tos = ntohs(ip4->tos); /* pas certains */
        //net->ipv4->ihl = /*ntohl(*/ip4->ihl/*)*/;
        net->ipv4->frag_off = ntohs(ip4->frag_off);
      } else if(convert == NETTOOLS_CONVERT_HOST2NET) {
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
	nettools_release_buffer(net);
	logger(LOG_ERR, "Unable to alloc memory for tcp header!\n");
	return -1;
      }
      memcpy(net->tcp, tcp, sizeof(struct tcphdr));
      offset += sizeof(union tcp_word_hdr);
      if(convert == NETTOOLS_CONVERT_NET2HOST) {
        net->tcp->source = ntohs(net->tcp->source);
        net->tcp->dest = ntohs(net->tcp->dest);
        net->tcp->seq = ntohs(net->tcp->seq);
        net->tcp->ack_seq = ntohs(net->tcp->ack_seq);
        net->tcp->check = ntohs(net->tcp->check);
      } else if(convert == NETTOOLS_CONVERT_HOST2NET) {
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
	nettools_release_buffer(net);
	logger(LOG_ERR, "Unable to alloc memory for udp header!\n");
	return -1;
      }
      memcpy(net->udp, udp, sizeof(struct udphdr));
      offset += sizeof(struct udphdr);
      if(convert == NETTOOLS_CONVERT_NET2HOST) {
        net->udp->source = ntohs(net->udp->source);
        net->udp->dest = ntohs(net->udp->dest);
        net->udp->check = ntohs(net->udp->check);
        net->udp->len = ntohs(net->udp->len);
      } else if(convert == NETTOOLS_CONVERT_HOST2NET) {
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
      nettools_release_buffer(net);
      logger(LOG_ERR, "Unable to alloc memory for arp header!\n");
      return -1;
    }
    memset(net->arp, 0, sizeof(struct arphdrs));
    net->arp->arp1 = (struct arphdr *)malloc(sizeof(struct arphdr));
    if(!net->arp->arp1) {
      nettools_release_buffer(net);
      logger(LOG_ERR, "Unable to alloc memory for arp1 header!\n");
      return -1;
    }
    memcpy(net->arp->arp1, arp, sizeof(struct arphdr));
    offset += sizeof(struct arphdr);  
    if(convert == NETTOOLS_CONVERT_NET2HOST) {
      net->arp->arp1->ar_op = ntohs(net->arp->arp1->ar_op);
      net->arp->arp1->ar_hrd = ntohs(net->arp->arp1->ar_hrd);
    } else if(convert == NETTOOLS_CONVERT_HOST2NET) {
      net->arp->arp1->ar_op = htons(net->arp->arp1->ar_op);
      net->arp->arp1->ar_hrd = htons(net->arp->arp1->ar_hrd);
    }  
    /* part 2 */
    if((net->arp->arp1->ar_op == 1 || net->arp->arp1->ar_op == 2) && net->arp->arp1->ar_pln == 4) {
      struct arphdr2 *p2 = (struct arphdr2*)(buffer + offset);
      net->arp->arp2 = (struct arphdr2 *)malloc(sizeof(struct arphdr2));   
      if(!net->arp->arp2) {
	nettools_release_buffer(net);
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
 * @fn void nettools_release_buffer(struct nettools_headers_s *net)
 * @brief Release the resources allocated by the decoder function.
 * @param net The headers pointer.
 */
void nettools_release_buffer(struct nettools_headers_s *net) {
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
 * @fn _Bool nettools_valid_mac(netiface_mac_t mac)
 * @brief Test if the MAC is valid.
 * @param mac MAC address to test.
 * @return 1 if valid else 0.
 */
_Bool nettools_valid_mac(netiface_mac_t mac) {
  const char *str_regex = "(([0-9A-Fa-f]{2}[-:]){5}[0-9A-Fa-f]{2})|(([0-9A-Fa-f]{4}.){2}[0-9A-Fa-f]{4})";
  return string_match(mac, str_regex);
}

/**
 * @fn void nettools_mac2str(netiface_bmac_t mac, netiface_mac_t m)
 * @brief Convert a MAC array into a string.
 * @param mac MAC to convert.
 * @param m MAC in string.
 */
void nettools_mac2str(netiface_bmac_t mac, netiface_mac_t m) {
  sprintf(m, "%02x:%02x:%02x:%02x:%02x:%02x", 
	  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

/**
 * @fn void nettools_str2mac(netiface_mac_t mac, netiface_bmac_t m)
 * @brief Convert a MAC string into a MAC array.
 * @param mac MAC to convert
 * @param m MAC in array.
 */
void nettools_str2mac(netiface_mac_t mac, netiface_bmac_t m) {
  sscanf(mac, "%x:%x:%x:%x:%x:%x", 
	 (__u32*)&m[0], (__u32*)&m[1], (__u32*)&m[2], (__u32*)&m[3], (__u32*)&m[4], (__u32*)&m[5]);
}


/**
 * @fn _Bool nettools_match_from_simple_filter(struct nettools_header_s *net, struct nettools_filter_s filter)
 * @brief Check if the input rule match.
 * @param net Headers.
 * @param filter Filter to test.
 * @return 1 if the rule match.
 */
_Bool nettools_match_from_simple_filter(struct nettools_headers_s *net, struct nettools_filter_s filter) {
  _Bool ip_found = 0, port_found = 0, mac_found = 0;
  if(nettools_valid_mac(filter.mac)) {
    netiface_bmac_t m;
    nettools_str2mac(filter.mac, m);
    if(memcmp(net->eth->h_source, m, ETH_ALEN) == 0)
      mac_found = 1;
    else if(memcmp(net->eth->h_dest, m, ETH_ALEN) == 0)
      mac_found = 1;
    if(!mac_found) return 0;
  } else mac_found = 1;
  if(net->eth->h_proto == ETH_P_IP || net->eth->h_proto == ETH_P_IPV6) {
    if(net->ipv4) {
      if(filter.ip) {
	char src [INET_ADDRSTRLEN], dst [INET_ADDRSTRLEN];
	memset(dst, 0, sizeof(dst));
	memset(src, 0, sizeof(src));
	inet_ntop(AF_INET, &net->ipv4->saddr, src, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &net->ipv4->daddr, dst, INET_ADDRSTRLEN);
	if(filter.ip == nettools_ip_to_long(src))
	  ip_found = 1;
	else if(filter.ip == nettools_ip_to_long(dst))
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
	
	if(filter.ip == nettools_ip_to_long(ip))
	  port_found = ip_found = 1;
	else {
	  bzero(ip, 17);
	  sprintf(ip, "%03d.%03d.%03d.%03d", net->arp->arp2->tip[0], net->arp->arp2->tip[1], net->arp->arp2->tip[2], net->arp->arp2->tip[3]);	
	  if(filter.ip == nettools_ip_to_long(ip))
	    port_found = ip_found = 1;
	}
      } else port_found = ip_found = 1;
    }
  }
  return mac_found && ip_found && port_found;
}


/**
 * @fn int nettools_recvfrom_timeout(int fd, long sec, long usec)
 * @brief Wait for input datas.
 * @param fd The RAW socket FD.
 * @param sec The seconds nb before timeout.
 * @return -1 on error, 0 on timeout else >=1
 */
int nettools_recvfrom_timeout(int fd, long sec, long usec) {
  // Setup timeval variable
  struct timeval timeout;
  timeout.tv_sec = sec;
  timeout.tv_usec = usec;
  // Setup fd_set structure
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  return select(fd+1, &fds, 0, 0, &timeout);
}

/**
 * @fn __u8 nettools_get_cidr(netiface_ip4_t ip)
 * @brief Get the CIDR value from an IP.
 * @param ip The IP.
 * @return The CIDR (0 min max 32)
 */
__u8 nettools_get_cidr(netiface_ip4_t ip) {
  unsigned char bits = 0;
  in_addr_t mask;
  in_addr_t host;
  if(!ip || !strlen(ip)) return 32;

  mask = inet_network(ip);
  host = ~mask;
  /* a valid netmask must be 2^n - 1 */
  if((host & (host + 1)) != 0)
    return -1;
  for(; mask; mask <<= 1) ++bits;
  return bits;
}

/**
 * @fn const char* nettools_get_mask_by_cidr(__u8 cidr)
 * @brief Get the associated mask from cidr.
 * @param cid The cidr.
 * @return The mask
 */
const char* nettools_get_mask_by_cidr(__u8 cidr) {
  if(cidr == 0) cidr = 1;
  if(cidr > 32) cidr = 32;
  return subnet_table[cidr - 1].smask;
}

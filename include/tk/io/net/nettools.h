/**
 *******************************************************************************
 * @file nettools.h
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
#ifndef __NETTOOLS_H__
  #define __NETTOOLS_H__

  #include <tk/io/net/netiface.h>
  #include <asm/types.h>
  #include <net/if.h>
  #include <linux/if_ether.h>
  #include <netinet/ip.h>
  #include <linux/udp.h>
  #include <linux/tcp.h>
  #include <net/if_arp.h>
  #include <arpa/inet.h>
  #include <sys/select.h>

  /* nombre max de chars dans une ligne */
  #define PRINT_HEX_MAX_PER_LINES 16

  struct nettools_filter_s {
    __u32 ip;
    __u32 port;
    netiface_name_t iface;
    netiface_mac_t mac;
  };

/* utilisee pour le decodage de la reponse/requete ARP */
  struct arphdr2 {
    unsigned char sha[ETH_ALEN];
    unsigned char sip[4];
    unsigned char tha[ETH_ALEN];
    unsigned char tip[4];
  };

  struct arphdrs{
      struct arphdr *arp1;
      struct arphdr2 *arp2;
  };

  struct nettools_headers_s {
      struct ethhdr *eth;
      struct arphdrs *arp;
      struct iphdr  *ipv4;
      struct udphdr *udp;
      struct tcphdr *tcp;
  };

  /**
   * @typedef pcap_hdr_t
   * @brief Global header
   * Source: http://wiki.wireshark.org/Development/LibpcapFileFormat
   */
  typedef struct pcap_hdr_s {
      __u32 magic_number;  /**< magic number */
      __u16 version_major; /**< major version number */
      __u16 version_minor; /**< minor version number */
      __s32 thiszone;      /**< GMT to local correction */
      __u32 sigfigs;       /**< accuracy of timestamps */
      __u32 snaplen;       /**< max length of captured packets, in octets */
      __u32 network;       /**< data link type */
  } pcap_hdr_t;


  /**
   * @typedef pcaprec_hdr_t
   * @brief Packet header
   * Source: http://wiki.wireshark.org/Development/LibpcapFileFormat
   */
  typedef struct pcaprec_hdr_s {
      __u32 ts_sec;        /**< timestamp seconds */
      __u32 ts_usec;       /**< timestamp microseconds */
      __u32 incl_len;      /**< number of octets of packet saved in file */
      __u32 orig_len;      /**< actual length of packet */
  } pcaprec_hdr_t;

  /**
   * @def NETTOOLS_PCAP_VERSION_MAJOR
   * @brief Version major du fichier pcap.
   * @see nettools_utils_pcap_global_hdr
   */
  #define NETTOOLS_PCAP_VERSION_MAJOR     2
  /**
   * @def NETTOOLS_PCAP_VERSION_MINOR
   * @brief Version minor du fichier pcap.
   * @see nettools_utils_pcap_global_hdr
   */
  #define NETTOOLS_PCAP_VERSION_MINOR     4
  /**
   * @def NETTOOLS_PCAP_MAGIC_NATIVE
   * @brief Magic du fichier pcap.
   * @see nettools_utils_pcap_global_hdr
   */
  #define NETTOOLS_PCAP_MAGIC_NATIVE      0xa1b2c3d4
  /**
   * @def NETTOOLS_PCAP_MAGIC_SWAPPED
   * @brief Magic du fichier pcap.
   * @see nettools_utils_pcap_global_hdr
   */
  #define NETTOOLS_PCAP_MAGIC_SWAPPED      0xd4c3b2a1
  /**
   * @def NETTOOLS_PCAP_LINKTYPE_ETHERNET
   * @brief Type de capture.
   * @see nettools_utils_pcap_global_hdr
   */
  #define NETTOOLS_PCAP_LINKTYPE_ETHERNET 1
  /**
   * @def NETTOOLS_PCAP_SNAPLEN
   * @brief Taille de la capture.
   * @see nettools_utils_pcap_global_hdr
   */
  #define NETTOOLS_PCAP_SNAPLEN           65535

  #define nettools_pcap_magic_str(magic) (magic == NETTOOLS_PCAP_MAGIC_NATIVE ? "NATIVE" : ( magic == NETTOOLS_PCAP_MAGIC_SWAPPED ? "SWAPPED" : "UNKNOWN"))

  typedef enum { NETTOOLS_CONVERT_NONE, NETTOOLS_CONVERT_HOST2NET, NETTOOLS_CONVERT_NET2HOST} nettools_convert_et;

  typedef __u8* net_buffer_t;

  struct net_payload_s {
      __u32 length;
      net_buffer_t buffer;
  };

 /**
   * @fn int nettools_decode_buffer(const net_buffer_t buffer, __u32 length, struct nettools_headers_s *net, bns_packet_convert_et convert)
   * @brief Decode the packets in terms of the input buffer.
   * @param buffer The buffer datas.
   * @param length The buffer length.
   * @param net Headers list.
   * @param convert Convert required fields.
   * @return -1 on error else the payload length (can be equals to 0).
   */
  int nettools_decode_buffer(const net_buffer_t buffer, __u32 length, struct nettools_headers_s *net, nettools_convert_et convert);

  /**
   * @fn void nettools_release_buffer(struct nettools_headers_s *net)
   * @brief Release the resources allocated by the decoder function.
   * @param net The headers pointer.
   */
  void nettools_release_buffer(struct nettools_headers_s *net);

  /**
   * @fn _Bool nettools_match_from_simple_filter(struct nettools_header_s *net, struct nettools_filter_s filter)
   * @brief Check if the input rule match.
   * @param net Headers.
   * @param filter Filter to test.
   * @return 1 if the rule match.
   */
  _Bool nettools_match_from_simple_filter(struct nettools_headers_s *net, struct nettools_filter_s filter);

  /**
   * @fn int nettools_is_ipv4(const char* ip)
   * @brief Test if the input string is an ipv4.
   * @param ip IP address.
   * @return -1 on error, 0 not match, 1 match.
   */
  int nettools_is_ipv4(const char* ip);

  /**
   * @fn int nettools_hostname_to_ip(const char *hostname, char* ip)
   * @brief Convert a hostname to an ip.
   * @param hostname Name of the host.
   * @param ip IP address.
   * @return -1 on error else 0.
   */
  int nettools_hostname_to_ip(const char *hostname, char* ip);

  /**
   * @fn _Bool nettools_device_is_up(int fd, char name[IF_NAMESIZE])
   * @brief Test if the current device is up.
   * @param fd Device FD.
   * @param name Device name.
   * @return 1 if up else 0..
   */
  _Bool nettools_device_is_up(int fd, char name[IF_NAMESIZE]);

  /**
   * @fn __u32 nettools_datas_available(int fd)
   * @brief Get the number of available datas to be read.
   * @param fd Socket FD.
   * @return Available datas.
   */
  __u32 nettools_datas_available(int fd);

  /**
   * @fn void nettools_print_hex(FILE* std, net_buffer_t buffer, int len, _Bool print_raw)
   * @brief Print the packet in hexa (wireshark like).
   * @param std Output stream.
   * @param buffer Packet.
   * @param len Packet length.
   * @param print_raw Display in raw mode.
   */
  void nettools_print_hex(FILE* std, net_buffer_t buffer, int len, _Bool print_raw);

  /**
   * @fn int nettools_prepare_ifaces(htable_t *ifaces, int *maxfd, fd_set *rset, const netiface_name_t iname)
   * @brief List all network interfaces, configures and adds into the list (CAUTION: after the call of this function a socket is opened).
   * @param ifaces Interfaces list (the list key == fd).
   * @param maxfd Used by select function.
   * @param rset fd_set Used by select function.
   * @param iname The interface name.
   * @return -1 on error else 0.
   */
  int nettools_prepare_ifaces(htable_t *ifaces, int *maxfd, fd_set *rset, const netiface_name_t iname);

  /**
   * @fn const char* nettools_long_to_ip(unsigned int v)
   * @brief Convert a long value to an IP address.
   * @param v Long value.
   * @return IP addr.
   */
  const char* nettools_long_to_ip(unsigned int v);

  /**
   * @fn unsigned int nettools_ip_to_long(const char* s)
   * @brief Convert an IP address to a long value.
   * @param s IP address
   * @return Long value.
   */
  unsigned int nettools_ip_to_long(const char* s);

  /**
   * @fn int nettools_ip_to_sockaddr(const char* ip, struct sockaddr_in *sin)
   * @brief Convert an ascii ip (or host name) to a sockaddr_in.
   * @param ip The ip (or hostname) to convert.
   * @param sin The output.
   * @return -1 on error else 0 on success.
   */
  int nettools_ip_to_sockaddr(const char* ip, struct sockaddr_in *sin);

  /**
   * @fn pcap_hdr_t nettools_pcap_global_hdr(void)
   * @brief Build the main header of the pcap file.
   * @param link Data link type.
   * @return pcap_hdr_t
   */
  pcap_hdr_t nettools_pcap_global_hdr(__u32 link);

  /**
   * @fn pcap_hdr_t nettools_pcap_packet_hdr(__u32 incl_len, __u32 ori_len)
   * @brief Build the packet header of the pcap file.
   * @return pcaprec_hdr_t.
   */
  pcaprec_hdr_t nettools_pcap_packet_hdr(__u32 incl_len, __u32 ori_len);

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
  void nettools_write_pcap_packet(FILE* output, __u32 link, const net_buffer_t buffer, size_t a_length, size_t r_length, _Bool *first);

  /**
   * @fn void nettools_mac2str(netiface_bmac_t mac, netiface_mac_t m)
   * @brief Convert a MAC array into a string.
   * @param mac MAC to convert.
   * @param m MAC in string.
   */
  void nettools_mac2str(netiface_bmac_t mac, netiface_mac_t m);

  /**
   * @fn void nettools_str2mac(netiface_mac_t mac, netiface_bmac_t m)
   * @brief Convert a MAC string into a MAC array.
   * @param mac MAC to convert
   * @param m MAC in array.
   */
  void nettools_str2mac(netiface_mac_t mac, netiface_bmac_t m);

  /**
   * @fn _Bool nettools_valid_mac(netiface_mac_t mac)
   * @brief Test if the MAC is valid.
   * @param mac MAC address to test.
   * @return 1 if valid else 0.
   */
  _Bool nettools_valid_mac(netiface_mac_t mac);


  /**
   * @fn int nettools_recvfrom_timeout(int fd, long sec, long usec)
   * @brief Wait for input datas.
   * @param fd The RAW socket FD.
   * @param sec The seconds nb before timeout.
   * @return -1 on error, 0 on timeout else >=1
   */
  int nettools_recvfrom_timeout(int fd, long sec, long usec);


  /**
   * @fn __u8 nettools_get_cidr(netiface_ip4_t ip)
   * @brief Get the CIDR value from an IP.
   * @param ip The IP.
   * @return The CIDR (0 min max 32)
   */
  __u8 nettools_get_cidr(netiface_ip4_t ip);

#endif /* __NETTOOLS_H__ */

/**
 *******************************************************************************
 * @file ntools.h
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
#ifndef __NTOOLS_H__
  #define __NTOOLS_H__

  #include <asm/types.h>
  #include <net/if.h>
  #include <linux/if_ether.h>
  #include <netinet/ip.h>
  #include <linux/udp.h>
  #include <linux/tcp.h>
  #include <net/if_arp.h>
  #include <arpa/inet.h>
  #include <sys/select.h>
  #include <linux/list.h>

  /* nombre max de chars dans une ligne */
  #define PRINT_HEX_MAX_PER_LINES 16

  #define NTOOLS_SMAC_LEN 18

  typedef char smac_t[NTOOLS_SMAC_LEN];
  typedef __u8 mac_t[ETH_ALEN];

  struct ntools_filter_s {
    __u32 ip;
    __u32 port;
    char iface[IF_NAMESIZE];
    smac_t mac;
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

  struct ntools_headers_s {
      struct ethhdr *eth;
      struct arphdrs *arp;
      struct iphdr  *ipv4;
      struct udphdr *udp;
      struct tcphdr *tcp;
  };

  struct iface_s {
    struct list_head list;              /**< Liste d'interfaces. */
    char             name[IF_NAMESIZE]; /**< Nom de l'interface. */
    int              index;             /**< Index de la carte. */
    int              fd;                /**< FD du socket utilise pour les io's/bind/select. */
    int              family;            /**< Famille de l'interface. */
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
   * @def NTOOLS_PCAP_VERSION_MAJOR
   * @brief Version major du fichier pcap.
   * @see ntools_utils_pcap_global_hdr
   */
  #define NTOOLS_PCAP_VERSION_MAJOR     2
  /**
   * @def NTOOLS_PCAP_VERSION_MINOR
   * @brief Version minor du fichier pcap.
   * @see ntools_utils_pcap_global_hdr
   */
  #define NTOOLS_PCAP_VERSION_MINOR     4
  /**
   * @def NTOOLS_PCAP_MAGIC_NATIVE
   * @brief Magic du fichier pcap.
   * @see ntools_utils_pcap_global_hdr
   */
  #define NTOOLS_PCAP_MAGIC_NATIVE      0xa1b2c3d4
  /**
   * @def NTOOLS_PCAP_MAGIC_SWAPPED
   * @brief Magic du fichier pcap.
   * @see ntools_utils_pcap_global_hdr
   */
  #define NTOOLS_PCAP_MAGIC_SWAPPED      0xd4c3b2a1
  /**
   * @def NTOOLS_PCAP_LINKTYPE_ETHERNET
   * @brief Type de capture.
   * @see ntools_utils_pcap_global_hdr
   */
  #define NTOOLS_PCAP_LINKTYPE_ETHERNET 1
  /**
   * @def NTOOLS_PCAP_SNAPLEN
   * @brief Taille de la capture.
   * @see ntools_utils_pcap_global_hdr
   */
  #define NTOOLS_PCAP_SNAPLEN           65535

  #define ntools_pcap_magic_str(magic) (magic == NTOOLS_PCAP_MAGIC_NATIVE ? "NATIVE" : ( magic == NTOOLS_PCAP_MAGIC_SWAPPED ? "SWAPPED" : "UNKNOWN"))

  typedef enum { NTOOLS_CONVERT_NONE, NTOOLS_CONVERT_HOST2NET, NTOOLS_CONVERT_NET2HOST} ntools_convert_et;

  typedef __u8* net_buffer_t;

 /**
   * @fn int ntools_decode_buffer(const net_buffer_t buffer, __u32 length, struct ntools_headers_s *net, bns_packet_convert_et convert)
   * @brief Decode the packets in terms of the input buffer.
   * @param buffer The buffer datas.
   * @param length The buffer length.
   * @param net Headers list.
   * @param convert Convert required fields.
   * @return -1 on error else the payload length (can be equals to 0).
   */
  int ntools_decode_buffer(const net_buffer_t buffer, __u32 length, struct ntools_headers_s *net, ntools_convert_et convert);

  /**
   * @fn void ntools_release_buffer(struct ntools_headers_s *net)
   * @brief Release the resources allocated by the decoder function.
   * @param net The headers pointer.
   */
  void ntools_release_buffer(struct ntools_headers_s *net);

  /**
   * @fn _Bool ntools_match_from_simple_filter(struct ntools_header_s *net, struct ntools_filter_s filter)
   * @brief Check if the input rule match.
   * @param net Headers.
   * @param filter Filter to test.
   * @return 1 if the rule match.
   */
  _Bool ntools_match_from_simple_filter(struct ntools_headers_s *net, struct ntools_filter_s filter);

  /**
   * @fn int ntools_is_ipv4(const char* ip)
   * @brief Test if the input string is an ipv4.
   * @param ip IP address.
   * @return -1 on error, 0 not match, 1 match.
   */
  int ntools_is_ipv4(const char* ip);

  /**
   * @fn int ntools_hostname_to_ip(const char *hostname, char* ip)
   * @brief Convert a hostname to an ip.
   * @param hostname Name of the host.
   * @param ip IP address.
   * @return -1 on error else 0.
   */
  int ntools_hostname_to_ip(const char *hostname, char* ip);

  /**
   * @fn _Bool ntools_device_is_up(int fd, char name[IF_NAMESIZE])
   * @brief Test if the current device is up.
   * @param fd Device FD.
   * @param name Device name.
   * @return 1 if up else 0..
   */
  _Bool ntools_device_is_up(int fd, char name[IF_NAMESIZE]);

  /**
   * @fn __u32 ntools_datas_available(int fd)
   * @brief Get the number of available datas to be read.
   * @param fd Socket FD.
   * @return Available datas.
   */
  __u32 ntools_datas_available(int fd);

  /**
   * @fn void ntools_print_hex(FILE* std, net_buffer_t buffer, int len, _Bool print_raw)
   * @brief Print the packet in hexa (wireshark like).
   * @param std Output stream.
   * @param buffer Packet.
   * @param len Packet length.
   * @param print_raw Display in raw mode.
   */
  void ntools_print_hex(FILE* std, net_buffer_t buffer, int len, _Bool print_raw);

  /**
   * @fn int ntools_prepare_ifaces(struct iface_s *ifaces, int *maxfd, fd_set *rset, const char iname[IF_NAMESIZE])
   * @brief List all network interfaces, configures and adds into the list (CAUTION: after the call of this function a socket is opened).
   * @param ifaces Interfaces list.
   * @param maxfd Used by select function.
   * @param rset fd_set Used by select function.
   * @param iname The interface name.
   * @return -1 on error else 0.
   */
  int ntools_prepare_ifaces(struct iface_s *ifaces, int *maxfd, fd_set *rset, const char iname[IF_NAMESIZE]);

  /**
   * @fn void ntools_add_iface(struct iface_s* list, char name[IF_NAMESIZE], int index, int fd, int family)
   * @brief Add an interface into the list.
   * @param list Interfaces list.
   * @param name Interface name.
   * @param index Interface index.
   * @param fd Socket FD.
   * @param family Interface family.
   */
  void ntools_add_iface(struct iface_s* list, char name[IF_NAMESIZE], int index, int fd, int family);

  /**
   * @fn void ntools_clear_ifaces(struct iface_s* ifaces)
   * @brief Clear the interfaces list.
   * @param ifaces List to clear.
   */
  void ntools_clear_ifaces(struct iface_s* ifaces);

  /**
   * @fn const char* ntools_long_to_ip(unsigned int v)
   * @brief Convert a long value to an IP address.
   * @param v Long value.
   * @return IP addr.
   */
  const char* ntools_long_to_ip(unsigned int v);

  /**
   * @fn unsigned int ntools_ip_to_long(const char* s)
   * @brief Convert an IP address to a long value.
   * @param s IP address
   * @return Long value.
   */
  unsigned int ntools_ip_to_long(const char* s);

  /**
   * @fn pcap_hdr_t ntools_pcap_global_hdr(void)
   * @brief Build the main header of the pcap file.
   * @param link Data link type.
   * @return pcap_hdr_t
   */
  pcap_hdr_t ntools_pcap_global_hdr(__u32 link);

  /**
   * @fn pcap_hdr_t ntools_pcap_packet_hdr(__u32 incl_len, __u32 ori_len)
   * @brief Build the packet header of the pcap file.
   * @return pcaprec_hdr_t.
   */
  pcaprec_hdr_t ntools_pcap_packet_hdr(__u32 incl_len, __u32 ori_len);

  /**
   * @fn void ntools_write_pcap_packet(const FILE* output, const net_buffer_t buffer, size_t a_length, size_t r_length, _Bool *first)
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
  void ntools_write_pcap_packet(FILE* output, __u32 link, const net_buffer_t buffer, size_t a_length, size_t r_length, _Bool *first);

  /**
   * @fn void ntools_mac2str(mac_t mac, smac_t m)
   * @brief Convert a MAC array into a string.
   * @param mac MAC to convert.
   * @param m MAC in string.
   */
  void ntools_mac2str(mac_t mac, smac_t m);

  /**
   * @fn void ntools_str2mac(smac_t mac, mac_t m)
   * @brief Convert a MAC string into a MAC array.
   * @param mac MAC to convert
   * @param m MAC in array.
   */
  void ntools_str2mac(smac_t mac, mac_t m);

  /**
   * @fn _Bool ntools_valid_mac(smac_t mac)
   * @brief Test if the MAC is valid.
   * @param mac MAC address to test.
   * @return 1 if valid else 0.
   */
  _Bool ntools_valid_mac(smac_t mac);

#endif /* __NTOOLS_H__ */
/**
 *******************************************************************************
 * @file neturils.h
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
#ifndef __NETUTILS_H__
  #define __NETUTILS_H__


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


  struct netutils_filter_s {
    __u32 ip;
    __u32 port;
    char iface[IF_NAMESIZE];
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

  struct netutils_headers_s {
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
   * @def NETUTILS_MAX_SSIZE
   * @brief Taille max du string utilise avec la fonction "netutils_size_to_string"
   * @see bns_utils_size_to_string
   */
  #define NETUTILS_MAX_SSIZE 15


  typedef enum { NETUTILS_CONVERT_NONE, NETUTILS_CONVERT_HOST2NET, NETUTILS_CONVERT_NET2HOST} netutils_convert_et;

 /**
   * @fn int netutils_decode_buffer(const char* buffer, __u32 length, struct netutils_headers_s *net, bns_packet_convert_et convert)
   * @brief Decodage des paquets en fonction du buffer.
   * @param buffer Buffer de donnee.
   * @param length Taille du buffer.
   * @param net Liste des entetes.
   * @param convert Conversion de certains champs des differentes entetes.
   * @return -1 sur erreur sinon la taill de la payload (peut etre 0).
   */
  int netutils_decode_buffer(const char* buffer, __u32 length, struct netutils_headers_s *net, netutils_convert_et convert);

  /**
   * @fn void netutils_release_buffer(struct netutils_headers_s *net)
   * @brief Liberation des ressources allouee par decode_network_buffer.
   * @param net Liste des entetes a liberer.
   */
  void netutils_release_buffer(struct netutils_headers_s *net);

  /**
   * @fn _Bool netutils_match_from_simple_filter(struct netutils_header_s *net, struct netutils_filter_s filter)
   * @brief Test si le regle matche ou non.
   * @param net entetes.
   * @param filter Filtre a tester.
   * @return Retourne 1 si match.
   */
  _Bool netutils_match_from_simple_filter(struct netutils_headers_s *net, struct netutils_filter_s filter);

  /**
   * @fn int netutils_is_ipv4(const char* ip)
   * @brief Test si l'adresse ip est valide.
   * @param ip Adresse IP.
   * @return -1 si erreur, 0 si non match, 1 si match.
   */
  int netutils_is_ipv4(const char* ip);

  /**
   * @fn int netutils_hostname_to_ip(const char *hostname, char* ip)
   * @brief Recuperation de l'adresse ip en fonction du nom de host.
   * @param hostname Nom du host.
   * @param ip Adresse IP.
   * @return -1 si erreur sinon 0.
   */
  int netutils_hostname_to_ip(const char *hostname, char* ip);

  /**
   * @fn _Bool netutils_device_is_up(int fd, char name[IF_NAMESIZE])
   * @brief Effectue un test pour savoir si le device est up
   * @param fd FD pour l'ioctl.
   * @param name Nom du device.
   * @return Vrai si up.
   */
  _Bool netutils_device_is_up(int fd, char name[IF_NAMESIZE]);

  /**
   * @fn __u32 netutils_datas_available(int fd)
   * @brief Recuperation du nombre de donnees a lire.
   * @param fd fd a tester.
   * @return Nb donnees a lire. 
   */
  __u32 netutils_datas_available(int fd);

  /**
   * @fn void netutils_print_hex(FILE* std, char* buffer, int len, _Bool print_raw)
   * @brief Affichage d'un packet (wireshark like).
   * @param std Flux de sortie.
   * @param buffer Packet.
   * @param len Taille du packet.
   * @param print_raw Affichage en raw mode.
   */
  void netutils_print_hex(FILE* std, char* buffer, int len, _Bool print_raw);

  /**
   * @fn int netutils_prepare_ifaces(struct iface_s *ifaces, int *maxfd, fd_set *rset, const char iname[IF_NAMESIZE])
   * @brief Liste toutes les interfaces et les ajoutent a la liste (IMPORTANT: apres appel de cette methode des sockets sont ouverts).
   * @param ifaces Liste des interfaces (la taille vaut 1 ou 0 si iname n'est pas vide).
   * @param maxfd Utilise pour le select.
   * @param rset fd_set utilise pour le select.
   * @param iname Demande la configuration d'une interface.
   * @return -1 en cas d'erreur sinon 0.
   */
  int netutils_prepare_ifaces(struct iface_s *ifaces, int *maxfd, fd_set *rset, const char iname[IF_NAMESIZE]);

  /**
   * @fn void netutils_add_iface(struct iface_s* list, char name[IF_NAMESIZE], int index, int fd, int family)
   * @brief Ajout d'un interface a la liste.
   * @param list Liste d'interfaces.
   * @param name Nom de l'interface.
   * @param index Index de l'interface.
   * @param fd FD du socket utilise.
   * @param family Famille de l'interface.
   */
  void netutils_add_iface(struct iface_s* list, char name[IF_NAMESIZE], int index, int fd, int family);

  /**
   * @fn void netutils_clear_ifaces(struct iface_s* ifaces)
   * @brief Suppression des elements de la liste.
   * @param ifaces Liste a vider.
   */
  void netutils_clear_ifaces(struct iface_s* ifaces);

  /**
   * @fn const char* netutils_long_to_ip(unsigned int v)
   * @brief Transforme un long en adresse IP.
   * @param v long a transformer.
   * @return L'adresse IP.
   */
  const char* netutils_long_to_ip(unsigned int v);

  /**
   * @fn unsigned int netutils_ip_to_long(const char* s)
   * @brief Transforme une adresse IP en long.
   * @param s IP a transformer.
   * @return Long.
   */
  unsigned int netutils_ip_to_long(const char* s);

  /**
   * @fn long netutils_fsize(FILE* file)
   * @brief Recupere la taille du fichier.
   * @param file Taille.
   * @return Long.
   */
  long netutils_fsize(FILE* file);

  /**
   * @fn pcap_hdr_t netutils_pcap_global_hdr(void)
   * @brief Construction du main header du fichier.
   * @return pcap_hdr_t
   */
  pcap_hdr_t netutils_pcap_global_hdr(void);

  /**
   * @fn pcap_hdr_t netutils_pcap_packet_hdr(__u32 incl_len, __u32 ori_len)
   * @brief Construction du header par paquets.
   * @return pcaprec_hdr_t.
   */
  pcaprec_hdr_t netutils_pcap_packet_hdr(__u32 incl_len, __u32 ori_len);


  /**
   * @fn void netutils_size_to_string(long size, char ssize[NETUTILS_MAX_SSIZE])
   * @brief Convertie une taille en string avec l'unite.
   * @param size Taille.
   * @param ssize Output
   */
  void netutils_size_to_string(long size, char ssize[NETUTILS_MAX_SSIZE]);

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
  void netutils_write_pcap_packet(FILE* output, const char* buffer, size_t a_length, size_t r_length, _Bool *first);
#endif /* __NETUTILS_H__ */

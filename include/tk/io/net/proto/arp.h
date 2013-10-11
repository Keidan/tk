/**
*******************************************************************************
* @file arp.h
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
#ifndef __ARP_H__
  #define __ARP_H__

  #include <tk/io/net/netiface.h>
  #include <linux/if_packet.h>


  #define ARP_TIMEOUT      4000 //4s
  #define ARP_MAX_ATTEMPTS 4

  typedef unsigned char arp_buffer_t[42];

  struct arpcfg_s {
      unsigned int max_attempts;
      unsigned long timeout;
      _Bool debug;
  };

  #define DEF_ARP_CFG ((struct arpcfg_s){ARP_MAX_ATTEMPTS,ARP_TIMEOUT,0})
  #define DEF_ARP_DBG ((struct arpcfg_s){ARP_MAX_ATTEMPTS,ARP_TIMEOUT,1})

/*  arp_flags and at_flags field values */
  #define ATE_INUSE       0x01 /* entry in use */
  #define ATE_COM         0x02 /* completed entry (enaddr valid) */
  #define ATE_PERM        0x04 /* permanent entry */
  #define ATE_PUBL        0x08 /* publish entry (respond for other host) */
  #define ATE_USETRAILERS 0x10 /* has requested trailers */
  #define ATE_PROXY       0x20 /* Do PROXY arp */

  struct arp_entry_s {
      netiface_name_t name;
      netiface_ip4_t ip;
      netiface_mac_t mac;
      netiface_bmac_t bmac;
      int flags;
  };


  /**
   * @fn int arp_find_from_table(char* ip, struct arp_entry_s *entry)
   * @brief Search an arp entry from the system table.
   * @param ip The ip or the hostname to search.
   * @param entry The result entry (only available if this function return 1).
   * @return -1: error or not found, 0: found
   */
  int arp_find_from_table(char* ip, struct arp_entry_s *entry);

  /**
   * @fn int arp_add_in_table(netiface_name_t name, const char *ip, netiface_mac_t mac)
   * @brief Add a new entry into the ARP table.
   * @param name The interface name.
   * @param ip The ip or the hostname to add.
   * @param mac The mac to add.
   * @return -1 on error else 0;
   */
  int arp_add_in_table(netiface_name_t name, const char *ip, netiface_mac_t mac);

  /**
   * @fn _Bool arp_entry_is_inuse(const struct arp_entry_s *entry)
   * @brief Test if the ARP entry contains the flag inuse.
   * @param entry The entry to test.
   * @return 0 if the entry does not contains the flag else 1.
   */
  _Bool arp_entry_is_inuse(const struct arp_entry_s *entry);

  /**
   * @fn _Bool arp_entry_is_completed(const struct arp_entry_s *entry)
   * @brief Test if the ARP entry contains the flag completed.
   * @param entry The entry to test.
   * @return 0 if the entry does not contains the flag else 1.
   */
  _Bool arp_entry_is_completed(const struct arp_entry_s *entry);

  /**
   * @fn _Bool arp_entry_is_permanent(const struct arp_entry_s *entry) 
   * @brief Test if the ARP entry contains the flag permanent.
   * @param entry The entry to test.
   * @return 0 if the entry does not contains the flag else 1.
   */
  _Bool arp_entry_is_permanent(const struct arp_entry_s *entry);

  /**
   * @fn _Bool arp_entry_is_published(const struct arp_entry_s *entry)
   * @brief Test if the ARP entry contains the flag published.
   * @param entry The entry to test.
   * @return 0 if the entry does not contains the flag else 1.
   */
  _Bool arp_entry_is_published(const struct arp_entry_s *entry);

  /**
   * @fn _Bool arp_entry_is_trailers(const struct arp_entry_s *entry)
   * @brief Test if the ARP entry contains the flag trailers.
   * @param entry The entry to test.
   * @return 0 if the entry does not contains the flag else 1.
   */
  _Bool arp_entry_is_trailers(const struct arp_entry_s *entry);

  /**
   * @fn _Bool arp_entry_is_proxy(const struct arp_entry_s *entry)
   * @brief Test if the ARP entry contains the flag proxy.
   * @param entry The entry to test.
   * @return 0 if the entry does not contains the flag else 1.
   */
  _Bool arp_entry_is_proxy(const struct arp_entry_s *entry);

  /**
   * @fn int arp_is_arp_frame(arp_buffer_t frame, unsigned int l)
   * @brief Test if the current frame is a vaid ARP frame. 
   * @param frame The frame to test.
   * @param l The frame length.
   * @return -1 = non arp, 0 = arp and 1 = arp reply.
   */
  int arp_is_arp_frame(arp_buffer_t frame, unsigned int l);

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
  int arp_send_request(int fd, struct sockaddr_ll *ndev, struct netiface_info_s me, arp_buffer_t buffer, netiface_ip4_t dest, _Bool debug);

  /**
   * @fn int arp_resolve_ip(struct arpcfg_s cfg, struct netiface_info_s me, netiface_ip4_t dest_ip, netiface_mac_t *dest_mac)
   * @brief Resolve the ARP request.
   * @param cfg ARP config.
   * @param me Internal iface config.
   * @param dest_ip The dest ip4 to resolve.
   * @param dest_mac The resolved mac.
   * @retrn -1 on error else 0.
   */
  int arp_resolve_ip(struct arpcfg_s cfg, struct netiface_info_s me, netiface_ip4_t dest_ip, netiface_mac_t *dest_mac);

#endif /* __ARP_H__ */

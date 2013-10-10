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

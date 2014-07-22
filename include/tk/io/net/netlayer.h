/**
*******************************************************************************
* @file netlayer.h
* @author Keidan
* @date 18/07/2014
* @par Project
* tk
*
* @par Copyright
* Copyright 2011-2014 Keidan, all right reserved
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
#ifndef __NETLAYER_H__
  #define __NETLAYER_H__

  #include <stdint.h>
  #include <tk/io/net/netiface.h>

  typedef void* netlayer_t;

  #define NETLAYER_DEFAULT_IP4_TTL 64
  #define NETLAYER_DEFAULT_IP4_TOS 16

  /**
   * @fn uint16_t netlayer_cksum16(uint16_t *buf, uint32_t nbytes)
   * @brief Generic checksum calculation.
   * @param buf The buffer to calculate;
   * @param nbytes The buffer len.
   * @return The checksum.
   */
  uint16_t netlayer_cksum16(uint16_t *buf, uint32_t nbytes);

  /**
   * @fn netlayer_t netlayer_new()
   * @brief Allocate a new netlayer
   * @return netlayer
   */
  netlayer_t netlayer_new();

  /**
   * @fn void netlayer_delete(netlayer_t nlayer)
   * @brief Release the allocated netlayer
   * @param netlayer netlayer pointer
   */
  void netlayer_delete(netlayer_t nlayer);

  /**
   * @fn void netlayer_ethernet(netlayer_t nlayer, netiface_mac_t hwr_src, netiface_mac_t hwr_dst, uint32_t iface_idx, uint16_t next_eth_p)
   * @brief Build the ethernet header
   * @param netlayer netlayer pointer
   * @param hwr_src The source MAC address
   * @param hwr_dst The destination MAC address
   * @param iface_idx The interface idx to use
   * @param next_eth_p The next packet type (eg: ETH_P_IP)
   */
  void netlayer_ethernet(netlayer_t nlayer, netiface_mac_t hwr_src, netiface_mac_t hwr_dst, uint32_t iface_idx, uint16_t next_eth_p);

  /**
   * @fn void netlayer_ip4(netlayer_t nlayer, uint8_t tos, uint8_t ttl, char* src_ip, char* dst_ip, uint8_t next_ipproto)
   * @brief Build the ip v4 header
   * @param nlayer netlayer pointer
   * @param tos The TOS value (eg: NETLAYER_DEFAULT_IP4_TOS)
   * @param ttl The TTL value (eg: NETLAYER_DEFAULT_IP4_TTL)
   * @param src_ip The source IPv4 address
   * @param dst_ip The destination IPv4 address
   * @param next_ipproto The next IP protocol (eg: IPPROTO_ICMP)
   */
  void netlayer_ip4(netlayer_t nlayer, uint8_t tos, uint8_t ttl, char* src_ip, char* dst_ip, uint8_t next_ipproto);

  /**
   * @fn void netlayer_payload(netlayer_t nlayer, uint8_t* buffer, uint32_t length)
   * @brief Add extra payload (stored in call orders)
   * @param nlayer netlayer pointer
   * @param buffer The buffer to add
   * @param length The buffer length
   */
  void netlayer_payload(netlayer_t nlayer, uint8_t* buffer, uint32_t length);

  /**
   * @fn int netlayer_finish(netlayer_t nlayer, int fd)
   * @brief Finish the packets and send ti the FD.
   * @param nlayer netlayer pointer
   * @param fd The socket FD
   * @return The result of sendto (man sendto)
   */
  int netlayer_finish(netlayer_t nlayer, int fd);

  /**
   * @fn void netlayer_clear(netlayer_t nlayer)
   * @brief Clear the internal buffer.
   * @param nlayer netlayer pointer
   */
  void netlayer_clear(netlayer_t nlayer);

#endif /* __NETLAYER_H__ */

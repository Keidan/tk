/**
 *******************************************************************************
 * @file nprint.h
 * @author Keidan
 * @date 19/05/2013
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

#ifndef __NPRINT_H__
  #define __NPRINT_H__

  #include <tk/io/net/ntools.h>
  #include <limits.h>

  #define NPRINT_SET_NSET(cond) (cond ? "Set" : "Not Set")

  /**
   * @fn void nprint_print_headers(const net_buffer_t buffer, __u32 length, struct ntools_headers_s  net)
   * @brief Print all headers.
   * @param buffer Buffer datas.
   * @param length Buffer length.
   * @param net Headers.
   */
  void nprint_print_headers(const net_buffer_t buffer, __u32 length, struct ntools_headers_s net);

  /**
   * @fn void nprint_print_eth(struct ethhdr *eth)
   * @brief Print the ethernet header.
   * @param eth Ethernet header.
   */
  void nprint_print_eth(struct ethhdr *eth);

  /**
   * @fn void nprint_print_arp(struct arphdrs *arpp)
   * @brief Print the ARP header.
   * @param arpp ARP header
   */
  void nprint_print_arp(struct arphdrs *arpp);

  /**
   * @fn void nprint_print_ip(struct iphdr* ipv4)
   * @brief Print the IP v4 header.
   * @param ipv4 IPv4 header.
   */
  void nprint_print_ip(struct iphdr* ipv4);

  /**
   * @fn void nprint_print_upd(struct udphdr *udp)
   * @brief Print the UDP header.
   * @param udp UDP header
   */
  void nprint_print_upd(struct udphdr *udp);

  /**
   * @fn void nprint_print_tcp(struct tcphdr *tcp)
   * @brief Print the TCP header
   * @param tcp TCP header.
   */
  void nprint_print_tcp(struct tcphdr *tcp);

#endif /* __NPRINT_H__ */

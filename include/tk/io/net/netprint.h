/**
 *******************************************************************************
 * @file netprint.h
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

#ifndef __NETPRINT_H__
  #define __NETPRINT_H__

  #include <tk/io/net/netutils.h>
  #include <limits.h>

  #define NETPRINT_SET_NSET(cond) (cond ? "Set" : "Not Set")

  /**
   * @fn void netprint_print_headers(const char* buffer, __u32 length, struct bns_network_s net)
   * @brief Print all headers.
   * @param buffer Buffer datas.
   * @param length Buffer length.
   * @param net Headers.
   */
  void netprint_print_headers(const char* buffer, __u32 length, struct netutils_headers_s net);

  /**
   * @fn void netprint_print_eth(struct ethhdr *eth)
   * @brief Print the ethernet header.
   * @param eth Ethernet header.
   */
  void netprint_print_eth(struct ethhdr *eth);

  /**
   * @fn void netprint_print_arp(struct arphdrs *arpp)
   * @brief Print the ARP header.
   * @param arpp ARP header
   */
  void netprint_print_arp(struct arphdrs *arpp);

  /**
   * @fn void netprint_print_ip(struct iphdr* ipv4)
   * @brief Print the IP v4 header.
   * @param ipv4 IPv4 header.
   */
  void netprint_print_ip(struct iphdr* ipv4);

  /**
   * @fn void netprint_print_upd(struct udphdr *udp)
   * @brief Print the UDP header.
   * @param udp UDP header
   */
  void netprint_print_upd(struct udphdr *udp);

  /**
   * @fn void netprint_print_tcp(struct tcphdr *tcp)
   * @brief Print the TCP header
   * @param tcp TCP header.
   */
  void netprint_print_tcp(struct tcphdr *tcp);

#endif /* __NETPRINT_H__ */

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

  #include <tk/net/netutils.h>
  #include <limits.h>

  #define NETPRINT_SET_NSET(cond) (cond ? "Set" : "Not Set")

  /**
   * @fn void netprint_print_headers(const char* buffer, __u32 length, struct bns_network_s net)
   * @brief Affichage des entetes.
   * @param buffer Buffer de donnee.
   * @param length Taille du buffer.
   * @param net Entetes.
   */
  void netprint_print_headers(const char* buffer, __u32 length, struct netutils_headers_s net);

  /**
   * @fn void netprint_print_eth(struct ethhdr *eth)
   * @brief Affichage de l'entete Ethernet.
   * @param eth Entete Ethernet.
   */
  void netprint_print_eth(struct ethhdr *eth);

  /**
   * @fn void netprint_print_arp(struct arphdrs *arpp)
   * @brief Affichage de l'entete ARP.
   * @param arpp Entete ARP.
   */
  void netprint_print_arp(struct arphdrs *arpp);

  /**
   * @fn void netprint_print_ip(struct iphdr* ipv4)
   * @brief Affichage de l'entete IPv4/IPv6.
   * @param ipv4 Entete IPv4.
   */
  void netprint_print_ip(struct iphdr* ipv4);

  /**
   * @fn void netprint_print_upd(struct udphdr *udp)
   * @brief Affichage de l'entete UDP.
   * @param udp Entete UDP.
   */
  void netprint_print_upd(struct udphdr *udp);

  /**
   * @fn void netprint_print_tcp(struct tcphdr *tcp)
   * @brief Affichage de l'entete TCP.
   * @param tcp Entet TCP.
   */
  void netprint_print_tcp(struct tcphdr *tcp);

#endif /* __NETPRINT_H__ */

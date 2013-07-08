/**
 *******************************************************************************
 * @file netprint.c
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tk/net/netprint.h>
#include <tk/net/netutils.h>
#include <tk/sys/log.h>


#define SET_NSET(cond) (!!(cond)), NETPRINT_SET_NSET(cond)

/** 
 * Pour eviter les pb de support et surtout de compilation la structure est 
 * copiée du header linux/ipv6.h
 */
struct ipv6hdr {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    __u8               priority:4,
  version:4;
#elif defined(__BIG_ENDIAN_BITFIELD)
  
    __u8               version:4,
  priority:4;
#else
#error  "Please fix <asm/byteorder.h>"
#endif
    __u8               flow_lbl[3];
  
    __be16             payload_len;
    __u8               nexthdr;
    __u8               hop_limit;
    struct  in6_addr   saddr;
    struct  in6_addr   daddr;
};


/**
 * @fn void netprint_print_headers(const char* buffer, __u32 length, struct bns_network_s net)
 * @brief Affichage des entetes.
 * @param buffer Buffer de donnee.
 * @param length Taille du buffer.
 * @param net Entetes.
 */
void netprint_print_headers(const char* buffer, __u32 length, struct netutils_headers_s net) {
  /* affichage de l'entete ethernet */
  netprint_print_eth(net.eth);
  /* Si le paquet contient un header IP v4/v6 on decode */
  if(net.ipv4) {
    /* affichage de l'entete IP */
    netprint_print_ip(net.ipv4);
    if(net.tcp) {
      /* affichage de l'entete TCP */
      netprint_print_tcp(net.tcp);
    } else if(net.udp) {
      /* affichage de l'entete UDP */
      netprint_print_upd(net.udp);
    }
    /* Si le paquet contient un header ARP */
  } else if(net.arp) {
    /* affichage de l'entete ARP */
    netprint_print_arp(net.arp);
  } /* le paquet ne contient pas de header ip ni arp ; non gere ici*/
  printf("\n");/* mise en page */
  /* affichage du buffer */
  netutils_print_hex(stdout, (char*)buffer, length, 0);
}

/**
 * @fn void netprint_print_eth(struct ethhdr *eth)
 * @brief Affichage de l'entete Ethernet.
 * @param eth Entete Ethernet.
 */
void netprint_print_eth(struct ethhdr *eth) {
  printf("Ethernet:\n");
  printf("\tSource: %02x:%02x:%02x:%02x:%02x:%02x\n\tDestination: %02x:%02x:%02x:%02x:%02x:%02x\n\tType:0x%04x\n",
	 eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5],
	 eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5],
	 eth->h_proto);
}

/**
 * @fn void netprint_print_arp(struct arphdrs *arpp)
 * @brief Affichage de l'entete ARP.
 * @param arpp Entete ARP.
 */
void netprint_print_arp(struct arphdrs *arpp) {
  struct arphdr *arp = arpp->arp1;
  struct arphdr2 *p2 = arpp->arp2;
  printf("Adress Resolution Protocol:\n");
  printf("\tHardware type: 0x%04x\n", arp->ar_hrd);
  printf("\tProtocol type: 0x%04x\n", arp->ar_pro);
  printf("\tHardware size: %x\n", arp->ar_hln);
  printf("\tProtocol size: %x\n", arp->ar_pln);
  printf("\tOpcode: %s (%x)\n", (arp->ar_op == 2 ? "reply" : (arp->ar_op == 1 ? "request" : "unknown")) , arp->ar_op);

  /* 
   * Uniquement pour les requetes reponse et si la taille du protocol vaut 4 (IPv4).
   * Je n'ai rien sous la main pour tester l'IPv6, je passe mon tour pour le moment...
   */
  if(p2) {
    printf("\tSender MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",
	   p2->sha[0], p2->sha[1], p2->sha[2], p2->sha[3], p2->sha[4], p2->sha[5]);
    printf("\tSender IP address: %d.%d.%d.%d\n",
	   p2->sip[0], p2->sip[1], p2->sip[2], p2->sip[3]);
    printf("\tTarget MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",
	   p2->tha[0], p2->tha[1], p2->tha[2], p2->tha[3], p2->tha[4], p2->tha[5]);
    printf("\tTarget IP address: %d.%d.%d.%d\n",
	   p2->tip[0], p2->tip[1], p2->tip[2], p2->tip[3]);
  }
}

/**
 * @fn void netprint_print_ip(struct iphdr* ipv4)
 * @brief Affichage de l'entete IPv4/IPv6.
 * @param ipv4 Entete IPv4.
 */
void netprint_print_ip(struct iphdr* ipv4) {
  if(ipv4->version == 4) {
    /* Affichage de l'entete IPv4 */
    char src [INET_ADDRSTRLEN], dst [INET_ADDRSTRLEN];
    memset(dst, 0, sizeof(dst));
    memset(src, 0, sizeof(src));
    inet_ntop(AF_INET, &ipv4->saddr, src, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ipv4->daddr, dst, INET_ADDRSTRLEN);

    printf("Internet Protocol Version %d:\n", ipv4->version);
    printf("\tVersion: %d\n\tHeader length: %d bytes\n", ipv4->version, (int)(ipv4->ihl + sizeof(struct iphdr)));
    printf("\tDifferentiated Services Field:\n");
    printf("\t\tTotal Length: %d\n\t\tIdentification: 0x%04x (%d)\n", ipv4->tot_len, ipv4->tos, ipv4->tos);
    printf("\tFlags: 0x%02x\n", ipv4->id);
    printf("\t\t%d... Reserved bit: %s\n",  SET_NSET(ipv4->id&IP_RF));
    printf("\t\t.%d.. Don't fragment: %s\n", SET_NSET(ipv4->id&IP_DF));
    printf("\t\t..%d. More fragments: %s\n", SET_NSET(ipv4->id&IP_MF));
    printf("\tFragment offset: %d\n", ipv4->frag_off);
    printf("\tTime to live: %d\n\tProtocol: %d\n\tHeader checksum: 0x%04x\n", ipv4->ttl, ipv4->protocol, ipv4->check);
    printf("\tSource: %s\n\tDestination: %s\n", src, dst);
  } else { /* ip v6 */
    /* Je n'ai rien sous la main pour tester l'IPv6 */
/*       struct ipv6hdr *ip6 = (struct ipv6hdr*)(buffer + *offset); */
/*       *offset += sizeof(struct ipv6hdr); */
/*       /\* Affichage de l'entete IPv6 *\/ */
/*       char dst [INET6_ADDRSTRLEN], src [INET6_ADDRSTRLEN]; */
/*       memset(dst, 0, sizeof(dst)); */
/*       memset(src, 0, sizeof(src)); */
/*       inet_ntop(AF_INET6, &ip6->saddr, src, INET6_ADDRSTRLEN); */
/*       inet_ntop(AF_INET6, &ip6->daddr, dst, INET6_ADDRSTRLEN); */
/*       printf("Internet Protocol Version 6:\n"); */
/*       printf("\tVersion: %d\n\tPriority: %d\n\tFlowlabel:0x%02x%02x%02x\n", */
/*       	     ip6->version, ip6->priority, */
/*       	     ip6->flow_lbl[0], ip6->flow_lbl[1], ip6->flow_lbl[2]); */
/*       printf("\tPayload length: %d\n\tNext header: %d\n\tHop limit: %d\n", */
/*       	      ip6->payload_len, ip6->nexthdr, ip6->hop_limit); */
/*       printf("\tSource: %s\n\tDestination: %s\n", src, dst); */
/*       return ip6->nexthdr; */
  }
}

/**
 * @fn void netprint_print_upd(struct udphdr *udp)
 * @brief Affichage de l'entete UDP.
 * @param udp Entete UDP.
 */
void netprint_print_upd(struct udphdr *udp) {
  /* Affichage de l'entete UDP */
  printf("User Datagram Protocol:\n");
  printf("\tSource port: %d\t\n\tDestination port: %d\n", udp->source, udp->dest);
  printf("\tLength: %d\n\tChecksum: 0x%04x\n", udp->len, udp->check);
}

/**
 * @fn void netprint_print_tcp(struct tcphdr *tcp)
 * @brief Affichage de l'entete TCP.
 * @param tcp Entet TCP.
 */
void netprint_print_tcp(struct tcphdr *tcp) { 
  /* Affichage de l'entete TCP */
  printf("Transmission Control Protocol:\n");
  printf("\tSource port: %d\n\tDestination port: %d\n", tcp->source, tcp->dest);
  printf("\tSequence number: %d\n\tAcknowledgement number: %d\n", tcp->seq, tcp->ack_seq);
  printf("\tFlags:\n");
  printf("\t\t%d... .... = Congestion Window Reduced (CWR): %s\n", SET_NSET(tcp->cwr));
  printf("\t\t.%d.. .... = ECN-Echo: %s\n", SET_NSET(tcp->ece));
  printf("\t\t..%d. .... = Urgent: %s\n", SET_NSET(tcp->urg));
  printf("\t\t...%d .... = Acknowledgement: %s\n", SET_NSET(tcp->ack));
  printf("\t\t.... %d... = Push: %s\n", SET_NSET(tcp->psh));
  printf("\t\t.... .%d.. = Reset: %s\n", SET_NSET(tcp->rst));
  printf("\t\t.... ..%d. = Syn: %s\n", SET_NSET(tcp->syn));
  printf("\t\t.... ...%d = Fin: %s\n", SET_NSET(tcp->fin));
  printf("\tWindow size: %d\n\tChecksum: 0x%04x\n", tcp->window, tcp->check);
  printf("\tUrg ptr: %d\n", tcp->urg_ptr);
}

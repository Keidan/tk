/**
*******************************************************************************
* @file ping.c
* @author Keidan
* @date 10/01/2014
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

#include <tk/io/net/proto/ping.h>
#include <tk/io/net/nettools.h>
#include <tk/io/net/netsocket.h>
#include <tk/sys/log.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <tk/sys/systask.h>
#include <tk/io/net/proto/arp.h>
#include <netinet/ip.h>
#include <netinet/ether.h>

struct ping_s {
    netsocket_t sock;
    htable_t ifaces;
    struct netiface_info_s iface;
    uint16_t seq;
    uint32_t timeout;
    pthread_t t_recv;
    systask_t timer;
    _Bool end;
    struct {
	ping_event_handler_fct fct;
	void* user_data;
    } handler;
    struct {
	char host[255];
	netiface_ip4_t ip;
	netiface_mac_t mac;
    } dest;
};

#define DEFAULT_TTL 64
#define ICMP_PACKET_SIZE 48
#define PING_TIMEOUT    15
#define ICMPHDR_SIZE sizeof(struct icmphdr)
#define create_ptr(local, param) struct ping_s *local = (struct ping_s*)param
#define get_fd(p) netsocket_get_fd(p->sock)

/** local definition of icmp frame, to add to header specific payload */
struct icmp_frame {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t sequence16b;
    struct {
	uint32_t payload_sequence;
	uint32_t timestamp;
	uint8_t data[ICMP_PACKET_SIZE];
    } payload;
};

static uint16_t request = 1;

/**
 * @fn static uint16_t ping_cksum(uint16_t *buf, int nbytes)
 * @brief This function is used calculate icmp header checksum
 * @param buf buffer,
 * @param nbytes size of buf
 * @return checkum of 16-bit length
 */
static uint16_t ping_cksum(uint16_t *buf, int nbytes);
/**
 * @fn static void* ping_receive_event(void* data)
 * @brief This function is a callback when a frame is received on the socket create
 * when trying to poing DNS. It filters every frame not corresponding to our ping
 * @param data user data
 * @return always NULL
 */
static void* ping_receive_event(void* data);
static void ping_send_from_iface(ping_t ping);

void ping_timeout(void* ptr) {
  create_ptr(p, ptr);
  logger(LOG_ERR, "Ping timeout reached.\n");
  ping_start(p, p->dest.host, systask_get_timeout(p->timer));
  if(p->handler.fct)
    p->handler.fct(p, evd(PING_RESULT_TIMEOUT, p->seq, p->dest.host, p->dest.ip, 0, p->handler.user_data));
}


/**
 * @fn ping_t ping_new(const char* iface)
 * @brief Open an PING connection.
 * @param iface The iface name to use.
 * @return The PING context else NULL on error.
 */
ping_t ping_new(const char *iface) {
  struct ping_s *p = NULL;
  if((p = malloc(sizeof(struct ping_s))) == NULL) {
    logger(LOG_ERR, "Unable to alloc a memory for the PING object.\n");
    return NULL;
  }
  memset(p, 0, sizeof(struct ping_s));


  p->ifaces = netiface_list_new(NETIFACE_LVL_UDP, NETIFACE_KEY_NAME);
  netiface_t ifa = netiface_list_get(p->ifaces, (char*)iface);
  if(!ifa) {
    netiface_list_delete(p->ifaces);
    logger(LOG_ERR, "ping iface failed\n");
    ping_delete(p);
    return NULL;
  }
  netiface_read(ifa, &p->iface);

  if(nettools_is_ipv4(p->iface.ip4) != 1) {
    logger(LOG_ERR, "No valid IP found for iface %s\n", iface);
    ping_delete(p);
    return NULL;
  }
  p->sock = netsocket_new0();
  if(p->sock < 0) {
    logger(LOG_ERR, "ping socket failed: (%d) %s\n", errno, strerror(errno));
    ping_delete(p);
    return NULL;
  }

  netsocket_set_fd(p->sock, socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)));
  if(get_fd(p) < 0) {
    logger(LOG_ERR, "ping fd failed %d: (%d) %s\n", get_fd(p), errno, strerror(errno));
    ping_delete(p);
    return NULL;
  }

  if(netiface_bind(ifa) == -1) {
    logger(LOG_ERR, "Unable to bind to the iface %s\n", iface);
    ping_delete(p);
    return NULL;
  }

  if(!(p->timer = systask_new(PING_TIMEOUT, 1, ping_timeout, p))) {
    ping_delete(p);
    return NULL;
  }
  if(pthread_create(&p->t_recv, NULL, ping_receive_event, p) != 0) {
    logger(LOG_ERR, "Unable to start the receiver thread: (%d) %s\n", errno, strerror(errno));
    ping_delete(p);
    return NULL;
  }

  return p;
}

/**
 * @fn void ping_delete(ping_t ping)
 * @brief release the ping context.
 * @param ping The context.
 */
void ping_delete(ping_t ping) {
  create_ptr(p, ping);
  if(!p) return;
  if(p->sock) netsocket_delete(p->sock), p->sock = NULL;
  if(p->t_recv) {
    pthread_cancel(p->t_recv);
    pthread_join(p->t_recv, NULL);
    p->t_recv = 0;
  }
  if(p->ifaces) netiface_list_delete(p->ifaces), p->ifaces = NULL;
  systask_delete(p->timer);
  p->handler.fct = NULL;
  free(p);
}


/**
 * @fn int ping_start(ping_t ping, const char* host, uint32_t delay)
 * @brief Start the ping process.
 * @param ping The ping context.
 * @param host The host to ping.
 * @param delay the timeout delay (in ms).
 * @return -1 on error else 0.
 */
int ping_start(ping_t ping, const char* host, uint32_t delay) {
  struct sockaddr_in addr;
  create_ptr(p, ping);
  if(!p) return -1;


  if(nettools_ip_to_sockaddr(host, &addr)) {
    logger(LOG_ERR, "Unable to resove the host: %s\n", host);
    return -1;
  }
  addr.sin_family = AF_INET;
  bzero(p->dest.host, sizeof(p->dest.host));
  bzero(p->dest.ip, sizeof(p->dest.ip));
  strcpy(p->dest.host, host);
  strcpy(p->dest.ip, inet_ntoa(addr.sin_addr));


  if(is_valid_index(p->iface)) {
    struct arpcfg_s acfg = {ARP_MAX_ATTEMPTS,ARP_TIMEOUT,1};
    if(arp_resolve_ip(acfg, p->iface, p->dest.ip, &p->dest.mac) || !strlen(p->dest.mac)){
      logger(LOG_ERR, "Unable to resolve the host %s\n", host);
      return -1;
    }
  }  

  p->end = 0;
  systask_set_timeout(p->timer, delay);

  ping_send_from_iface(ping);

  systask_restart(p->timer);
  return 0;
}

/**
 * @fn void ping_set_event_handler(ping_t ping, ping_event_handler_fct evt, void* user_data)
 * @brief release the ping context.
 * @param ping The context.
 * @param evt The handler.
 * @param user_data The user data passed to the handler.
 */
void ping_set_event_handler(ping_t ping, ping_event_handler_fct evt, void* user_data){
  create_ptr(p, ping);
  if(!p) return;
  p->handler.fct = evt;
  p->handler.user_data = user_data;
}

/**
 * @fn static void* ping_receive_event(void* data)
 * @brief This function is a callback when a frame is received on the socket create
 * when trying to poing DNS. It filters every frame not corresponding to our ping
 * @param data user data
 * @return always NULL
 */
static void* ping_receive_event(void* data) {
  create_ptr(p, data);
  unsigned char buf[4096];
  uint32_t seq;
  struct timespec tp;
  struct icmp_frame *icmp;
  struct iphdr *ip_header;
  uint32_t nms;
  char *dns;

  while(!p->end) {
    memset(buf, 0, sizeof(buf));
    int reads = read(get_fd(p), buf, sizeof(4096));
    if(reads == -1) {
      logger(LOG_ERR, "Error in read: (%d) %s", errno, strerror(errno));
      if(p->handler.fct)
	p->handler.fct(p, evd(PING_RESULT_READ_ERROR, p->seq, p->dest.host, p->dest.ip, 0, p->handler.user_data));
      break;
    }

    clock_gettime(CLOCK_MONOTONIC ,&tp);
    nms  = (tp.tv_sec *1e3) + (tp.tv_nsec *1e-6);

    nettools_print_hex(stdout, buf, reads, 0);

    ip_header = (struct iphdr *)(buf+sizeof(struct ether_header));

    dns = inet_ntoa(*(struct in_addr *)&ip_header->saddr);

    if (ip_header->protocol != IPPROTO_ICMP) {
      logger(LOG_ERR, "filtering non icmp receive");
      if(p->handler.fct)
	p->handler.fct(p, evd(PING_RESULT_NON_ICMP, p->seq, p->dest.host, p->dest.ip, 0, p->handler.user_data));
      continue;
    }

    if (strcmp(p->dest.ip, dns)) {
      logger(LOG_ERR, "filtering not dns origin receive(%s), origin (%s)", dns, p->dest.ip);
      if(p->handler.fct)
	p->handler.fct(p, evd(PING_RESULT_NOT_DNS_ORIGIN, p->seq, p->dest.host, p->dest.ip, 0, p->handler.user_data));
      continue;
    }

    icmp = (struct icmp_frame *)(buf + sizeof(struct ether_header) + sizeof(struct ip));

    if (icmp->type != ICMP_ECHOREPLY) {
      logger(LOG_ERR, "ping failed");
      if(p->handler.fct)
	p->handler.fct(p, evd(PING_RESULT_NOT_REPLY, p->seq, p->dest.host, p->dest.ip, 0, p->handler.user_data));
      continue;
    }

    seq = ntohl(icmp->payload.payload_sequence);
    if ((seq != p->seq) ||
	(ntohs(icmp->sequence16b) != p->seq)) {

      logger(LOG_ERR, "receive non expected frame: type:%d seq:%d %d",
	     icmp->type, seq, ntohs(icmp->sequence16b));

      if(p->handler.fct)
	p->handler.fct(p, evd(PING_RESULT_NON_EXPECTED_FRAME, p->seq, p->dest.host, p->dest.ip, 0, p->handler.user_data));
      continue;
    }
    uint32_t timestamp = (nms - ntohl(icmp->payload.timestamp));
    logger(LOG_INFO, "pong received (seq %d) in %d msec", seq, timestamp);
    systask_stop(p->timer);
    if(p->handler.fct)
      p->handler.fct(p, evd(PING_RESULT_SUCCESS, seq, p->dest.host, p->dest.ip, timestamp, p->handler.user_data));
  }
  pthread_exit(0);
  return NULL;
}

/**
 * @fn static uint16_t ping_cksum(uint16_t *buf, int nbytes)
 * @brief This function is used calculate icmp header checksum
 * @param buf buffer,
 * @param nbytes size of buf
 * @return checkum of 16-bit length
 */
static uint16_t ping_cksum(uint16_t *buf, int nbytes) {
  uint32_t sum = 0;
  uint16_t oddbyte = 0;

  while (nbytes > 1) {
    sum += *buf++;
    nbytes -= 2;
  }

  if (nbytes == 1) {
    oddbyte = 0;
    *((uint16_t *) &oddbyte) = *(uint8_t *) buf;
    sum += oddbyte;
  }
  
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return (uint16_t) ~sum;
}
unsigned short ping_ip_csum(unsigned short *buf, int nwords) {
    unsigned long sum;
    for(sum=0; nwords>0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

static void ping_send_from_iface(ping_t ping) {
  create_ptr(p, ping);
  if(!p) return;
  int tx_len = 0;
  netiface_bmac_t src, dst;
  memset(&src, 0, sizeof(netiface_bmac_t));
  memset(&dst, 0, sizeof(netiface_bmac_t));
  nettools_str2mac(p->dest.mac, dst);
  nettools_str2mac(p->iface.mac, src);

  uint32_t sendbuf_len = sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct icmp_frame);

  char sendbuf[sendbuf_len];
  memset(sendbuf, 0, sendbuf_len);

  struct ether_header *eh = (struct ether_header *) sendbuf;
  memcpy(eh->ether_shost, src, sizeof(netiface_bmac_t));
  memcpy(eh->ether_dhost, dst, sizeof(netiface_bmac_t));
  eh->ether_type = htons(ETH_P_IP);
  tx_len += sizeof(struct ether_header);
  struct iphdr *iph = (struct iphdr *) (sendbuf + tx_len);

  /* IP Header */
  iph->ihl = 5;
  iph->version = 4;
  iph->tos = 16; // Low delay
  iph->id = htons(54321);
  iph->ttl = DEFAULT_TTL; // hops
  iph->protocol = IPPROTO_ICMP;
  /* Source IP address, can be spoofed */
  iph->saddr = inet_addr(p->iface.ip4);
  /* Destination IP address */
  iph->daddr = inet_addr(p->dest.ip);
  tx_len += sizeof(struct iphdr);

  struct timespec tp;
  int i, n;
  struct icmp_frame *icmp = (struct icmp_frame*) (sendbuf + tx_len);
  icmp->type = ICMP_ECHO;
  icmp->code = 0;
  icmp->id = htons(getpid());
  icmp->sequence16b = htons(request);
  icmp->payload.payload_sequence = htonl(request&0x0000FFFF);

  (void)clock_gettime(CLOCK_MONOTONIC ,&tp);
  icmp->payload.timestamp = htonl((tp.tv_sec *1e3) + (tp.tv_nsec *1e-6));

  for (i=0; i< ICMP_PACKET_SIZE; i++)
    icmp->payload.data[i] = i;

  icmp->checksum = ping_cksum((uint16_t*)icmp, sizeof(struct icmp_frame));  
  p->seq = request;
  tx_len += sizeof(struct icmp_frame);

  /* Length of IP payload and header */
  iph->tot_len = htons(tx_len - sizeof(struct ether_header));
  /* Calculate IP checksum on completed header */
  iph->check = ping_ip_csum((unsigned short *)(sendbuf+sizeof(struct ether_header)), sizeof(struct iphdr)/2);

  /* Destination address */
  struct sockaddr_ll socket_address;
  /*prepare sockaddr_ll*/
  /*RAW communication*/
  socket_address.sll_family   = PF_PACKET;
  /*we don't use a protocoll above ethernet layer
    ->just use anything here*/
  socket_address.sll_protocol = htons(ETH_P_IP);
  /*index of the network device
    see full code later how to retrieve it*/
  socket_address.sll_ifindex  = p->iface.index;
  /*ARP hardware identifier is ethernet*/
  socket_address.sll_hatype   = ARPHRD_ETHER;
  socket_address.sll_pkttype  = PACKET_OTHERHOST;
  /*address length*/
  socket_address.sll_halen    = ETH_ALEN;
  /*MAC - begin*/
  memcpy(socket_address.sll_addr, dst, ETH_ALEN);
  /*MAC - end*/
  socket_address.sll_addr[6]  = 0x00;/*not used*/
  socket_address.sll_addr[7]  = 0x00;/*not used*/
  /* Send packet */
  if ((n = sendto(get_fd(p), sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll))) < 0)
    logger(LOG_ERR, "Send failed %d: (%d) %s\n", get_fd(p), errno, strerror(errno));  
  logger(LOG_INFO, "ping %s %d bytes seq %d\n", p->dest.host, n, p->seq);
  request++;

}

/**
*******************************************************************************
* @file netroute.c
* @author Keidan
* @date 23/10/2013
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
#include <tk/io/net/nettools.h>
#include <tk/io/net/netroute.h>
#include <tk/sys/log.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/rtnetlink.h>
#include <net/route.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#define BUFSIZE 8192

/**
 * @fn void netroute_parse(llist_t *list, struct nlmsghdr *nlhdr)
 * @breif Parse the kernel response.
 * @param list The container.
 * @param nlhdr The buffer response.
 */
void netroute_parse(llist_t *list, struct nlmsghdr *nlhdr);
/**
 * @fn int netroute_read(int fd, char *pbuf, __u32 seq)
 * @breif Read the kernel response.
 * @param fd The socket fd.
 * @param pbuf The buffer response.
 * @param seq The seq num of the message.
 * @return -1 on error else 0 on success.
 */
int netroute_read(int fd, char *pbuf, __u32 seq);


/**
 * @fn int netroute_add(struct netroute_item_add_s *item)
 * @bief add a new route entry.
 * @param item The netroute to add.
 * @return -1 on error else 0 on succes.
 */
int netroute_add(struct netroute_item_add_s *item) {
  int fd;
  struct rtentry route;
  struct sockaddr_in *addr;
  int err = 0;
  __u8 cidr;

  // create the socket
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1)  {
    logger(LOG_ERR, "Socket open failed: (%d) %s\n", errno, strerror(errno));
    return -1;
  }
  // set route struct to zero
  memset(&route, 0, sizeof(route));
  // assign the default gateway
  if(item->gateway && strlen(item->gateway)) {
    addr = (struct sockaddr_in*) &route.rt_gateway;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(item->gateway);
  }
  // assign the destination
  if(item->dst && strlen(item->dst)) {
    addr = (struct sockaddr_in*) &route.rt_dst;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(item->dst);
    if(item->dst_mask && !strlen(item->dst_mask)) {
      cidr = nettools_get_cidr(item->dst);
      strcpy(item->dst_mask, nettools_get_mask_by_cidr(cidr));
    }
  }
  // assign the destination mask
  if(item->dst_mask && strlen(item->dst_mask)) {
    addr = (struct sockaddr_in*) &route.rt_genmask;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(item->dst_mask);
  }
  if(item->src_iface && strlen(item->src_iface))
    route.rt_dev = (char*)&item->src_iface[0];
  // set the flags NET/UP
  route.rt_flags = RTF_UP | RTF_GATEWAY;
  // make the ioctl
  if ((err = ioctl(fd, SIOCADDRT, &route)) != 0) {
    close(fd);
    logger(LOG_ERR, "SIOCADDRT failed: (%d) %s\n", errno, strerror(errno));
    return -1;
  }
  close(fd);
  return 0;
}

/**
 * @fn int netroute_del(struct netroute_item_add_s *item)
 * @bief Delete a route entry.
 * @param item The netroute to del.
 * @return -1 on error else 0 on succes.
 */
int netroute_del(struct netroute_item_add_s *item) {
  int fd;
  struct rtentry route;
  struct sockaddr_in *addr;
  int err = 0;
  __u8 cidr;

  // create the socket
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1)  {
    logger(LOG_ERR, "Socket open failed: (%d) %s\n", errno, strerror(errno));
    return -1;
  }
  // set route struct to zero
  memset(&route, 0, sizeof(route));
  // assign the default gateway
  if(item->gateway && strlen(item->gateway)) {
    addr = (struct sockaddr_in*) &route.rt_gateway;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(item->gateway);
  }
  // assign the destination
  if(item->dst && strlen(item->dst)) {
    addr = (struct sockaddr_in*) &route.rt_dst;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(item->dst);
    if(item->dst_mask && !strlen(item->dst_mask)) {
      cidr = nettools_get_cidr(item->dst);
      strcpy(item->dst_mask, nettools_get_mask_by_cidr(cidr));
    }
  }
  // assign the destination mask
  if(item->dst_mask && strlen(item->dst_mask)) {
    addr = (struct sockaddr_in*) &route.rt_genmask;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(item->dst_mask);
  }
  if(item->src_iface && strlen(item->src_iface))
    route.rt_dev = (char*)&item->src_iface[0];
  // set the flags NET/UP
  route.rt_flags = RTF_UP | RTF_GATEWAY;
  // make the ioctl
  if ((err = ioctl(fd, SIOCDELRT, &route)) != 0) {
    close(fd);
    logger(LOG_ERR, "SIOCDELRT failed: (%d) %s\n", errno, strerror(errno));
    return -1;
  }
  close(fd);
  return 0;
}



/**
 * @fn llist_t netroute_ls()
 * @brief List all routes.
 * @return The route list (struct netroute_item_ls_s)
 */
llist_t netroute_ls() {
  llist_t list = NULL;
  struct nlmsghdr *nlmsg;
  //struct rtmsg *rt;
  char msg[BUFSIZE];
  int seq = 0, fd = 0;
  __u32  len;

  if((fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0) {
    logger(LOG_ERR, "Socket open failed: (%d) %s\n", errno, strerror(errno));
    return NULL;
  }

  // Initialize the buffer
  memset(msg, 0, BUFSIZE);

  // point the header and the msg structure pointers into the buffer
  nlmsg = (struct nlmsghdr *)msg;
  //rt = (struct rtmsg *)NLMSG_DATA(nlmsg);
							
  // Fill in the nlmsg header
  nlmsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
  nlmsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .
							
  nlmsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
  nlmsg->nlmsg_seq = seq++; // Sequence of the message packet.
  nlmsg->nlmsg_pid = getpid(); // PID of process sending the request.
							
  // Send the request 
  if(send(fd, nlmsg, nlmsg->nlmsg_len, 0) < 0){
    logger(LOG_ERR, "Socket send failed: (%d) %s\n", errno, strerror(errno));
    close(fd);
    return NULL;
  }
							
  // Read the response
  if((len = netroute_read(fd, msg, seq)) < 0) {
    logger(LOG_ERR, "Read route response failed: (%d) %s\n", errno, strerror(errno));
    close(fd);
    return NULL;
  }
  // THIS IS THE NETTSTAT -RL code I commented out the printing here and in parse routes
  //fprintf(stdout, "Destination\tGateway\tInterface\tSource\n");
  for(;NLMSG_OK(nlmsg,len);nlmsg = NLMSG_NEXT(nlmsg,len))
    netroute_parse(&list, nlmsg);
  close(fd);
  return list;
}

/**
 * @fn int netroute_read(int fd, char *pbuf, __u32 seq)
 * @breif Read the kernel response.
 * @param fd The socket fd.
 * @param pbuf The buffer response.
 * @param seq The seq num of the message.
 * @return -1 on error else 0 on success.
 */
int netroute_read(int fd, char *pbuf, __u32 seq) {
  struct nlmsghdr *nlhdr;
  __u32 rlen = 0, mlen = 0;
					
  do {
    /* Recieve response from the kernel */
    if((rlen = recv(fd, pbuf, BUFSIZE - mlen, 0)) < 0)
      return -1;
						
    nlhdr = (struct nlmsghdr *)pbuf;
						
    /* Check if the header is valid */
    if((NLMSG_OK(nlhdr, rlen) == 0) || (nlhdr->nlmsg_type == NLMSG_ERROR))
      return -1;
						
    /* Check if the its the last message */
    if(nlhdr->nlmsg_type == NLMSG_DONE)
      break;
    else {
      /* Else move the pointer to buffer appropriately */
      pbuf += rlen;
      mlen += rlen;
    }
						
    /* Check if its a multi part message */
    if(!(nlhdr->nlmsg_flags & NLM_F_MULTI))
      /* return if its not */
      break;
  } while((nlhdr->nlmsg_seq != seq) || (nlhdr->nlmsg_pid != getpid()));
  return mlen;
}

/**
 * @fn void netroute_parse(llist_t *list, struct nlmsghdr *nlhdr)
 * @breif Parse the kernel response.
 * @param list The container.
 * @param nlhdr The buffer response.
 */
void netroute_parse(llist_t *list, struct nlmsghdr *nlhdr) {
  struct netroute_item_ls_s item;
  memset(&item, 0, sizeof(struct netroute_item_ls_s));
					
  struct rtmsg *rt;
  struct rtattr *attr;
  struct in_addr in;
  int rtlen;
  rt = (struct rtmsg *)NLMSG_DATA(nlhdr);
  /* If the route is not for AF_INET or does not belong to main routing table
     then return. */
  if((rt->rtm_family != AF_INET) || (rt->rtm_table != RT_TABLE_MAIN))
    return;
  
  strcpy(item.src, NETROUTE_IP4_ALL);
  strcpy(item.dst, NETROUTE_IP4_ALL);
  strcpy(item.gateway, NETROUTE_IP4_ALL);
  /* get the rtattr field */
  attr = (struct rtattr *)RTM_RTA(rt);
  rtlen = RTM_PAYLOAD(nlhdr);
  for(;RTA_OK(attr,rtlen);attr = RTA_NEXT(attr,rtlen)) {
    switch(attr->rta_type) {
      case RTA_OIF:
	if_indextoname(*(int *)RTA_DATA(attr), item.iface);
	break;
      case RTA_GATEWAY:
	in.s_addr = *(u_int *)RTA_DATA(attr);
	strcpy(item.gateway, (char *)nettools_net_to_str_ip(in));
	break;
      case RTA_PREFSRC:
	in.s_addr = *(u_int *)RTA_DATA(attr);
	strcpy(item.src, (char *)nettools_net_to_str_ip(in));
	break;
      case RTA_DST:
	in.s_addr = *(u_int *)RTA_DATA(attr);
	strcpy(item.dst, (char *)nettools_net_to_str_ip(in));
	break;
    }
  }
  if(strlen(item.iface))
    (*list) = llist_pushback_and_alloc(*list, &item, sizeof(struct netroute_item_ls_s));
}

/**
*******************************************************************************
* @file nettsocket.c
* @author Keidan
* @date 26/11/2013
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
#include <tk/io/net/netsocket.h>
#include <tk/io/net/nettools.h>
#include <tk/sys/log.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/tcp.h>


#define NETSOCKET_MAGIC 0xC00750c
#define create_ptr(local, param) struct netsocket_s *local = (struct netsocket_s*)param
#define test_ptr(ptr) (ptr && ptr->magic == NETSOCKET_MAGIC)

struct netsocket_s {
    __u32 magic;
    int fd;
    netsocket_mode_et mode;
    struct netsocket_inet_s inet;
};


/**
 * @fn int netsocket_get_fd(netsocket_t sock)
 * @brief Get the socket fd.
 * @param sock The socket associated with the fd.
 * @return the FD else -1 on error.
 */
int netsocket_get_fd(netsocket_t sock) {
  create_ptr(s, sock);
  if(!test_ptr(s)) return -1;
  return s->fd;
}


/**
 * @fn netsocket_t netsocket_new(struct netsocket_inet_s inet, netsocket_mode_et mode)
 * @brief Create a new socket.
 * @param inet The inet address.
 * @param mode The socket mode.
 * @return A netsocket_t instance else NULL on error.
 */
netsocket_t netsocket_new(struct netsocket_inet_s inet, netsocket_mode_et mode) {
  struct netsocket_s *ns = NULL;
  if((ns = malloc(sizeof(struct netsocket_s))) == NULL) {
    logger(LOG_ERR, "Unable to allocate a memory.\n");
    return NULL;
  }
  memset(ns, 0, sizeof(struct netsocket_s));
  ns->magic = NETSOCKET_MAGIC;
  ns->fd = 0;
  ns->mode = mode;
  memcpy(&ns->inet, &inet, sizeof(struct netsocket_inet_s));
  return ns;
}

/**
 * @fn void netsocket_delete(netsocket_t sock)
 * @brief close the socket
 * @param sock The socket to close.
 */
void netsocket_delete(netsocket_t sock) {
  create_ptr(s, sock);
  if(!test_ptr(s)) return;
  if(s) {
    netsocket_close(sock);
    free(s);
  }
}

/**
 * @fn void netsocket_close(netsocket_t sock)
 * @brief close the socket
 * @param sock The socket to close.
 */
void netsocket_close(netsocket_t sock) {
  create_ptr(s, sock);
  if(!test_ptr(s)) return;
  if(s) {
    if(s->fd > 0) close(s->fd), s->fd = 0;
  }
}

/**
 * @fn int netsocket_connect(netsocket_t sock)
 * @brief Connect a connected mode socket (see NETSOCKET_MODE_TCP)
 * @param sock The socket to connect.
 * @return -1 on error else 0
 */
int netsocket_connect(netsocket_t sock) {
  create_ptr(s, sock);
  if(!test_ptr(s)) return -1;
  struct sockaddr_in serv_addr;
  int on = 1;
  if(s->mode == NETSOCKET_MODE_TCP)
    s->fd = socket(AF_INET, SOCK_STREAM, 0);
  else return -1;
  if(s->fd < 0) {
    logger(LOG_ERR, "socket failed: (%d) %s.\n", errno, strerror(errno));
    return -1;
  }
  if(setsockopt(s->fd, IPPROTO_TCP, TCP_NODELAY, (char*) &on, sizeof(int))<0)  {
    logger(LOG_ERR, "setsockopt: (%d) %s.\n", errno, strerror(errno));
    return -1;
  }
  bzero((char *) &serv_addr, sizeof(struct sockaddr_in));
  if(nettools_ip_to_sockaddr(s->inet.host, &serv_addr) == -1)
    return -1;
  serv_addr.sin_port = htons(s->inet.port);
  if (connect(s->fd, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in)) < 0) {
    logger(LOG_ERR, "connect: (%d) %s.\n", errno, strerror(errno));
    return -1;
  }
  return 0;
}

/**
 * @fn int netsocket_bind(netsocket_t sock)
 * @brief Bind a socket.
 * @param sock The socket to bind.
 * @return -1 on error else 0
 */
int netsocket_bind(netsocket_t sock) {
  create_ptr(s, sock);
  if(!test_ptr(s)) return -1;
  struct sockaddr_in addr;
  int ling;

  if(s->mode == NETSOCKET_MODE_RAW)
    s->fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  else if(s->mode == NETSOCKET_MODE_UDP)
    s->fd = socket(AF_INET, SOCK_DGRAM, 0);
  else 
    s->fd = socket(AF_INET, SOCK_STREAM, 0);
  if(s->fd < 0) {
    logger(LOG_ERR, "socket failed: (%d) %s.\n", errno, strerror(errno));
    return -1;
  }
  /* set reuseaddr*/
  ling = 1;
  if (setsockopt(s->fd, SOL_SOCKET, SO_REUSEADDR, (char *)&ling, sizeof(ling)) != 0) {
    logger(LOG_ERR, "setsockopt: (%d) %s.\n", errno, strerror(errno));
    return -1;
  }
  bzero((char *)&addr, sizeof(addr));
  addr.sin_port = htons(s->inet.port);
  if (!strcmp(s->inet.host, NETSOCKET_ALL_IP))
    addr.sin_addr.s_addr = INADDR_ANY;
  else {
    if(nettools_ip_to_sockaddr(s->inet.host, &addr) == -1)
      return -1;
  }
  if (bind(s->fd, (struct sockaddr *)&addr, sizeof(addr))==-1) {
    logger(LOG_ERR, "bind: (%d) %s.\n", errno, strerror(errno));
    return -1;
  }
  return 0;
}

/**
 * @fn int netsocket_read(netsocket_t sock, const struct consnetsocket_packet_s *packet)
 * @brief Read a packet fro the sockedf
 * @param sock The socket to read.
 * @param packet The packet to write.
 * @return -1 on error (see errno) else 0
 */
int netsocket_write(netsocket_t sock, const struct netsocket_packet_s *packet) {
  create_ptr(s, sock);
  if(!test_ptr(s)) return -1;
  if(s->mode == NETSOCKET_MODE_UDP || s->mode == NETSOCKET_MODE_RAW) {
    struct sockaddr_in addr;
    bzero((char *)&addr, sizeof(addr));
    addr.sin_port = htons(s->inet.port);
    if (!strcmp(s->inet.host, NETSOCKET_ALL_IP))
      addr.sin_addr.s_addr = INADDR_ANY;
    else {
      if(nettools_ip_to_sockaddr(s->inet.host, &addr) == -1)
	return -1;
    }
    // sendto
    return sendto(s->fd, packet->buffer, packet->length, 0, (const struct sockaddr*)&addr, sizeof(addr));
  } else {
    // write
    return write(s->fd, packet->buffer, packet->length);
  }
}

/**
 * @fn netsocket_t netsocket_accept(netsocket_t sock)
 * @brief Accept a new connection.
 * @param sock The server socket.
 * @return The new connection else NULL on error.
 */
netsocket_t netsocket_accept(netsocket_t sock) {
 create_ptr(s, sock);
  if(!test_ptr(s)) return NULL;
  struct sockaddr_in cli_addr;
  socklen_t clilen = sizeof(cli_addr);
  int fd = accept(s->fd, (struct sockaddr *) &cli_addr, &clilen);
  if (fd < 0) {
    logger(LOG_ERR, "accept: (%d) %s.\n", errno, strerror(errno));
    return NULL;
  }
  struct netsocket_inet_s inet;
  bzero(&inet, sizeof(struct netsocket_s));
  strcpy(inet.host, inet_ntoa(cli_addr.sin_addr));
  inet.port = cli_addr.sin_port;
  struct netsocket_s *nsclient = (struct netsocket_s*)netsocket_new(inet, s->mode);
  nsclient->fd = fd;
  return nsclient;
}

/**
 * @fn int netsocket_read(netsocket_t sock, struct netsocket_packet_s *packet)
 * @brief Read a packet fro the sockfd
 * @param sock The socket to read.
 * @param packet The packet to read (packet->buffer required free).
 * @return -1 on error (see errno) else 0
 */
int netsocket_read(netsocket_t sock, struct netsocket_packet_s *packet) {
  create_ptr(s, sock);
  if(!test_ptr(s)) return -1;
  __u32 av = nettools_datas_available(s->fd);
  if(!av) {
    logger(LOG_ERR, "No data available!\n");
    return -1;
  }
  memset(packet, 0, sizeof(struct netsocket_s));
  packet->length = av;
  packet->buffer = malloc(packet->length);
  if(!packet->buffer) {
    packet->length = 0;
    logger(LOG_ERR, "Unable to allocate a memory for the packet datas");
    return -1;
  }
  memset(packet->buffer, 0, packet->length);
  if(s->mode == NETSOCKET_MODE_UDP || s->mode == NETSOCKET_MODE_RAW) {
    struct sockaddr_in addr;
    bzero((char *)&addr, sizeof(addr));
    addr.sin_port = htons(s->inet.port);
    if (!strcmp(s->inet.host, NETSOCKET_ALL_IP))
      addr.sin_addr.s_addr = INADDR_ANY;
    else {
      if(nettools_ip_to_sockaddr(s->inet.host, &addr) == -1)
	return -1;
    }
    // recvfrom
    return sendto(s->fd, packet->buffer, packet->length, 0, (const struct sockaddr*)&addr, sizeof(addr));
  } else {
    return recv(s->fd, packet->buffer, packet->length, 0);
  }
}

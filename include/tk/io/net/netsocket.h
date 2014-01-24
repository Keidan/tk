/**
 *******************************************************************************
 * @file nettsocket.h
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
#ifndef __NETSOCKET_H__
  #define __NETSOCKET_H__

  #include <tk/io/net/netiface.h>

  #define NETSOCKET_ALL_IP "*"

  struct netsocket_inet_s {
      char host[255];
      int port;
  };
  
  struct netsocket_packet_s {
      unsigned char* buffer;
      unsigned int length;
  };

  typedef enum { 
    NETSOCKET_MODE_TCP,
    NETSOCKET_MODE_UDP,
    NETSOCKET_MODE_RAW,
  } netsocket_mode_et;

  typedef void* netsocket_t;

  /**
   * @fn netsocket_t netsocket_new0()
   * @brief Create a new socket.
   * @return A netsocket_t instance else NULL on error.
   */
  netsocket_t netsocket_new0();

  /**
   * @fn netsocket_t netsocket_new(struct netsocket_inet_s inet, netsocket_mode_et mode)
   * @brief Create a new socket.
   * @param inet The inet address.
   * @param mode The socket mode.
   * @return A netsocket_t instance else NULL on error.
   */
  netsocket_t netsocket_new(struct netsocket_inet_s inet, netsocket_mode_et mode);

  /**
   * @fn void netsocket_delete(netsocket_t sock)
   * @brief Delete the socket
   * @param sock The socket to delete.
   */
  void netsocket_delete(netsocket_t sock);
  /**
   * @fn int netsocket_open(netsocket_t sock)
   * @brief open the socket
   * @param sock The socket to open.
   * @return -1 on error else 0
   */
  int netsocket_open(netsocket_t sock);

  /**
   * @fn void netsocket_close(netsocket_t sock)
   * @brief close the socket
   * @param sock The socket to close.
   */
  void netsocket_close(netsocket_t sock);

  /**
   * @fn int netsocket_get_fd(netsocket_t sock)
   * @brief Get the socket fd.
   * @param sock The socket associated with the fd.
   * @return the FD else -1 on error.
   */
  int netsocket_get_fd(netsocket_t sock);

  /**
   * @fn void netsocket_set_fd(netsocket_t sock, int fd)
   * @brief Set the socket fd.
   * @param sock The socket associated with the fd.
   * @param fd the new FD
   */
  void netsocket_set_fd(netsocket_t sock, int fd);

  /**
   * @fn int netsocket_connect(netsocket_t sock)
   * @brief Connect a connected mode socket (see NETSOCKET_MODE_TCP)
   * @param sock The socket to connect.
   * @return -1 on error else 0
   */
  int netsocket_connect(netsocket_t sock);

  /**
   * @fn int netsocket_bind(netsocket_t sock)
   * @brief Bind a socket.
   * @param sock The socket to bind.
   * @return -1 on error else 0
   */
  int netsocket_bind(netsocket_t sock);

  /**
   * @fn netsocket_t netsocket_accept(netsocket_t sock)
   * @brief Accept a new connection.
   * @param sock The server socket.
   * @return The new connection else NULL on error.
   */
  netsocket_t netsocket_accept(netsocket_t sock);

  /**
   * @fn int netsocket_read(netsocket_t sock, struct netsocket_packet_s *packet)
   * @brief Read a packet fro the sockfd
   * @param sock The socket to read.
   * @param packet The packet to read (packet->buffer required free).
   * @return -1 on error (see errno) else 0
   */
  int netsocket_read(netsocket_t sock, struct netsocket_packet_s *packet);

  /**
   * @fn int netsocket_read(netsocket_t sock, const struct consnetsocket_packet_s *packet)
   * @brief Read a packet fro the sockedf
   * @param sock The socket to read.
   * @param packet The packet to write.
   * @return -1 on error (see errno) else 0
   */
  int netsocket_write(netsocket_t sock, const struct netsocket_packet_s *packet);

#endif /* __NETSOCKET_H__ */

/**
*******************************************************************************
* @file ping.h
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
#ifndef __PING_H__
  #define __PING_H__

  #include <stdint.h>

  typedef void* ping_t;

  typedef enum {
    PING_RESULT_READ_ERROR,
    PING_RESULT_NON_ICMP,
    PING_RESULT_NOT_DNS_ORIGIN,
    PING_RESULT_NOT_REPLY,
    PING_RESULT_NON_EXPECTED_FRAME,
    PING_RESULT_TIMEOUT,
    PING_RESULT_SUCCESS,
  } ping_result_et;

  struct ping_event_data_s {
      ping_result_et result;
      uint16_t seq;
      char* host;
      char* ip;
      uint32_t timestamp;
      void* user_data;
  };
  #define evd(result, seq, host, ip, timestamp, user_data) ((struct ping_event_data_s){result, seq, host, ip, timestamp, user_data})
 

  typedef void (*ping_event_handler_fct)(ping_t p, struct ping_event_data_s data);


  /**
   * @fn ping_t ping_new(const char* iface)
   * @brief Open an PING connection.
   * @param iface The iface name to use.
   * @return The PING context else NULL on error.
   */
  ping_t ping_new(const char *iface);

  /**
   * @fn void ping_delete(ping_t ping)
   * @brief release the ping context.
   * @param ping The context.
   */
  void ping_delete(ping_t ping);


  /**
   * @fn void ping_set_event_handler(ping_t ping, ping_event_handler_fct evt, void* user_data)
   * @brief release the ping context.
   * @param ping The context.
   * @param evt The handler.
   * @param user_data The user data passed to the handler.
   */
  void ping_set_event_handler(ping_t ping, ping_event_handler_fct evt, void* user_data);

  /**
   * @fn int ping_start(ping_t ping, const char* host, uint32_t delay)
   * @brief Start the ping process.
   * @param ping The ping context.
   * @param host The host to ping.
   * @param delay the timeout delay (in s).
   * @return -1 on error else 0.
   */
  int ping_start(ping_t ping, const char* host, uint32_t delay);

  /**
   * @fn void ping_stop(ping_t ping)
   * @brief stop the ping context.
   * @param ping The context.
   */
  void ping_stop(ping_t ping);

#endif /* __PING_H__ */

/**
 *******************************************************************************
 * @file log.h
 * @author Keidan
 * @date 03/04/2013
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
#ifndef __LOG_H__
  #define __LOG_H__


  #include <libgen.h>
  #include <syslog.h>
  #ifndef MAX_MSG_SIZE
    #define MAX_MSG_SIZE 8192
  #endif

  #define __LOG_FILE__                   basename(__FILE__)
  #define __TMP_LOG__(prio, ...)         log_print(prio, __VA_ARGS__)

  #define logger(prio, fmt, ...)      __TMP_LOG__(prio, "[%s::%s(%d) -> " fmt, __LOG_FILE__, __func__, __LINE__, ##__VA_ARGS__)

  #define log_init_cast(i, o, f) (&((struct log_s){i, o, f}))
  struct log_s {
      char* ident;
      int option;
      int facility;
  };

  /**
   * @fn void log_init(struct log_s init)
   * @brief Open the syslog session.
   * @param init Log initializer.
   */
  void log_init(struct log_s init);

  /**
   * @fn void log_close()
   * @brief Close the syslog session.
   */
  void log_close();

  /**
   * @fn void log_print(int prio, const char* fmt, ...)
   * @brief Log a new message.
   * @param prio Log priority.
   * @param fmt Format/message.
   */
  void log_print(int prio, const char* fmt, ...);

#endif /* __LOG_H__ */

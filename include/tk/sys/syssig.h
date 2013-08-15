/**
 *******************************************************************************
 * @file syssig.h
 * @author Keidan
 * @date 14/08/2013
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
#ifndef __SYSSIG_H__
  #define __SYSSIG_H__

  #include <tk/sys/log.h>
  #include <signal.h>

  /**
   * @typedef void (*syssig_signal_fct)(int sig)
   * @brief signal catch callback.
   * @param sig The signal
   */
  typedef void (*syssig_signal_fct)(int sig);

  /**
   * @typedef void (*syssig_exit_fct)(void)
   * @brief Exit callback.
   * @param sig The signal
   */
  typedef void (*syssig_exit_fct)(void);

  /**
   * @fn void syssig_init(const struct log_s *linit, syssig_exit_fct exit_catch)
   * @brief Add exit callback action and start the syslog managment.
   * @param linit Start syslog (if not NULL)
   * @param exit_catch The signal callback.
   */
  void syssig_init(const struct log_s *linit, syssig_exit_fct exit_catch);

  /**
   * @fn void syssig_add_signal(int signal, syssig_signal_fct signal_catch)
   * @brief Add a new signal callback.
   * @param signal The signal to add.
   * @param signal_catch The callback.
   */
  void syssig_add_signal(int signal, syssig_signal_fct signal_catch);

  /**
   * @fn void syssig_remove_signal(int signal)
   * @brief Remove an added signal callback.
   * @param signal The signal to remove
   */
  void syssig_remove_signal(int signal);

#endif /* __SYSSIG_H__ */

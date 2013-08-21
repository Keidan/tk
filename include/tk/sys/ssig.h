/**
 *******************************************************************************
 * @file ssig.h
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
#ifndef __SSIG_H__
  #define __SSIG_H__

  #include <tk/sys/log.h>
  #include <signal.h>

  #ifndef SSIG_MAX_SIGNALS
    #define SSIG_MAX_SIGNALS 50
  #endif /* SSIG_MAX_SIGNALS */

  /**
   * @typedef void (*ssig_signal_fct)(int sig)
   * @brief signal catch callback.
   * @param sig The signal
   */
  typedef void (*ssig_signal_fct)(int sig);

  /**
   * @typedef void (*ssig_exit_fct)(void)
   * @brief Exit callback.
   * @param sig The signal
   */
  typedef void (*ssig_exit_fct)(void);

  /**
   * @fn void ssig_init(const struct log_s *linit, ssig_exit_fct exit_catch)
   * @brief Add exit callback action and start the syslog managment.
   * @param linit Start syslog (if not NULL)
   * @param exit_catch The signal callback.
   */
  void ssig_init(const struct log_s *linit, ssig_exit_fct exit_catch);

  /**
   * @fn void ssig_add_signal(int signal, ssig_signal_fct signal_catch)
   * @brief Add a new signal callback.
   * @param signal The signal to add.
   * @param signal_catch The callback.
   */
  void ssig_add_signal(int signal, ssig_signal_fct signal_catch);

  /**
   * @fn void ssig_remove_signal(int signal)
   * @brief Remove an added signal callback.
   * @param signal The signal to remove
   */
  void ssig_remove_signal(int signal);

#endif /* __SSIG_H__ */

/**
*******************************************************************************
* @file syssig.c
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
#include <tk/sys/syssig.h>
#include <tk/utils/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct sigctx_s {
    syssig_exit_fct       exit;
    _Bool                 log;
    struct sigaction      sa;
    syssig_signal_fct     signals[SYSSIG_MAX_SIGNALS];
};

static struct sigctx_s g_sigctx;

static void syssig_sig(int sig) {
  if(sig < 0 || sig > SYSSIG_MAX_SIGNALS) {
    fprintf(stderr, "Invalid signal value (%d)!!!\n", sig);
    return;
  }
  if(g_sigctx.signals[sig])
    g_sigctx.signals[sig](sig);
}

static void syssig_atexit(void) { 
  if(g_sigctx.exit) g_sigctx.exit();
  if(g_sigctx.log) log_close();
  memset(g_sigctx.signals, 0, sizeof(syssig_signal_fct)*SYSSIG_MAX_SIGNALS);
  g_sigctx.exit = NULL;
  g_sigctx.log = 0;
}


/**
 * @fn void syssig_init(const struct log_s *linit, syssig_exit_fct exit_catch)
 * @brief Add exit callback action and start the syslog managment.
 * @param linit Start syslog (if not NULL)
 * @param exit_catch The signal callback.
 */
void syssig_init(const struct log_s *linit, syssig_exit_fct exit_catch){
  memset(g_sigctx.signals, 0, sizeof(syssig_signal_fct)*SYSSIG_MAX_SIGNALS);
  g_sigctx.exit = exit_catch;
  g_sigctx.log = 0;
  if(linit) log_init(*linit), g_sigctx.log = 1;

  atexit(syssig_atexit);
  memset(&g_sigctx.sa, 0, sizeof(struct sigaction));
  g_sigctx.sa.sa_handler = syssig_sig;
//  (void)sigaction(SIGINT, &sa, NULL);
//  (void)sigaction(SIGTERM, &sa, NULL);
}

/**
 * @fn void syssig_add_signal(int signal, syssig_signal_fct signal_catch)
 * @brief Add a new signal callback.
 * @param signal The signal to add.
 * @param signal_catch The callback.
 */
void syssig_add_signal(int signal, syssig_signal_fct signal_catch) {
  if(signal < 0 || signal > SYSSIG_MAX_SIGNALS) return;
  g_sigctx.signals[signal] = signal_catch;
  (void)sigaction(signal, &g_sigctx.sa, NULL);
}

/**
 * @fn void syssig_remove_signal(int signal)
 * @brief Remove an added signal callback.
 * @param signal The signal to remove
 */
void syssig_remove_signal(int signal) {
  if(signal < 0 || signal > SYSSIG_MAX_SIGNALS) return;
  g_sigctx.signals[signal] = NULL;
}

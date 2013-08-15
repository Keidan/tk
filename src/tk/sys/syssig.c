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
#include <tk/collection/htable.h>
#include <tk/text/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct sigctx_s {
    syssig_exit_fct       exit;
    _Bool                 log;
    struct sigaction      sa;
    htable_t              table;
};

static struct sigctx_s g_sigctx;

static void syssig_sig(int sig) {
  syssig_signal_fct signal_catch = htable_lookup(g_sigctx.table, (char*)string_convert(sig, 10));
  if(signal_catch) signal_catch(sig);
}

static void syssig_atexit(void) { 
  if(g_sigctx.exit) g_sigctx.exit();
  if(g_sigctx.log) log_close();
  if(g_sigctx.table) 
    htable_clear(g_sigctx.table);
  g_sigctx.exit = NULL;
  g_sigctx.log = 0;
  g_sigctx.table = NULL;
}


/**
 * @fn void syssig_init(const struct log_s *linit, syssig_exit_fct exit_catch)
 * @brief Add exit callback action and start the syslog managment.
 * @param linit Start syslog (if not NULL)
 * @param exit_catch The signal callback.
 */
void syssig_init(const struct log_s *linit, syssig_exit_fct exit_catch){
  g_sigctx.table = htable_new();
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
  if(!g_sigctx.table) return;
  htable_add(g_sigctx.table, (char*)string_convert(signal, 10), &signal_catch, sizeof(syssig_signal_fct));
  (void)sigaction(signal, &g_sigctx.sa, NULL);
}

/**
 * @fn void syssig_remove_signal(int signal)
 * @brief Remove an added signal callback.
 * @param signal The signal to remove
 */
void syssig_remove_signal(int signal) {
  if(!g_sigctx.table) return;
  if(!htable_remove(g_sigctx.table, (char*)string_convert(signal, 10)))
     (void)sigaction(signal, NULL, NULL);
}

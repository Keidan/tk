/**
 *******************************************************************************
 * @file log.c
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
#include <tk/sys/log.h>
#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>


/**
 * Fonction permetant l'ouverture des logs syslog.
 * @param ident Identifiant utilise pour les logs.
 * @param option options syslogs.
 * @param facility Facility syslog.
 */
void log_init(const char* ident, int option, int facility) {
  openlog (ident, option, facility);
}

/**
 * Fonction permetant la fermeture des logs syslog.
 */
void log_close() {
  closelog();
}

/**
 * Fonction permetant de logger un message.
 * @param prio Prio du log.
 * @param fmt Format/message.
 */
void log_print(int prio, const char* fmt, ...) {
  va_list args;
  char msg[MAX_MSG_SIZE] = {0};
  va_start(args, fmt);
  vsnprintf(msg, MAX_MSG_SIZE, fmt, args);
  va_end(args);
  syslog(prio, "%s", msg);
}

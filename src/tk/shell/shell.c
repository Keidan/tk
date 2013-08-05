/**
 *******************************************************************************
 * @file shell.c
 * @author Keidan
 * @date 05/08/2013
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
#include <tk/shell/shell.h>
#include <tk/sys/log.h>
#include <stdlib.h>
#include <string.h>

struct shell_s {
    shell_term_t term;
    shell_buffer_t buffer;
};


/**
 * @fn shell_t shell_new()
 * @brief Allocation of a new shell.
 * @return The shell else NULL on error.
 */
shell_t shell_new() {
  struct shell_s *s = malloc(sizeof(struct shell_s));
  if(!s) {
    logger(LOG_ERR, "SHELL: Not enough memory.\n");
    return NULL;
  }
  memset(s, 0, sizeof(struct shell_s));
  if((s->term = shell_term_new()) == NULL) {
    shell_delete(s);
    logger(LOG_ERR, "SHELL: Unable to allocate a new terminal.\n");
    return NULL;
  }
  if((s->buffer = shell_buffer_new()) == NULL) {
    shell_delete(s);
    logger(LOG_ERR, "SHELL: Unable to allocate a new buffer.\n");
    return NULL;
  }
  return s;
}

/**
 * @fn void shell_delete(shell_t shell)
 * @brief Delete an allocated shell.
 * @param shell The shell.
 */
void shell_delete(shell_t shell) {
  struct shell_s *s = (struct shell_s*)shell;
  if(s) {
    if(s->term)
      shell_term_delete(s->term), s->term = NULL;
    if(s->buffer)
      shell_buffer_delete(s->buffer), s->buffer = NULL;
    free(s);
  }
}

/**
 * @fn shell_term_t shell_get_term(shell_t shell)
 * @brief Get the associated terminal.
 * @param shell The shell.
 * @return The term.
 */
shell_term_t shell_get_term(shell_t shell) {
  struct shell_s *s = (struct shell_s*)shell;
  if(!s) return NULL;
  return s->term;
}

/**
 *******************************************************************************
 * @file shell_buffer.c
 * @author Keidan
 * @date 03/08/2013
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
#include <tk/shell/shell_buffer.h>
#include <tk/sys/log.h>
#include <stdlib.h>
#include <string.h>
#include <tk/text/string.h>

struct shell_buffer_s {
    char* str;
    uint32_t length;
};

/**
 * @fn shell_term_t shell_buffer_new()
 * @brief New shell buffer.
 */
shell_buffer_t shell_buffer_new() {
  struct shell_buffer_s *b = malloc(sizeof(struct shell_buffer_s));
  if(!b) {
    logger(LOG_ERR, "SHELL_BUFFER: Not enough memory.\n");
    return NULL;
  }
  memset(b, 0, sizeof(struct shell_buffer_s));
  return b;
}

/**
 * @fn void shell_buffer_delete(shell_buffer_t buffer)
 * @brief Delete shell buffer.
 * @param buffer The buffer pointer.
 */
void shell_buffer_delete(shell_buffer_t buffer) {
  struct shell_buffer_s *b = (struct shell_buffer_s*) buffer;
  if(b) {
    if(b->str) free(b->str), b->str = NULL;
    free(b);
  }
}

/**
 * @fn void shell_buffer_clear(shell_buffer_t buffer)
 * @brief Clear the buffer.
 * @param buffer The buffer pointer.
 */
void shell_buffer_clear(shell_buffer_t buffer) {
}

/**
 *******************************************************************************
 * @file process.c
 * @author Keidan
 * @date 27/11/2013
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
#include <tk/sys/process.h>
#include <tk/sys/log.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


/**
 * @fn stringbuffer_t process_execute(char* fmt, ...)
 * @brief Execute a command in a pipe.
 * @param fmt The command to execute.
 * @param ... The command args.
 * @return The result else NULL on error (for non NULL returns stringbuffer_delete is required).
 */
stringbuffer_t process_execute(char* fmt, ...) {
  va_list args;
  stringbuffer_t b = stringbuffer_new();
  va_start(args, fmt);
  stringbuffer_vprintf(b, fmt, args);
  va_end(args);
  stringbuffer_append(b, " 2>&1");

  FILE *fpipe;
  if ( !(fpipe = (FILE*)popen(stringbuffer_to_str(b),"r")) ) {  // If fpipe is NULL
    logger(LOG_ERR, "Unable to execute the command '%s': (%d) %s\n", stringbuffer_to_str(b), errno, strerror(errno));
    stringbuffer_delete(b);
    return NULL;
  }
  stringbuffer_delete(b);
  stringbuffer_t result = stringbuffer_new();
  char* line = NULL;
  size_t sz = 0;
  while(getline(&line, &sz, fpipe) != -1)
    stringbuffer_append(result, line);
  if(line) free(line);
  stringbuffer_trim_to_size(result);
  pclose(fpipe);
  return result;
}


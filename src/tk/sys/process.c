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
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

/**
 * @fn void process_exec_p(struct pstream_s *stream, char* fpath, char* args)
 * @brief Execute a piped application an retrieved the IO stream.
 * @param stream The result IO.
 * @param fpath The full script path.
 * @param args The script args.
 */
void process_exec_p(struct pstream_s *stream, char* fpath, char* args) {
  pid_t pid;
  int rpipes[2];
  int wpipes[2];
  bzero(stream, sizeof(struct pstream_s));
  pipe(rpipes);
  pipe(wpipes);

  pid = fork();
  stream->cpid = pid;
  if (pid == 0) {
    /* child */
    close(wpipes[1]);
    close(rpipes[0]);
    dup2(wpipes[0], STDIN_FILENO);
    dup2(rpipes[1], STDOUT_FILENO);
    dup2(rpipes[1], STDERR_FILENO);
    close(wpipes[0]);
    close(rpipes[1]);

    execl(fpath, fpath, args, (char*) NULL);
    exit(127);
  }
  close(wpipes[0]);
  close(rpipes[1]);
  stream->fd_in = wpipes[1];
  stream->fd_out = rpipes[0];
}

/**
 * @fn void process_kill_p(struct pstream_s *stream)
 * @brief Kill a pending process executed with process_p functions.
 * @param stream The strea input.
 */
void process_kill_p(struct pstream_s *stream) {
  kill(-stream->cpid, SIGTERM);
  sleep(2);
  kill(-stream->cpid, SIGKILL);
  bzero(stream, sizeof(struct pstream_s));
}


/**
 * @fn stringbuffer_t process_exec(char* fmt, ...)
 * @brief Execute a command in a pipe.
 * @param fmt The command to execute.
 * @param ... The command args.
 * @return The result else NULL on error (for non NULL returns stringbuffer_delete is required).
 */
stringbuffer_t process_exec(char* fmt, ...) {
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


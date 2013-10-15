/**
*******************************************************************************
* @file probe.c
* @author Keidan
* @date 11/10/2013
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
#include <tk/sys/probe.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define PROC_SYS_MODPROBE "/proc/sys/kernel/modprobe"

/**
 * @fn int probe_get_binary(file_name_t probe)
 * @brief Get th e real path of the modprobe application.
 * @param probe The result.
 * @return -1 on error else 0 on success.
*/
int probe_get_binary(file_name_t probe) {
  int procfile;
  file_name_t ret;
  memset(ret, 0, sizeof(file_name_t));
  procfile = open(PROC_SYS_MODPROBE, O_RDONLY);
  if (procfile < 0)
    return -1;
  switch (read(procfile, ret, sizeof(file_name_t))) {
    case -1:
    case sizeof(file_name_t):/* Partial read.  Wierd */
      close(procfile);
      return -1;
  }
  if (ret[strlen(ret) - 1] == '\n')
    ret[strlen(ret) - 1] = 0;
  close(procfile);
  return 0;
}


/**
 * @fn int probe_insert(const char *modname, const char *modprobe, _Bool quiet)
 * @brief Insert a kernel module.
 * @param modname The module name.
 * @param modprobe modprobe application path (probe_get_modprobe executed if NULL).
 * @param quiet Quiet mode.
 * @return -1 on error else 0 on success.
 */
int probe_insert(const char *modname, const char *modprobe, _Bool quiet) {
  file_name_t buf;
  char *argv[4];
  int *status = malloc(sizeof(int));
  /* If they don't explicitly set it, read out of kernel */
  if (modprobe == NULL) {
    if(probe_get_binary(buf) == -1);
      return -1;
    modprobe = buf;
  }

  /*
   * Need to flush the buffer, or the child may output it again
   * when switching the program thru execv.
   */
  fflush(stdout);

  switch (vfork()) {
    case 0:
      argv[0] = (char *)modprobe;
      argv[1] = (char *)modname;
      if (quiet) {
	argv[2] = (char*)"-q";
	argv[3] = NULL;
      } else {
	argv[2] = NULL;
	argv[3] = NULL;
      }
      execv(argv[0], argv);
      /* not usually reached */
      exit(1);
    case -1:
      return -1;  
    default: /* parent */
      wait(status);
      status = status;
  }
  
  if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
    free(status);
    return 0;
  }
  free(status);
  return -1;
}

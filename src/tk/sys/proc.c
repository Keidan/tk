/**
 *******************************************************************************
 * @file proc.c
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>       
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <tk/sys/proc.h>
#include <tk/sys/systools.h>
#include <tk/sys/log.h>

int proc_stat(struct proc_stat_s *stat, i_t pid) {
  FILE* file = NULL;
  size_t sz = 0, d = 0;
  char filename[FILENAME_MAX];
  char *buffer = 0;
  /* build the file name */
  systools_get_proc_filename(filename, pid, "stat");

  /* open the file */
  if((file = fopen(filename, "r")) == NULL) {
    logger(LOG_ERR, "fopen('%s') failed: (%d) %s!\n", filename, errno, strerror(errno));
    return -1;
  }

  d = getline(&buffer, &sz, file);
  d++; // for the warning .... ugly and useless for some gcc...
  fclose (file);
  memset(stat, 0, sizeof(struct proc_stat_s));
  sscanf(buffer, "%d %s %c %d %d %d %d %d %lu %lu %lu %lu %lu %llu %llu %llu %llu %d %d %d %d "
	 "%llu %lu %lu %lu %lu %lu %lu %lu %lu %lld %lld %lld %lld %lu %c %c %lld %d %lu %lu %lu %llu %llu",
	 &stat->pid, stat->name, &stat->state.st,
	 &stat->ppid, &stat->pgrp, &stat->session,
	 &stat->tty_nb, &stat->tpgid, &stat->flags,
	 &stat->minflt, &stat->cminflt, &stat->majflt,
	 &stat->cmajflt, &stat->utime, &stat->stime,
	 &stat->cutime, &stat->cstime, &stat->priority,
	 &stat->nice, &stat->nlwp, &stat->obsolete,
	 &stat->start_time, &stat->vsize, &stat->vm_rss,
	 &stat->rss_rlim, &stat->start_code, &stat->end_code,
	 &stat->start_stack, &stat->kstk_esp, &stat->kstk_eip,
	 &stat->signal, &stat->blocked, &stat->sigignore,
	 &stat->sigcatch, &stat->wchan, &stat->placeholder0,
	 &stat->placeholder1, &stat->sigpnd, &stat->processor,
	 &stat->rtprio, &stat->sched, &stat->twait,
	 &stat->gtime, &stat->gctime);
  switch(stat->state.st) {
    case 'D':
      strcpy(stat->state.desc, "Uninterruptible");
      break;
    case'R':
      strcpy(stat->state.desc, "Runnable");
      break;
    case'S':
      strcpy(stat->state.desc, "Sleeping");
      break;
    case'T':
      strcpy(stat->state.desc, "Stopped");
      break;
    case'X':
      strcpy(stat->state.desc, "Terminated");
      break;
    case'Z':
      strcpy(stat->state.desc, "Zombie");
      break;
    default:
      strcpy(stat->state.desc, "ERROR");
      break;
  }
  return 0;
}

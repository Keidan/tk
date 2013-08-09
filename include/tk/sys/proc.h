/**
 *******************************************************************************
 * @file proc.h
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
#ifndef __PROC_H__
  #define __PROC_H__

  #include <stdio.h>

  struct pstate_s {
      char st;
      char desc[16];
  };

  typedef unsigned long      lu_t;
  typedef unsigned long long llu_t;
  typedef long long          ll_t;
  typedef int                i_t;

  struct proc_stat_s {
      i_t                pid;                 /**< PID. */
      char               name[FILENAME_MAX];  /**< filename of the executable being run by the process, enclosed in parentheses. */
      struct pstate_s    state;               /**< process state as a single character (D/R/S/T/Z). */
      i_t                ppid;                /**< PID of the parent process. */
      i_t                pgrp;                /**< ID of the process group. */
      i_t                session;             /**< session ID. */
      i_t                tty_nb;              /**< number of the controlling terminal the process uses. */
      i_t                tpgid;               /**< ID of the process group that is in the foreground on the terminal. */
      lu_t               flags;               /**< task flags associated with the process, as a decimal number. */
      lu_t               minflt;              /**< number of minor page faults. */
      lu_t               cminflt;             /**< number of minor page faults, including those from child processes. */
      lu_t               majflt;              /**< number of major page faults. */
      lu_t               cmajflt;             /**< number of major page faults, including those from child processes. */
      llu_t              utime;               /**< CPU time spent in user code, measured in jiffies. */
      llu_t              stime;               /**< CPU time spent in kernel code, measured in jiffies. */
      llu_t              cutime;              /**< CPU time spent in user code, including time from children. */
      llu_t              cstime;              /**< CPU time spent in kernel code, including time from children. */
      i_t                priority;            /**< priority. */
      i_t                nice;                /**< niceness. */ 
      i_t                nlwp;                /**< number of threads. */
      i_t                obsolete;            /**< obsolete field, always 0. */
      llu_t              start_time;          /**< time when the process started, measured in nanoseconds since the system boot. */
      lu_t               vsize;               /**< size of the process' virtual memory space (in pages). */
      lu_t               vm_rss;              /**< size of the process' resident set (in pages). */
      lu_t               rss_rlim;            /**< limit of the process' resident set size (in pages). */
      lu_t               start_code;          /**< start address of the program code. */
      lu_t               end_code;            /**< end address of the program code. */
      lu_t               start_stack;         /**< start address of the stack. */
      lu_t               kstk_esp;            /**< current value of the stack pointer. */
      lu_t               kstk_eip;            /**< current value of the instruction pointer. */
      ll_t               signal;              /**< bit mask of pending signals, as a decimal number. */
      ll_t               blocked;             /**< bit mask of blocked signals, as a decimal number. */
      ll_t               sigignore;           /**< bit mask of ignored signals, as a decimal number. */
      ll_t               sigcatch;            /**< bit mask of signals for which a handler is set, as a decimal number. */
      lu_t               wchan;               /**< address of a kernel function where the process currently waits (0 if the process is not waiting). */
      char               placeholder0;        /**< placeholder, always 0. */
      char               placeholder1;        /**< placeholder, always 0. */
      ll_t               sigpnd;              /**< number of signal to be sent to the parent when this process terminates. */
      i_t                processor;           /**< number of CPU the process is currently scheduled on. */
      lu_t               rtprio;              /**< realtime priority. */
      lu_t               sched;               /**< scheduling policy. */
      lu_t               twait;               /**< time the process has spent waiting for I/O. */
      llu_t              gtime;               /**< guest time, measured in jiffies. */
      llu_t              gctime;              /**< guest time of the children, measured in jiffies. */  
  };
  
  int proc_stat(struct proc_stat_s *stat, i_t pid);

#endif /* __PROC_H__ */

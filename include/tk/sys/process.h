/**
 *******************************************************************************
 * @file process.h
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
#ifndef __PROCESS_H__
  #define __PROCESS_H__

  #include <tk/io/file.h>
  #include <tk/utils/stringbuffer.h>

  struct pstream_s {
      int fd_in;
      int fd_out;
      int cpid;
  };

  struct process_netshell_s {
      int fd;
      file_name_t sh_path;
      file_name_t sh_name;
};

/**
 * @fn void process_exec_bash_over_socket(struct process_netshell_s cfg)
 * @brief Attach the socket to a new shell instance.
 * @param cfg The cofiguration.
 */
 void process_exec_bash_over_socket(struct process_netshell_s cfg);

  /**
   * @fn void process_exec_p(struct pstream_s *stream, char* fpath, char* args)
   * @brief Execute a piped application an retrieved the IO stream.
   * @param stream The result IO.
   * @param fpath The full script path.
   * @param args The script args.
   */
  void process_exec_p(struct pstream_s *stream, char* fpath, char* args);


  /**
   * @fn void process_kill_p(struct pstream_s *stream)
   * @brief Kill a pending process executed with process_p functions.
   * @param stream The strea input.
   */
  void process_kill_p(struct pstream_s *stream);

  /**
   * @fn stringbuffer_t process_exec(char* fmt, ...)
   * @brief Execute a command in a pipe.
   * @param fmt The command to execute.
   * @param ... The command args.
   * @return The result else NULL on error (for non NULL returns stringbuffer_delete is required).
   */
  stringbuffer_t process_exec(char* fmt, ...);

#endif /* __PROCESS_H__ */

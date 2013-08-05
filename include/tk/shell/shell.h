/**
 *******************************************************************************
 * @file shell.h
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
#ifndef __SHELL_H__
  #define __SHELL_H__

  #include <tk/shell/shell_buffer.h>
  #include <tk/shell/shell_term.h>

  typedef void* shell_t;

  /**
   * @fn shell_t shell_new()
   * @brief Allocation of a new shell.
   * @return The shell else NULL on error.
   */
  shell_t shell_new();

  /**
   * @fn void shell_delete(shell_t shell)
   * @brief Delete an allocated shell.
   * @param shell The shell.
   */
  void shell_delete(shell_t shell);

  /**
   * @fn shell_term_t shell_get_term(shell_t shell)
   * @brief Get the associated terminal.
   * @param shell The shell.
   * @return The term.
   */
  shell_term_t shell_get_term(shell_t shell);


#endif /* __SHELL_H__ */

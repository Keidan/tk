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

  #include <tk/text/stringbuffer.h>
  #include <tk/shell/shell_term.h>

  typedef void* shell_t;

  /**
   * @fn shell_t shell_new(_Bool icanon)
   * @brief Allocation of a new shell.
   * @param icanon Enable canonical read.
   * @return The shell else NULL on error.
   */
  shell_t shell_new(_Bool icanon);

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

  /**
   * @fn void shell_set_prompt(shell_t shell, const char* prompt)
   * @brief Update the prompt value.
   * @param shell The shell.
   * @param prompt The prompt value else NULL to erase it.
   */
  void shell_set_prompt(shell_t shell, const char* prompt);

  /**
   * @fn const char* shell_get_prompt(shell_t shell)
   * @brief Get the prompt value.
   * @param shell The shell.
   * @return The prompt value else NULL.
   */
  const char* shell_get_prompt(shell_t shell);

  /**
   * @fn int shell_exit_loop(shell_t shell)
   * @brief Kill the main loop
   * @param shell The shell.
   * @return -1 on error else 0.
   */
  int shell_exit_loop(shell_t shell);

  /**
   * @fn int shell_main_loop(shell_t shell)
   * @brief Entering into the main loop.
   * @param shell The shell.
   * @return -1 on error else 0.
   */
  int shell_main_loop(shell_t shell);

  /**
   * @fn void shell_print_prompt(shell_t shell)
   * @brief Print the prompt.
   * @param shell The shell.
   */
  void shell_print_prompt(shell_t shell);

#endif /* __SHELL_H__ */

/**
 *******************************************************************************
 * @file shell_buffer.h
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
#ifndef __SHELL_BUFFER_H__
  #define __SHELL_BUFFER_H__

  typedef void* shell_buffer_t;

  /**
   * @fn shell_term_t shell_buffer_new()
   * @brief New shell buffer.
   */
  shell_buffer_t shell_buffer_new();

  /**
   * @fn void shell_buffer_delete(shell_buffer_t buffer)
   * @brief Delete shell buffer.
   * @param buffer The buffer pointer.
   */
  void shell_buffer_delete(shell_buffer_t buffer);

  /**
   * @fn void shell_buffer_clear(shell_buffer_t buffer)
   * @brief Clear the buffer.
   * @param buffer The buffer pointer.
   */
  void shell_buffer_clear(shell_buffer_t buffer);

#endif /* __SHELL_BUFFER_H__ */

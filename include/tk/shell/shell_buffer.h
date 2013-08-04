#ifndef __SHELL_BUFFER_H__
  #define __SHELL_BUFFER_H__

  #include <stdarg.h>


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
   * @fn void shell_buffer_vappend(shell_buffer_t buffer, const char* format, va_list args)
   * @brief Append a new string to the buffer.
   * @param buffer The buffer pointer.
   * @param format The string format.
   * @param args The arguments.
   */
  void shell_buffer_vappend(shell_buffer_t buffer, const char* format, va_list args);

  /**
   * @fn void shell_buffer_clear(shell_buffer_t buffer)
   * @brief Clear the buffer.
   * @param buffer The buffer pointer.
   */
  void shell_buffer_clear(shell_buffer_t buffer);

#endif /* __SHELL_BUFFER_H__ */

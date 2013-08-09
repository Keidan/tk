/**
 *******************************************************************************
 * @file stringbuffer.h
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
#ifndef __STRINGBUFFER_H__
  #define __STRINGBUFFER_H__

  #include <stdint.h>

  typedef void* stringbuffer_t;

  /**
   * @fn shell_term_t stringbuffer_new()
   * @brief New shell buffer.
   */
  stringbuffer_t stringbuffer_new();

  /**
   * @fn void stringbuffer_delete(stringbuffer_t buffer)
   * @brief Delete shell buffer.
   * @param buffer The buffer pointer.
   */
  void stringbuffer_delete(stringbuffer_t buffer);

  /**
   * @fn void stringbuffer_clear(stringbuffer_t buffer)
   * @brief Clear the buffer.
   * @param buffer The buffer pointer.
   */
  void stringbuffer_clear(stringbuffer_t buffer);

  /**
   * @fn char* stringbuffer_to_str(stringbuffer_t buffer)
   * @brief Get the buffer to char* format.
   * @param buffer The buffer.
   * @return The char* (can be NULL)
   */
  char* stringbuffer_to_str(stringbuffer_t buffer);

  /**
   * @fn uint32_t stringbuffer_length(stringbuffer_t buffer)
   * @brief Get the buffer length.
   * @param buffer The buffer.
   * @return The length
   */
  uint32_t stringbuffer_length(stringbuffer_t buffer);

  /**
   * @fn uint32_t stringbuffer_capacity(stringbuffer_t buffer)
   * @brief Get the buffer capacity.
   * @param buffer The buffer.
   * @return The capacity
   */
  uint32_t stringbuffer_capacity(stringbuffer_t buffer);

  /**
   * @fn int stringbuffer_set_capacity(stringbuffer_t buffer, uint32_t capacity)
   * @brief Change the buffer capacity.
   * @param buffer The buffer.
   * @param capacity The buffer capacity.
   * @return -1 on error else 0.
   */
  int stringbuffer_set_capacity(stringbuffer_t buffer, uint32_t capacity);

  /**
   * @fn int stringbuffer_append(stringbuffer_t shell, const char* str)
   * @brief Append a string into the buffer.
   * @param buffer The buffer.
   * @param str The string to append.
   * @return -1 on error else 0.
   */
  int stringbuffer_append(stringbuffer_t buffer, const char* str);

  /**
   * @fn int stringbuffer_append_char(stringbuffer_t shell, const char c)
   * @brief Append a char into the buffer.
   * @param buffer The buffer.
   * @param c The char to append.
   * @return -1 on error else 0.
   */
  int stringbuffer_append_char(stringbuffer_t buffer, const char c);

  /**
   * @fn int stringbuffer_copy(stringbuffer_t shell, const char* str)
   * @brief Erase the buffer with the copy string.
   * @param buffer The buffer.
   * @param str The string to copy.
   * @return -1 on error else 0.
   */
  int stringbuffer_copy(stringbuffer_t buffer, const char* str);

  /**
   * @fn int stringbuffer_copy_char(stringbuffer_t shell, const char c)
   * @brief Erase the buffer with the copy char.
   * @param buffer The buffer.
   * @param c The char to copy.
   * @return -1 on error else 0.
   */
  int stringbuffer_copy_char(stringbuffer_t buffer, const char c);

  /**
   * @fn int stringbuffer_trim_to_size(stringbuffer_t buffer)
   * @brief Trim the buffer allocation size to the buffer size.
   * @param buffer The buffer.
   * @return -1 on error else 0.
   */
  int stringbuffer_trim_to_size(stringbuffer_t buffer);


  /**
   * @fn int stringbuffer_erase2(stringbuffer_t buffer, uint32_t index)
   * @brief Erase a string.
   * @param buffer The buffer.
   * @param index The index.
   * @return -1 on error else 0.
   */
  int stringbuffer_erase2(stringbuffer_t buffer, uint32_t index);

  /**
   * @fn int stringbuffer_insert(stringbuffer_t buffer, uint32_t index, char* str)
   * @brief Insert a string from a specific position.
   * @param buffer The buffer.
   * @param index The index.
   * @param str The str.
   * @return -1 on error else 0.
   */
  int stringbuffer_insert(stringbuffer_t buffer, uint32_t index, char* str);

  /**
   * @fn int stringbuffer_erase(stringbuffer_t buffer, uint32_t index, uint32_t length)
   * @brief Erase a string.
   * @param buffer The buffer.
   * @param index The start index.
   * @param length The length.
   * @return -1 on error else 0.
   */
  int stringbuffer_erase(stringbuffer_t buffer,uint32_t index, uint32_t length);

  /**
   * @fn int stringbuffer_printf(stringbuffer_t buffer, const char* fmt, ...)
   * @brief Simple printf into the stringbuffer.
   * @param buffer The buffer.
   * @param fmt The format.
   * @param ... The arguments.
   * @return -1 on error else 0.
   */
  int stringbuffer_printf(stringbuffer_t buffer, const char* fmt, ...);

#endif /* __STRINGBUFFER_H__ */

/**
 *******************************************************************************
 * @file bytebuffer.h
 * @author Keidan
 * @date 10/01/2014
 * @par Project
 * tk
 *
 * @par Copyright
 * Copyright 2011-2014 Keidan, all right reserved
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
#ifndef __BYTEBUFFER_H__
  #define __BYTEBUFFER_H__

  #include <stdint.h>
  #include <stdarg.h>

  typedef void* bytebuffer_t;

  /**
   * @fn buffer_term_t bytebuffer_new()
   * @brief New byte buffer.
   */
  bytebuffer_t bytebuffer_new();

  /**
   * @fn void bytebuffer_delete(bytebuffer_t buffer)
   * @brief Delete byte buffer.
   * @param buffer The buffer pointer.
   */
  void bytebuffer_delete(bytebuffer_t buffer);

  /**
   * @fn void bytebuffer_clear(bytebuffer_t buffer)
   * @brief Clear the buffer.
   * @param buffer The buffer pointer.
   */
  void bytebuffer_clear(bytebuffer_t buffer);

  /**
   * @fn char* bytebuffer_to_bytes(bytebuffer_t buffer)
   * @brief Get the buffer to char* format.
   * @param buffer The buffer.
   * @return The char* (can be NULL)
   */
  char* bytebuffer_to_bytes(bytebuffer_t buffer);

  /**
   * @fn uint32_t bytebuffer_length(bytebuffer_t buffer)
   * @brief Get the buffer length.
   * @param buffer The buffer.
   * @return The length
   */
  uint32_t bytebuffer_length(bytebuffer_t buffer);

  /**
   * @fn uint32_t bytebuffer_capacity(bytebuffer_t buffer)
   * @brief Get the buffer capacity.
   * @param buffer The buffer.
   * @return The capacity
   */
  uint32_t bytebuffer_capacity(bytebuffer_t buffer);

  /**
   * @fn int bytebuffer_set_capacity(bytebuffer_t buffer, uint32_t capacity)
   * @brief Change the buffer capacity.
   * @param buffer The buffer.
   * @param capacity The buffer capacity.
   * @return -1 on error else 0.
   */
  int bytebuffer_set_capacity(bytebuffer_t buffer, uint32_t capacity);

  /**
   * @fn int bytebuffer_append(bytebuffer_t buffer, const char* bytes, uint32_t bytes_length)
   * @brief Append a bytes into the buffer.
   * @param buffer The buffer.
   * @param bytes The bytes to append.
   * @param bytes_length The bytes length.
   * @return -1 on error else 0.
   */
  int bytebuffer_append(bytebuffer_t buffer, const char* bytes, uint32_t bytes_length);

  /**
   * @fn int bytebuffer_append_byte(bytebuffer_t buffer, const char c)
   * @brief Append a byte into the buffer.
   * @param buffer The buffer.
   * @param c The byte to append.
   * @return -1 on error else 0.
   */
  int bytebuffer_append_byte(bytebuffer_t buffer, const char c);

  /**
   * @fn int bytebuffer_copy(bytebuffer_t buffer, const char* bytes, uint32_t bytes_length)
   * @brief Erase the buffer with the copy bytes.
   * @param buffer The buffer.
   * @param bytes The bytes to copy.
   * @param bytes_length The bytes length.
   * @return -1 on error else 0.
   */
  int bytebuffer_copy(bytebuffer_t buffer, const char* bytes, uint32_t bytes_length);

  /**
   * @fn int bytebuffer_copy_byte(bytebuffer_t buffer, const char c)
   * @brief Erase the buffer with the copy char.
   * @param buffer The buffer.
   * @param c The byte to copy.
   * @return -1 on error else 0.
   */
  int bytebuffer_copy_byte(bytebuffer_t buffer, const char c);

  /**
   * @fn int bytebuffer_trim_to_size(bytebuffer_t buffer)
   * @brief Trim the buffer allocation size to the buffer size.
   * @param buffer The buffer.
   * @return -1 on error else 0.
   */
  int bytebuffer_trim_to_size(bytebuffer_t buffer);

#endif /* __BYTEBUFFER_H__ */

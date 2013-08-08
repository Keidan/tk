/**
 *******************************************************************************
 * @file stringbuffer.c
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
#include <tk/text/stringbuffer.h>
#include <tk/sys/log.h>
#include <stdlib.h>
#include <string.h>
#include <tk/text/string.h>

struct stringbuffer_s {
    char* str;
    uint32_t length;
    uint32_t alength;
};

/**
 * @fn shell_term_t stringbuffer_new()
 * @brief New shell buffer.
 */
stringbuffer_t stringbuffer_new() {
  struct stringbuffer_s *b = malloc(sizeof(struct stringbuffer_s));
  if(!b) {
    logger(LOG_ERR, "STRINGBUFFER: Not enough memory.\n");
    return NULL;
  }
  memset(b, 0, sizeof(struct stringbuffer_s));
  return b;
}

/**
 * @fn void stringbuffer_delete(stringbuffer_t buffer)
 * @brief Delete shell buffer.
 * @param buffer The buffer pointer.
 */
void stringbuffer_delete(stringbuffer_t buffer) {
  struct stringbuffer_s *b = (struct stringbuffer_s*) buffer;
  if(b) {
    if(b->str) free(b->str), b->str = NULL;
    b->length = b->alength = 0;
    free(b);
  }
}

/**
 * @fn void stringbuffer_clear(stringbuffer_t buffer)
 * @brief Clear the buffer.
 * @param buffer The buffer pointer.
 */
void stringbuffer_clear(stringbuffer_t buffer) {
  struct stringbuffer_s *b = (struct stringbuffer_s*) buffer;
  if(!b || !b->str) return;
  bzero(b->str, b->length);
  b->length = 0;
}

/**
 * @fn char* stringbuffer_to_str(stringbuffer_t buffer)
 * @brief Get the buffer to char* format.
 * @param buffer The buffer.
 * @return The char* (can be NULL)
 */
char* stringbuffer_to_str(stringbuffer_t buffer) {
  struct stringbuffer_s *b = (struct stringbuffer_s*) buffer;
  if(!b) return NULL;
  return b->str;
}

/**
 * @fn uint32_t stringbuffer_length(stringbuffer_t buffer)
 * @brief Get the buffer length.
 * @param buffer The buffer.
 * @return The length
 */
uint32_t stringbuffer_length(stringbuffer_t buffer) {
  struct stringbuffer_s *b = (struct stringbuffer_s*) buffer;
  if(!b) return 0;
  return b->length;
}

/**
 * @fn uint32_t stringbuffer_capacity(stringbuffer_t buffer)
 * @brief Get the buffer capacity.
 * @param buffer The buffer.
 * @return The capacity
 */
uint32_t stringbuffer_capacity(stringbuffer_t buffer) {
  struct stringbuffer_s *b = (struct stringbuffer_s*) buffer;
  if(!b) return 0;
  return b->alength ? b->alength - 1 : 0;
}


/**
 * @fn int stringbuffer_set_capacity(stringbuffer_t buffer, uint32_t capacity)
 * @brief Change the buffer capacity.
 * @param buffer The buffer.
 * @param capacity The buffer capacity.
 * @return -1 on error else 0.
 */
int stringbuffer_set_capacity(stringbuffer_t buffer, uint32_t capacity) {
  struct stringbuffer_s *b = (struct stringbuffer_s*) buffer;
  char* tmp;
  if(!b) return -1;
  if((b->alength - 1) == capacity) return 0;
  tmp = (char*)realloc(b->str, capacity + 1);
  if(!tmp) {
    logger(LOG_ERR, "%s: Not enough memory.\n", __func__);
    return -1;
  }

  b->alength = capacity + 1;
  b->str = tmp;
  b->str[capacity] = 0;
  return 0;
}

/**
 * @fn int stringbuffer_append(stringbuffer_t shell, const char* str)
 * @brief Append a string into the buffer.
 * @param buffer The buffer.
 * @param str The string to append.
 * @return -1 on error else 0.
 */
int stringbuffer_append(stringbuffer_t buffer, const char* str) {
  struct stringbuffer_s *b = (struct stringbuffer_s*) buffer;
  uint32_t slen = 0, diff;
  char* tmp;
  if(!b) return -1;
  if(!str) return -1;
  slen = strlen(str);
  if(!b->alength) {
    b->alength = slen + 1;
    b->length = b->alength - 1;
    b->str = (char*)malloc(b->alength);
    if(!b->str) {
      logger(LOG_ERR, "%s: Not enough memory.\n", __func__);
      b->length = b->alength = 0;
      return -1;
    }
    bzero(b->str, b->alength);
    strncpy(b->str, str, b->length);
  } else if(slen <= b->alength) {
    b->length = slen;
    strncat(b->str, str, b->length);
  } else {
    diff = abs(slen - b->alength) + 3;
    b->alength += diff;
    b->length = strlen(b->str) + slen;
    tmp = (char*)realloc(b->str, b->alength);
    if(!tmp) {
      logger(LOG_ERR, "%s: Not enough memory.\n", __func__);
      b->alength -= diff;
      b->length = b->alength - 1;
      return -1;
    }
    b->str = tmp;
    strncat(b->str, str, b->length);
  }
  return 0;
}


/**
 * @fn int stringbuffer_copy(stringbuffer_t shell, const char* str)
 * @brief Erase the buffer with the copy string.
 * @param buffer The buffer.
 * @param str The string to copy.
 * @return -1 on error else 0.
 */
int stringbuffer_copy(stringbuffer_t buffer, const char* str) {
  struct stringbuffer_s *b = (struct stringbuffer_s*) buffer;
  if(!b) return -1;
  stringbuffer_clear(b);
  return stringbuffer_append(b, str);
}

/**
 * @fn int stringbuffer_trim_to_size(stringbuffer_t buffer)
 * @brief Trim the buffer allocation size to the buffer size.
 * @param buffer The buffer.
 * @return -1 on error else 0.
 */
int stringbuffer_trim_to_size(stringbuffer_t buffer) {
  struct stringbuffer_s *b = (struct stringbuffer_s*) buffer;
  if(!b || !b->str) return -1;
  if(!b->alength) return 0;
  if((b->alength - 1) == b->length) return 0;
  b->length = strlen(b->str);
  return stringbuffer_set_capacity(b, b->length);
}

/**
 * @fn int stringbuffer_erase2(stringbuffer_t buffer, uint32_t index)
 * @brief Erase a string.
 * @param buffer The buffer.
 * @param index The index.
 * @return -1 on error else 0.
 */
int stringbuffer_erase2(stringbuffer_t buffer, uint32_t index) {
  struct stringbuffer_s *b = (struct stringbuffer_s*) buffer;
  if(!b || index > b->length || !b->length)
    return -1;
  bzero(b->str + index, b->length - index);
  b->length = strlen(b->str);
  return 0;
}

/**
 * @fn int stringbuffer_erase(stringbuffer_t buffer, uint32_t index, uint32_t length)
 * @brief Erase a string.
 * @param buffer The buffer.
 * @param index The start index.
 * @param length The length.
 * @return -1 on error else 0.
 */
int stringbuffer_erase(stringbuffer_t buffer,uint32_t index, uint32_t length) {
  struct stringbuffer_s *b = (struct stringbuffer_s*) buffer;
  if(!b || (index+length) > b->length || !b->length)
    return -1;
  memcpy(b->str+index, b->str + index + length, b->length - (index + length));
  bzero(b->str + b->length - length, length);
  b->length = strlen(b->str);
  return 0;
}

/**
 * @fn int stringbuffer_insert(stringbuffer_t buffer, uint32_t index, char* str)
 * @brief Insert a string from a specific position.
 * @param buffer The buffer.
 * @param index The index.
 * @param str The str.
 * @return -1 on error else 0.
 */
int stringbuffer_insert(stringbuffer_t buffer, uint32_t index, char* str) {
  struct stringbuffer_s *b = (struct stringbuffer_s*) buffer;
  if(!b || index > b->length || !b->length)
    return -1;
  if(index == b->length)
    return stringbuffer_append(buffer, str);
 
  char tmp[b->length - index];
  memcpy(tmp, b->str + index, b->length - index);
  tmp[b->length - index] = 0;
  bzero(b->str + index, b->length - index);
  b->length = strlen(b->str);
  stringbuffer_append(b, str);
  stringbuffer_append(b, tmp);
  return 0;
}

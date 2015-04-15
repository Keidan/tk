/**
 *******************************************************************************
 * @file bytebuffer.c
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
#include <tk/utils/bytebuffer.h>
#include <tk/sys/log.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <tk/utils/string.h>

#define SMAGIC 0xC001
#define EMAGIC 0x1337

struct bytebuffer_s {
    int smagic;
    void* bytes;
    uint32_t length;
    uint32_t alength;
    int emagic;
};

/**
 * @fn buffer_term_t bytebuffer_new()
 * @brief New byte buffer.
 */
bytebuffer_t bytebuffer_new() {
  struct bytebuffer_s *b = malloc(sizeof(struct bytebuffer_s));
  if(!b) {
    logger(LOG_ERR, "%s: Not enough memory.\n", __func__);
    return NULL;
  }
  memset(b, 0, sizeof(struct bytebuffer_s));
  b->smagic = SMAGIC;
  b->emagic = EMAGIC;
  bytebuffer_set_capacity(b, 1);
  memset(b->bytes, 0, b->alength);
  return b;
}

/**
 * @fn void bytebuffer_delete(bytebuffer_t buffer)
 * @brief Delete byte buffer.
 * @param buffer The buffer pointer.
 */
void bytebuffer_delete(bytebuffer_t buffer) {
  struct bytebuffer_s *b = (struct bytebuffer_s*) buffer;
  if(b) {
    if(b->smagic != SMAGIC || b->emagic != EMAGIC) {
      logger(LOG_ERR, "%s: Currupted buffer detected.\n", __func__);
      return;
    }
    if(b->bytes) free(b->bytes), b->bytes = NULL;
    b->length = b->alength = 0;
    free(b);
  }
}

/**
 * @fn void bytebuffer_clear(bytebuffer_t buffer)
 * @brief Clear the buffer.
 * @param buffer The buffer pointer.
 */
void bytebuffer_clear(bytebuffer_t buffer) {
  struct bytebuffer_s *b = (struct bytebuffer_s*) buffer;
  if(!b || !b->bytes) return;
  bzero(b->bytes, b->length);
  b->length = 0;
}

/**
 * @fn char* bytebuffer_to_bytes(bytebuffer_t buffer)
 * @brief Get the buffer to char* format.
 * @param buffer The buffer.
 * @return The char* (can be NULL)
 */
char* bytebuffer_to_bytes(bytebuffer_t buffer) {
  struct bytebuffer_s *b = (struct bytebuffer_s*) buffer;
  if(!b) return NULL;
  return b->bytes;
}

/**
 * @fn uint32_t bytebuffer_length(bytebuffer_t buffer)
 * @brief Get the buffer length.
 * @param buffer The buffer.
 * @return The length
 */
uint32_t bytebuffer_length(bytebuffer_t buffer) {
  struct bytebuffer_s *b = (struct bytebuffer_s*) buffer;
  if(!b) return 0;
  return b->length;
}

/**
 * @fn uint32_t bytebuffer_capacity(bytebuffer_t buffer)
 * @brief Get the buffer capacity.
 * @param buffer The buffer.
 * @return The capacity
 */
uint32_t bytebuffer_capacity(bytebuffer_t buffer) {
  struct bytebuffer_s *b = (struct bytebuffer_s*) buffer;
  if(!b) return 0;
  return b->alength ? b->alength - 1 : 0;
}


/**
 * @fn int bytebuffer_set_capacity(bytebuffer_t buffer, uint32_t capacity)
 * @brief Change the buffer capacity.
 * @param buffer The buffer.
 * @param capacity The buffer capacity.
 * @return -1 on error else 0.
 */
int bytebuffer_set_capacity(bytebuffer_t buffer, uint32_t capacity) {
  struct bytebuffer_s *b = (struct bytebuffer_s*) buffer;
  char* tmp;
  if(!b) return -1;
  if((b->alength - 1) == capacity) return 0;
  tmp = (char*)realloc(b->bytes, capacity + 1);
  if(!tmp) {
    logger(LOG_ERR, "%s: Not enough memory.\n", __func__);
    return -1;
  }

  b->alength = capacity + 1;
  b->bytes = tmp;
  memset(b->bytes + capacity, 0, 1);
  return 0;
}

/**
 * @fn int bytebuffer_append_byte(bytebuffer_t buffer, const char c)
 * @brief Append a byte into the buffer.
 * @param buffer The buffer.
 * @param c The byte to append.
 * @return -1 on error else 0.
 */
int bytebuffer_append_byte(bytebuffer_t buffer, const char c) {
  char cc[2];
  cc[0] = c;
  cc[1] = 0;
  return bytebuffer_append(buffer, cc, 1);
}

/**
 * @fn int bytebuffer_append(bytebuffer_t buffer, const void* bytes, uint32_t bytes_length)
 * @brief Append a bytes into the buffer.
 * @param buffer The buffer.
 * @param bytes The bytes to append.
 * @param bytes_length The bytes length.
 * @return -1 on error else 0.
 */
int bytebuffer_append(bytebuffer_t buffer, const void* bytes, uint32_t bytes_length) {
  struct bytebuffer_s *b = (struct bytebuffer_s*) buffer;
  uint32_t slen = 0, diff, of;
  char* tmp;
  if(!b) return -1;
  if(!bytes) return -1;
  slen = bytes_length;
  if(!b->alength) {
    b->alength = slen + 1;
    b->length = b->alength - 1;
    b->bytes = (char*)malloc(b->alength);
    if(!b->bytes) {
      logger(LOG_ERR, "%s: Not enough memory.\n", __func__);
      b->length = b->alength = 0;
      return -1;
    }
    bzero(b->bytes, b->alength);
    memcpy(b->bytes, bytes, b->length);
  } else if(b->length + slen <= b->alength) {
    memcpy(b->bytes + b->length, bytes, slen);
    b->length += slen;
  } else {
    diff = abs(b->alength - (b->length + slen)) + 1;
    b->alength += diff;
    of = b->length;
    b->length = b->alength;//strlen(b->bytes) + slen;
    tmp = (char*)realloc(b->bytes, b->alength);
    if(!tmp) {
      logger(LOG_ERR, "%s: Not enough memory.\n", __func__);
      b->alength -= diff;
      b->length = b->alength - 1;
      return -1;
    }
    b->bytes = tmp;
    memcpy(b->bytes + of, bytes, slen);
  }
  return 0;
}

/**
 * @fn int bytebuffer_copy_byte(bytebuffer_t buffer, const char c)
 * @brief Erase the buffer with the copy byte.
 * @param buffer The buffer.
 * @param c The byte to copy.
 * @return -1 on error else 0.
 */
int bytebuffer_copy_byte(bytebuffer_t buffer, const char c) {
  char cc[2];
  cc[0] = c;
  cc[1] = 0;
  return bytebuffer_copy(buffer, cc, 1);
}

/**
 * @fn int bytebuffer_copy(bytebuffer_t buffer, const void* bytes, uint32_t bytes_length)
 * @brief Erase the buffer with the copy bytes.
 * @param buffer The buffer.
 * @param bytes The bytes to copy.
 * @param bytes_length The bytes length.
 * @return -1 on error else 0.
 */
int bytebuffer_copy(bytebuffer_t buffer, const void* bytes, uint32_t bytes_length) {
  struct bytebuffer_s *b = (struct bytebuffer_s*) buffer;
  if(!b) return -1;
  bytebuffer_clear(b);
  return bytebuffer_append(b, bytes, bytes_length);
}

/**
 * @fn int bytebuffer_trim_to_size(bytebuffer_t buffer)
 * @brief Trim the buffer allocation size to the buffer size.
 * @param buffer The buffer.
 * @return -1 on error else 0.
 */
int bytebuffer_trim_to_size(bytebuffer_t buffer) {
  struct bytebuffer_s *b = (struct bytebuffer_s*) buffer;
  if(!b || !b->bytes) return -1;
  if(!b->alength) return 0;
  if((b->alength - 1) == b->length) return 0;
  b->alength = b->length;
  return bytebuffer_set_capacity(b, b->length);
}


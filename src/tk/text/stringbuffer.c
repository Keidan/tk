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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <tk/text/string.h>

#define SMAGIC 0x8001
#define EMAGIC 0x1337

struct stringbuffer_s {
    int smagic;
    char* str;
    uint32_t length;
    uint32_t alength;
    int emagic;
};

/**
 * @fn shell_term_t stringbuffer_new()
 * @brief New shell buffer.
 */
stringbuffer_t stringbuffer_new() {
  struct stringbuffer_s *b = malloc(sizeof(struct stringbuffer_s));
  if(!b) {
    logger(LOG_ERR, "%s: Not enough memory.\n", __func__);
    return NULL;
  }
  memset(b, 0, sizeof(struct stringbuffer_s));
  b->smagic = SMAGIC;
  b->emagic = EMAGIC;
  stringbuffer_set_capacity(b, 1);
  b->str[0] = 0;
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
    if(b->smagic != SMAGIC || b->emagic != EMAGIC) {
      logger(LOG_ERR, "%s: Currupted buffer detected.\n", __func__);
      return;
    }
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
 * @fn int stringbuffer_append_char(stringbuffer_t shell, const char c)
 * @brief Append a char into the buffer.
 * @param buffer The buffer.
 * @param c The char to append.
 * @return -1 on error else 0.
 */
int stringbuffer_append_char(stringbuffer_t buffer, const char c) {
  char cc[2];
  cc[0] = c;
  cc[1] = 0;
  return stringbuffer_append(buffer, cc);
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
  } else if((strlen(b->str) + slen) <= b->alength) {
    b->length += slen;
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
 * @fn int stringbuffer_copy_char(stringbuffer_t shell, const char c)
 * @brief Erase the buffer with the copy char.
 * @param buffer The buffer.
 * @param c The char to copy.
 * @return -1 on error else 0.
 */
int stringbuffer_copy_char(stringbuffer_t buffer, const char c) {
  char cc[2];
  cc[0] = c;
  cc[1] = 0;
  return stringbuffer_copy(buffer, cc);
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

/**
 * @fn int stringbuffer_printf(stringbuffer_t buffer, const char* fmt, ...)
 * @brief Simple printf into the stringbuffer.
 * @param buffer The buffer.
 * @param fmt The format.
 * @param ... The arguments.
 * @return -1 on error else 0.
 */
int stringbuffer_printf(stringbuffer_t buffer, const char* fmt, ...) {
  struct stringbuffer_s *b = (struct stringbuffer_s*) buffer;
  if(!b) return -1;
  uint32_t ltmp = 40;
  char *p, c, tmp[ltmp];
  int i, count, idx;
  va_list pa;
  stringbuffer_clear(b);
  va_start(pa, fmt);
  p = (char*)fmt;
  while(*p != '\0') {
    if(*p == '%') {
      p++;
      if((*p) == '#') { /* %#x */
	stringbuffer_append(b, "0x");
  	p++;
      }
      /* %02x */
      if((isdigit(*p) || *p == ' ') && isdigit(*p+1)) {
	c = *p;
	count = (*p+1) - '0';
	for(idx = 0; idx < count; idx++)
	  stringbuffer_append_char(b, c);
	p+=2;
      }
      switch (*p) {
        case '%' :
	  stringbuffer_append_char(b, *p);
  	  break;
        case 'c' :
	  stringbuffer_append_char(b, (char)va_arg(pa, int));/* !!!!! */
  	  break;
        case 'd' :
        case 'i' :
  	  i = va_arg(pa, int);	
	  if(i<0){
	    i=-i;
	    stringbuffer_append_char(b, '-');
	  }
	  stringbuffer_append(b, string_convert(i, 10));
  	  break;
        case 'u' :
	  stringbuffer_append(b, string_convert(va_arg(pa, unsigned int), 10));
  	  break;
        case 'o' :
	  stringbuffer_append(b, string_convert(va_arg(pa, unsigned int), 8));
  	  break;
        case 'p' :
        case 'x' :
	  stringbuffer_append(b, string_convert(va_arg(pa, unsigned int), 16));
  	  break;
        case 'X' :
	  bzero(tmp, ltmp);
	  string_toupper(string_convert(va_arg(pa, int), 16), tmp);
	  stringbuffer_append(b, tmp);
  	  break;
        case 'f' :
	  bzero(tmp, ltmp);
  	  snprintf(tmp, ltmp, "%f", va_arg(pa, double));/* !!!!! */
	  stringbuffer_append(b, tmp);
  	  break;
        case 's' :
	  stringbuffer_append(b, va_arg(pa, char *)); 
  	  break;
	default:
	  stringbuffer_append_char(b, *p); 
      } /* end switch */
    }
    else {
      stringbuffer_append_char(b, *p);
    }
    p++;
  }
  va_end(pa);
  return 0;
}

/**
*******************************************************************************
* @file stringtoken.c
* @author Keidan
* @date 10/04/2013
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
#include <tk/utils/stringtoken.h>
#include <tk/utils/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

typedef struct {
    char*    str;
    char*    sep;
    uint32_t len;
    uint32_t offset;
} stringtoken_handle_t;

/**
 * @fn stringtoken_t stringtoken_init(const char* str, const char* sep)
 * @brief Initialisation of the string token.
 * @param str String to use.
 * @param sep Delimiter.
 * @return Get a token pointer else NULL on error.
 */
stringtoken_t stringtoken_init(const char* str, const char* sep) {
  stringtoken_handle_t *tok = malloc(sizeof(stringtoken_handle_t));
  tok->str = (char*)str;
  tok->sep = (char*)sep;
  tok->len = strlen(str);
  tok->offset = 0;
  return tok;
}

/**
 * @fn uint32_t stringtoken_count(stringtoken_t tok)
 * @brief Count the number of tokens.
 * @param tok Token pointer.
 * @return The number of tokens.
 */
uint32_t stringtoken_count(stringtoken_t tok) {
  stringtoken_handle_t *t = (stringtoken_handle_t *)tok;
  if(!t) return 0;
  uint32_t offset = t->offset, count = 0;
  t->offset = 0;
  while(stringtoken_has_more_tokens(t)) {
    stringtoken_next_token(t);
    count++;
  }
  t->offset = offset;
  return count;
}

/**
 * @fn void stringtoken_release(stringtoken_t tok)
 * @brief Release the token.
 * @param tok Token pointer.
 */
void stringtoken_release(stringtoken_t tok) {
  free(tok);
}


/**
 * @fn _Bool stringtoken_has_more_tokens(stringtoken_t tok)
 * @brief Test if the token contains more tokens.
 * @param tok Token pointer.
 * @return 1 if has more else 0.
 */
_Bool stringtoken_has_more_tokens(stringtoken_t tok) {
  stringtoken_handle_t *t = (stringtoken_handle_t *)tok;
  if(!t) return 0;
  return t->offset < t->len;
}

/**
 * @fn char* stringtoken_next_token(stringtoken_t tok)
 * @brief Get the next token.
 * @param tok Token pointer
 * @return New token else NULL (free is required for non NULL values)
 */
char* stringtoken_next_token(stringtoken_t tok) {
  stringtoken_handle_t *t = (stringtoken_handle_t *)tok;
  if(!t) return NULL;
  // search the next split
  char c;
  int i = 0;
  int end = 0;
  // check immediate end of look
  if (!stringtoken_has_more_tokens(tok))
    return NULL;
  while (!end) {
    // check end of look
    if (t->offset + i >= t->len)
      break;
    // look next char
    c = t->str[t->offset + i];
    if (stringtoken_split_condition(c, t->sep)) {
      // split here
      end = 1;
    }
    else i++;
  }
  char* s = string_substring(t->str, t->offset, i);
  t->offset += i + 1; // go just after the sep
  return s;
}


/**
 * @fn void stringtoken_set_separator(stringtoken_t tok, char* sep)
 * @brief update the delimiter.
 * @param tok Token pointer.
 * @param sep New delmiter.
 */
void stringtoken_set_separator(stringtoken_t tok, char* sep) {
  stringtoken_handle_t *t = (stringtoken_handle_t *)tok;
  t->sep = sep;
}

/**
 * @fn int stringtoken_split_condition(char c, char* sep)
 * @brief Test if the condition can be splited.
 * @param c Char to test.
 * @param sep Delimiter.
 */
int stringtoken_split_condition(char c, char* sep) {
  uint32_t i;
  for (i = 0; i< strlen(sep); i++)
    if (c== sep[i]) return 1;
  return 0; // no matches
}


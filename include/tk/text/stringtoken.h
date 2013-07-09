/**
 *******************************************************************************
 * @file stringtoken.h
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
#ifndef __STRINGTOKEN_H__
  #define __STRINGTOKEN_H__

  #include <stdint.h>
  #include <string.h>

  typedef void* stringtoken_t;

  /**
   * @fn stringtoken_t stringtoken_init(const char* str, const char* sep)
   * @brief Initialisation of the string token.
   * @param str String to use.
   * @param sep Delimiter.
   * @return Get a token pointer else NULL on error.
   */
  stringtoken_t stringtoken_init(const char* str, const char* sep);

  /**
   * @fn void stringtoken_release(stringtoken_t tok)
   * @brief Release the token.
   * @param tok Token pointer.
   */
  void stringtoken_release(stringtoken_t tok);

  /**
   * @fn _Bool stringtoken_has_more_tokens(stringtoken_t tok)
   * @brief Test if the token contains more tokens.
   * @param tok Token pointer.
   * @return 1 if has more else 0.
   */
  _Bool stringtoken_has_more_tokens(stringtoken_t tok);

  /**
   * @fn uint32_t stringtoken_count(stringtoken_t tok)
   * @brief Count the number of tokens.
   * @param tok Token pointer.
   * @return The number of tokens.
   */
  uint32_t stringtoken_count(stringtoken_t tok);

  /**
   * @fn char* stringtoken_next_token(stringtoken_t tok)
   * @brief Get the next token.
   * @param tok Token pointer
   * @return New token else NULL (free is required for non NULL values)
   */
  char* stringtoken_next_token(stringtoken_t tok);

  /**
   * @fn void stringtoken_set_separator(stringtoken_t tok, char* sep)
   * @brief update the delimiter.
   * @param tok Token pointer.
   * @param sep New delmiter.
   */
  void stringtoken_set_separator(stringtoken_t tok, char* sep);

  /**
   * @fn int stringtoken_split_condition(char c, char* sep)
   * @brief Test if the condition can be splited.
   * @param c Char to test.
   * @param sep Delimiter.
   */
  int stringtoken_split_condition(char c, char* sep);

#endif /* __STRINGTOKEN_H__ */

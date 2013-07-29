/**
 *******************************************************************************
 * @file string.h
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
#ifndef __STRING_H__
  #define __STRING_H__

  #include <stdint.h>
  #include <string.h>

  typedef char string_t[255];

  /**
   * @fn char* string_substring(const char* str, size_t begin, size_t len)
   * @brief Substract a string.
   * @param str Input string.
   * @param begin Begin index.
   * @param len Length.
   * @return substring (free required).
   */
  char* string_substring(const char* str, size_t begin, size_t len);

  /**
   * @fn int string_indexof(const char* source, const char* needed)
   * @brief Search the index of the needed string into the input string.
   * @param source Search string.
   * @param needed Needed string.
   * @return -1 if not found else 0.
   */
  int string_indexof(const char* source, const char* needed);

  /**
   * @fn size_t string_count(const char* source, const char needed)
   * @brief Count the number of occurences of the needed char.
   * @param source Search string.
   * @param needed The char.
   * @return Occurences number.
   */
  size_t string_count(const char* source, const char needed);

  /**
   * @fn int string_isint(const char* source)
   * @brief Test if the string is a valid digit.
   * @param source The source string
   * @return 1 if the string is a valid int.
   */
  int string_isint(const char* source);


  /**
   * @fn void string_tolower(const char* source, char* dest)
   * @brief Convert a string in lower case.
   * @param source Source string.
   * @param dest Destination string.
   */
  void string_tolower(const char* source, char* dest);

  /**
   * @fn void string_toupper(const char* source, char* dest)
   * @brief Convert a string in upper case.
   * @param source Source string.
   * @param dest Destination string.
   */
  void string_toupper(const char* source, char* dest);

  /**
   * @fn int string_parse_int(char* str, int def)
   * @brief Convert a string int to int value.
   * @param str Src string.
   * @param def the default value on error
   * @return the int value.
   */
  int string_parse_int(char* str, int def);

  /**
   * @fn _Bool string_match(const char* str, const char* regex)
   * @brief Test if the regex match with the input string.
   * @param str The string to test.
   * @param regex The regex
   * @return 1 if the regex match else 0.
   */
  _Bool string_match(const char* str, const char* regex);

#endif /* __STRING_H__ */

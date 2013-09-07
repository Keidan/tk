/**
*******************************************************************************
* @file string.c
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
#include <tk/utils/string.h>
#include <tk/sys/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <errno.h>

/**
 * @fn void string_tolower(const char* source, char* dest)
 * @brief Convert a string in lower case.
 * @param source Source string.
 * @param dest Destination string.
 */
void string_tolower(const char* source, char* dest) {
  int i;
  for(i = 0; i < strlen(source); i++)
    dest[i] = tolower(source[i]);
}

/**
 * @fn void string_toupper(const char* source, char* dest)
 * @brief Convert a string in upper case.
 * @param source Source string.
 * @param dest Destination string.
 */
void string_toupper(const char* source, char* dest) {
  int i;
  for(i = 0; i < strlen(source); i++)
    dest[i] = toupper(source[i]);
}

/**
 * @fn char* string_substring(const char* str, size_t begin, size_t len)
 * @brief Substract a string.
 * @param str Input string.
 * @param begin Begin index.
 * @param len Length.
 * @return substring (free required).
 */
char* string_substring(const char* str, size_t begin, size_t len)  { 
  if (str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin+len)) 
    return 0; 
  return strndup(str + begin, len); 
} 

/**
 * @fn int string_indexof(const char* source, const char* needed)
 * @brief Search the index of the needed string into the input string.
 * @param source Search string.
 * @param needed Needed string.
 * @return -1 if not found else 0.
 */
int string_indexof(const char* source, const char* needed) {
  char * found = strstr(source, needed);
  if(found != NULL)  return found - source;
  return -1;
}

/**
 * @fn size_t string_count(const char* source, const char needed)
 * @brief Count the number of occurences of the needed char.
 * @param source Search string.
 * @param needed The char.
 * @return Occurences number.
 */
size_t string_count(const char* source, const char needed) {
  size_t i, len = strlen(source), count = 0;
  for(i = 0; i < len; i++)
    if(source[i] == needed) count++;
  return count;
}


/**
 * @fn int string_isint(const char* source)
 * @brief Test if the string is a valid digit.
 * @param source The source string
 * @return 1 if the string is a valid int.
 */
int string_isint(const char* source) {
  int i, off = 0;
  if(source[0] == '-') off = 1;
  for(i = off; i < strlen(source); i++)
    if(!isdigit(source[i]))
      return 0;
  return 1;
}

/**
 * @fn int string_parse_int(char* str, int def)
 * @brief Convert a string int to int value.
 * @param str Src string.
 * @param def the default value on error
 * @return the int value.
 */
int string_parse_int(char* str, int def) {
  int n = strtol(str, NULL, 10);
  if((errno == ERANGE) || (errno == EINVAL)) {
    return def;
  }
  return n;
}

/**
 * @fn long long int string_parse_long(char* str, long long int def)
 * @brief Convert a string int to int value.
 * @param str Src string.
 * @param def the default value on error
 * @return the int value.
 */
long long int string_parse_long(char* str, long long int def) {
  int n = strtoll(str, NULL, 10);
  if((errno == ERANGE) || (errno == EINVAL)) {
    return def;
  }
  return n;
}

/**
 * @fn _Bool string_match(const char* str, const char* regex)
 * @brief Test if the regex match with the input string.
 * @param str The string to test.
 * @param regex The regex
 * @return 1 if the regex match else 0.
 */
_Bool string_match(const char* str, const char* regex) {
  regex_t preg;
  char *text;
  size_t size;
  int err = regcomp(&preg, regex, REG_NOSUB|REG_EXTENDED);
  if(!err) {
    int match = regexec(&preg, str, 0, NULL, 0);
    regfree(&preg);
    if(!match) return 1;
    else if(match == REG_NOMATCH) return 0;
    else {
      size = regerror(err, &preg, NULL, 0);
      text = malloc(sizeof (*text) * size);
      if(text) {
        regerror (err, &preg, text, size);
        logger(LOG_ERR, "regex failed with error: %s\n", text);
        free (text);
      }
      else {
	logger(LOG_ERR, "Unable to alloc memory for regex message!\n");
      }
    }
  }
  return 0;
}

/**
 * @fn const char* string_convert(unsigned long num, int base)
 * @brief Convert an unsigned int in string.
 * @param num The uint
 * @param base The desired base.
 * @return The string.
 */
const char* string_convert(unsigned long num, int base) {
  static char buf[33];
  char *ptr;
  ptr=&buf[sizeof(buf)-1];
  *ptr='\0';
  do {
    *--ptr="0123456789abcdef"[num%base];
    num /= base;
  } while(num != 0);
  return (const char*)ptr;
}

/**
 * @fn const char* const string_hex2bin(const char* numstr)
 * @brief Convert and hexa string in binary value.
 * @param hexstr The hex string.
 * @return The binary representation.
 */
const char* const string_hex2bin(const char* hexstr) {
  static char str[132];
  static char hex[] = "0123456789ABCDEF";
  static char *quad [] = { "0000", "0001", "0010", "0011", "0100", "0101",
                           "0110", "0111", "1000", "1001", "1010", "1011",
                           "1100", "1101", "1110", "1111" };
  if(hexstr[0] == '0' && hexstr[1] == 'x')
    hexstr+=2;
  bzero(str, 132);
  int i = 0;
  while(*hexstr) {
    strcpy(str+i, quad[strchr(hex, *hexstr)-hex]);
    i+=4;
    hexstr++;
  }
  return str;
}

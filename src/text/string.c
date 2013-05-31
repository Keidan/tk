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
#include <tk/text/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <errno.h>

/**
 * @brief Passe une chaine en minuscule.
 * @param source Chaine source.
 * @param dest Chaine de destination.
 */
void string_tolower(const char* source, char* dest) {
  int i;
  for(i = 0; i < strlen(source); i++)
    dest[i] = tolower(source[i]);
}

/**
 * @brief Passe une chaine en majuscule.
 * @param source Chaine source.
 * @param dest Chaine de destination.
 */
void string_toupper(const char* source, char* dest) {
  int i;
  for(i = 0; i < strlen(source); i++)
    dest[i] = toupper(source[i]);
}

/**
 * @brief Simple fonction substring
 * @param str Chaine d'entree.
 * @param begin Point de depart.
 * @param len Longueur.
 * @return Fragment de chaine.
 */
char* string_substring(const char* str, size_t begin, size_t len)  { 
  if (str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin+len)) 
    return 0; 
  return strndup(str + begin, len); 
} 

/**
 * @brief Simple fonction indexof
 * @param source Chaine de recherche.
 * @param needed Chaine a chercher.
 * @return Retourne -1 si non trouve sinon sa position.
 */
int string_indexof(const char* source, const char* needed) {
  char * found = strstr(source, needed);
  if(found != NULL)  return found - source;
  return -1;
}

/**
 * @brief Test si une chaine est un entier ou non.
 * @param source Chaine de test.
 * @return Retourne 1 si oui sinon retourne 0.
 */
int string_isint(const char* source) {
  int i;
  for(i = 0; i < strlen(source); i++)
    if(!isdigit(source[i]))
      return 0;
  return 1;
}

/**
 * @brief Converti un entier string en int
 * @param str Chaine source.
 * @param def Valeur en cas d'erreur.
 * @return l'entier.
 */
int string_parse_int(char* str, int def) {
  int n = strtol(str, NULL, 10);
  if((errno == ERANGE) || (errno == EINVAL)) {
    return def;
  }
  return n;
}

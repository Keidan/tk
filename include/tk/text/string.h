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

  /**
   * @brief Simple fonction substring
   * @param str Chaine d'entree.
   * @param begin Point de depart.
   * @param len Longueur.
   * @return Fragment de chaine.
   */
  char* string_substring(const char* str, size_t begin, size_t len);

  /**
   * @brief Simple fonction indexof
   * @param source Chaine de recherche.
   * @param needed Chaine a chercher.
   * @return Retourne -1 si non trouve sinon sa position.
   */
  int string_indexof(const char* source, const char* needed);

  /**
   * @fn size_t string_count(const char* source, const char needed)
   * @brief Compte le nombre de fois ou le char est trouve.
   * @param source Chaine de recherche.
   * @param needed Char a compter.
   * @return Nombre d'occurences.
   */
  size_t string_count(const char* source, const char needed);

  /**
   * @brief Test si une chaine est un entier ou non.
   * @param source Chaine de test.
   * @return Retourne 1 si oui sinon retourne 0.
   */
  int string_isint(const char* source);

  /**
   * @brief Passe une chaine en minuscule.
   * @param source Chaine source.
   * @param dest Chaine de destination.
   */
  void string_tolower(const char* source, char* dest);

  /**
   * @brief Passe une chaine en majuscule.
   * @param source Chaine source.
   * @param dest Chaine de destination.
   */
  void string_toupper(const char* source, char* dest);

  /**
   * @brief Converti un entier string en int
   * @param str Chaine source.
   * @param def Valeur en cas d'erreur.
   * @return l'entier.
   */
  int string_parse_int(char* str, int def);

#endif /* __STRING_H__ */

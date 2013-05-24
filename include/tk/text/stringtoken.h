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
   * @brief Initialisation du string token.
   * @param str Chaine a utiliser.
   * @param sep Separateur.
   * @return Retourne un handle sur le token ou null.
   */
  stringtoken_t stringtoken_init(const char* str, const char* sep);

  /**
   * @brief Liberation des ressources.
   * @param tok Handle sur le token.
   */
  void stringtoken_release(stringtoken_t tok);

  /**
   * @brief Test si il y a encore des tokens.
   * @param tok Handle sur le token.
   * @return 1 si il en reste sinon 0.
   */
  int stringtoken_has_more_tokens(stringtoken_t tok);


  uint32_t stringtoken_count(stringtoken_t tok);

  /**
   * @brief Recuperation du prochain token.
   * @param tok Handle sur le token.
   * @return Nouveau token ou null.
   */
  char* stringtoken_next_token(stringtoken_t tok);

  /**
   * @brief Changement de separateur.
   * @param tok Handle sur le token.
   * @param sep Nouveau separateur.
   */
  void stringtoken_set_separator(stringtoken_t tok, char* sep);

  /**
   * @brief Test si la condition peut etre splitee.
   * @param c Char a tester.
   * @param sep Separateur.
   */
  int stringtoken_split_condition(char c, char* sep);

#endif /* __STRINGTOKEN_H__ */

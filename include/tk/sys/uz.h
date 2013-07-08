/**
 *******************************************************************************
 * @file uz.h
 * @author Keidan
 * @date 03/02/2013
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
#ifndef __UZ_H__
  #define __UZ_H__

  #include <stdlib.h>
  #include <stdbool.h>
  #include <zlib-minizip/unzip.h>
  #include <string.h>


  #ifndef UZ_DIR_DELIMITER
    /* Directory delimiter */
    #define UZ_DIR_DELIMITER '/'
  #endif

  struct uzentry_s {
      char          name[FILENAME_MAX];
      _Bool         isdir;
      unz_file_info info;
      char          *content;              /* Uncompressed file content */
  };

  
  struct uz_ctx_s {
      int magic;
      char filename [FILENAME_MAX];        /* Zip file name */
      unzFile ctx;                         /* Internale zip context */
      unz_global_info ginfo;               /* Global informations about the zip file */
  };
  typedef struct uz_ctx_s *uz_t;

  typedef void (*uz_file_content_fct)(uz_t uz, struct uzentry_s entry);

  /**
   * @fn uz_t uz_open(const char filename[FILENAME_MAX])
   * @brief Ouvre un fichier ZIP.
   * @param filename Chemin du fichier ZIP.
   * @return Le context ZIP ou NULL en cas d'erreur.
   */
  uz_t uz_open(const char filename[FILENAME_MAX]);

  /**
   * @fn _Bool uz_is_valid(uz_t uz)
   * @brief Test si le pointeur est valide.
   * @param uz Context ZIP.
   * @return 0 si il n'est pas valide sinon 1.
   */
  _Bool uz_is_valid(uz_t uz);

  /**
   * @fn void uz_close(uz_t uz)
   * @brief Ferme le context ZIP.
   * @param uz Context ZIP.
   */
  void uz_close(uz_t uz);

  /**
   * @fn _Bool uz_is_dir(char* path)
   * @brief Test si le fichier est un repertoire ou non.
   * @param path fichier a tester.
   * @return 0 si il n'est pas un repertoire sinon 1.
   */
  _Bool uz_is_dir(char* path);

  /**
   * @fn int uz_get_contents(uz_t uz, uz_file_content_fct uz_file_content)
   * @brief Recuperation et decompression des fichiers du ZIP.
   * @param uz Context ZIP.
   * @param uz_file_content Fonction appelee apres decompression d'un fichier.
   * @return -1 en cas d'erreur sinon 0.
   */
  int uz_get_contents(uz_t uz, uz_file_content_fct uz_file_content);

#endif /* __UZ_H__ */

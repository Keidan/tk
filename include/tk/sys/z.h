/**
 *******************************************************************************
 * @file z.h
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
#ifndef __Z_H__
  #define __Z_H__

  #include <stdlib.h>
  #include <stdbool.h>
  #include <zlib-minizip/unzip.h>
  #include <string.h>


  #ifndef Z_DIR_DELIMITER
    /* Directory delimiter */
    #define Z_DIR_DELIMITER '/'
  #endif

  struct zentry_s {
      char          name[FILENAME_MAX];
      _Bool         isdir;
      unz_file_info info;
      char          *content;              /* Uncompressed file content */
  };

  
  struct z_ctx_s {
      int magic;
      char filename [FILENAME_MAX];        /* Zip file name */
      char dir_delimiter;
      unzFile ctx;                         /* Internale zip context */
      unz_global_info ginfo;               /* Global informations about the zip file */
  };
  typedef struct z_ctx_s *z_t;

  typedef void (*z_file_content_fct)(z_t z, struct zentry_s entry);

  /**
   * @fn z_t z_open(const char filename[FILENAME_MAX])
   * @brief Open a new ZIP file.
   * @param filename ZIP file name.
   * @return The ZIP context else NULL on error.
   */
  z_t z_open(const char filename[FILENAME_MAX]);

  /**
   * @fn _Bool z_is_valid(z_t z)
   * @brief Check if the inut pointer is valid.
   * @param z Pointer to test.
   * @return 1 the pointer is valid else 0.
   */
  _Bool z_is_valid(z_t z);

  /**
   * @fn void z_close(z_t z)
   * @brief Close the zip context.
   * @param z ZIP context.
   */
  void z_close(z_t z);

  /**
   * @fn _Bool z_is_dir(z_t z, char* path)
   * @brief Test if the input path os a directry.
   * @param z ZIP context.
   * @param path Path to test.
   * @return 1 if the current path is a directory else 0.
   */
  _Bool z_is_dir(z_t z, char* path);

  /**
   * @fn int z_get_contents(z_t z, z_file_content_fct z_file_content)
   * @brief Unzip the ZIP files.
   * @param z ZIP context.
   * @param z_file_content Callback to received the uncompressed file datas.
   * @return -1 on failure else 0.
   */
  int z_get_contents(z_t z, z_file_content_fct z_file_content);

  /**
   * @fn void z_set_dir_delimiter(z_t z, char delimiter)
   * @brief Change the directory delimiter.
   * @param z ZIP context.
   * @param delimiter The new deimiter.
   */
  void z_set_dir_delimiter(z_t z, char delimiter);


  /**
   * @fn char z_get_dir_delimiter(z_t z)
   * @brief Get the current directory delimiter.
   * @param z ZIP context.
   * @return The deimiter.
   */
  char z_get_dir_delimiter(z_t z);
#endif /* __Z_H__ */

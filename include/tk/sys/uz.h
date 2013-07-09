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
      char dir_delimiter;
      unzFile ctx;                         /* Internale zip context */
      unz_global_info ginfo;               /* Global informations about the zip file */
  };
  typedef struct uz_ctx_s *uz_t;

  typedef void (*uz_file_content_fct)(uz_t uz, struct uzentry_s entry);

  /**
   * @fn uz_t uz_open(const char filename[FILENAME_MAX])
   * @brief Open a new ZIP file.
   * @param filename ZIP file name.
   * @return The ZIP context else NULL on error.
   */
  uz_t uz_open(const char filename[FILENAME_MAX]);

  /**
   * @fn _Bool uz_is_valid(uz_t uz)
   * @brief Check if the inut pointer is valid.
   * @param uz Pointer to test.
   * @return 1 the pointer is valid else 0.
   */
  _Bool uz_is_valid(uz_t uz);

  /**
   * @fn void uz_close(uz_t uz)
   * @brief Close the zip context.
   * @param uz ZIP context.
   */
  void uz_close(uz_t uz);

  /**
   * @fn _Bool uz_is_dir(uz_t uz, char* path)
   * @brief Test if the input path os a directry.
   * @param uz ZIP context.
   * @param path Path to test.
   * @return 1 if the current path is a directory else 0.
   */
  _Bool uz_is_dir(uz_t uz, char* path);

  /**
   * @fn int uz_get_contents(uz_t uz, uz_file_content_fct uz_file_content)
   * @brief Unzip the ZIP files.
   * @param uz ZIP context.
   * @param uz_file_content Callback to received the uncompressed file datas.
   * @return -1 on failure else 0.
   */
  int uz_get_contents(uz_t uz, uz_file_content_fct uz_file_content);

  /**
   * @fn void uz_set_dir_delimiter(uz_t uz, char delimiter)
   * @brief Change the directory delimiter.
   * @param uz ZIP context.
   * @param delimiter The new deimiter.
   */
  void uz_set_dir_delimiter(uz_t uz, char delimiter);


  /**
   * @fn char uz_get_dir_delimiter(uz_t uz)
   * @brief Get the current directory delimiter.
   * @param uz ZIP context.
   * @return The deimiter.
   */
  char uz_get_dir_delimiter(uz_t uz);
#endif /* __UZ_H__ */

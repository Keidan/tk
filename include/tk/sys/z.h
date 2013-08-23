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
  #include <tk/collection/fifo.h>
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
  
  typedef void* z_t;

  struct z_uc_s {
      unzFile ctx;                         /* Internale zip context */
      unz_global_info ginfo;               /* Global informations about the zip file */
  };
  typedef struct z_uc_s *z_uc_t;

  typedef enum {
    Z_C_STORE=0,
    Z_C_FASTER=1,
    Z_C_BETTER=9
  } z_clevel_et;

  struct z_compress_s {
      char zame[FILENAME_MAX];
      char zpassword[FILENAME_MAX];
      _Bool append;
      z_clevel_et level;
      _Bool exclude_path;
  };

typedef void (*z_uncompress_callback_fct)(z_t z, struct zentry_s entry);


  /**
   * @fn z_t z_new(void)
   * @brief Alloc a new z context.
   * @return The ZIP context else NULL on error.
   */
  z_t z_new(void);

  /**
   * @fn void z_delete(z_t zip)
   * @brief Delete the ZIP context.
   * @param zip The pointer to release.
   */
  void z_delete(z_t zip);
  
  /**
   * @fn int z_open(z_t zip, const char filename[FILENAME_MAX])
   * @brief Open a new ZIP file.
   * @param zip The ZIP context.
   * @param filename ZIP file name.
   * @return -1 on error else 0.
   */
  int z_open(z_t zip, const char filename[FILENAME_MAX]);

  /**
   * @fn void z_close(z_t zip)
   * @brief Close the zip context.
   * @param zip ZIP context.
   */
  void z_close(z_t zip);

  /**
   * @fn _Bool z_is_dir(z_t zip, char* path)
   * @brief Test if the input path os a directry.
   * @param zip ZIP context.
   * @param path Path to test.
   * @return 1 if the current path is a directory else 0.
   */
  _Bool z_is_dir(z_t zip, char* path);

  /**
   * @fn int z_uncompress(z_t zip, z_uncompress_callback_fct callback)
   * @brief Unzip the ZIP files.
   * @param zip ZIP context.
   * @param callback Callback to received the uncompressed file datas.
   * @return -1 on failure else 0.
   */
  int z_uncompress(z_t zip, z_uncompress_callback_fct callback);

  /**
   * @fn int z_get_global_zinfo(z_t zip, unz_global_info *ginfo)
   * @brief Get the global zip info (open must be all).
   * @param zip ZIP context.
   * @param ginfo The result infos.
   * @return -1 on error else 0.
   */
  int z_get_global_zinfo(z_t zip, unz_global_info *ginfo);

  /**
   * @fn void z_set_dir_delimiter(z_t zip, char delimiter)
   * @brief Change the directory delimiter.
   * @param zip ZIP context.
   * @param delimiter The new deimiter.
   */
  void z_set_dir_delimiter(z_t zip, char delimiter);


  /**
   * @fn char z_get_dir_delimiter(z_t zip)
   * @brief Get the current directory delimiter.
   * @param zip ZIP context.
   * @return The deimiter.
   */
  char z_get_dir_delimiter(z_t zip);
#endif /* __Z_H__ */

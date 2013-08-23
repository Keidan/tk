/**
 *******************************************************************************
 * @file file.h
 * @author Keidan
 * @date 23/08/2013
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
#ifndef __FILE_H__
  #define __FILE_H__

  #ifndef __USE_FILE_OFFSET64
    #define __USE_FILE_OFFSET64
  #endif
  #ifndef __USE_LARGEFILE64
    #define __USE_LARGEFILE64
  #endif
  #ifndef _LARGEFILE64_SOURCE
    #define _LARGEFILE64_SOURCE
  #endif
  #ifndef _FILE_OFFSET_BIT
    #define _FILE_OFFSET_BIT 64
  #endif

  #include <time.h>
  #include <stdio.h>

  #define FILE_MAXNAME 256

  /**
   * @fn _Bool file_exists(const char* filename)
   * @brief Test if the file exeists.
   * @param filename The file name to test.
   * @return 1 if the file exists else 0.
   */
  _Bool file_exists(const char* filename);

  /**
   * @fn long file_fsize(FILE* file)
   * @brief Getting the file size.
   * @param file The file.
   * @return off_t.
   */
  off_t file_fsize(FILE* file);

  /**
   * @fn off64_t file_fsize64(FILE* file)
   * @brief Getting the file size.
   * @param file The file.
   * @return off64_t.
   */
  off64_t file_fsize64(FILE* file);

  /**
   * @fn off_t file_size(FILE* file)
   * @brief Getting the file size.
   * @param file The file.
   * @return off_t.
   */
  off_t file_size(const char* filename);

  /**
   * @fn off64_t file_size64(FILE* file)
   * @brief Getting the file size.
   * @param file The file.
   * @return off64_t.
   */
  off64_t file_size64(const char* filename);

  /**
   * @fn int file_time(char* fname, struct tm* t)
   * @brief Getting the file time.
   * @param fname name of file to get info on
   * @param t return value: access, modific. and creation times
   * @return 0 if the time is available else -1.
   */
  int file_time(char* fname, struct tm* t);

  /**
   * @fn _Bool file_is_large_file(const char* filename)
   * @brief Test if the file is a large file.
   * @param filename The file name to test.
   * @return 1 if the file is a large file else 0.
   */
  _Bool file_is_large_file(const char* filename);

#endif /* __FILE_H__ */

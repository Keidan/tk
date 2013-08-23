/**
*******************************************************************************
* @file file.c
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
#include <tk/io/file.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>


/**
 * @fn long file_fsize(FILE* file)
 * @brief Getting the file size.
 * @param file The file.
 * @return off_t.
 */
off_t file_fsize(FILE* file) {
  off_t size = 0L, old = 0L;
  if (file) {
    old = ftell(file);
    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    fseek(file, old, SEEK_SET);
  }
  return size;
}

/**
 * @fn off64_t file_fsize64(FILE* file)
 * @brief Getting the file size.
 * @param file The file.
 * @return off64_t.
 */
off64_t file_fsize64(FILE* file) {
  off64_t size = 0L, old = 0L;
  if (file) {
    old = ftello64(file);
    fseeko64(file, 0L, SEEK_END);
    size = ftello64(file);
    fseeko64(file, old, SEEK_SET);
  }
  return size;
}

/**
 * @fn off_t file_size(FILE* file)
 * @brief Getting the file size.
 * @param file The file.
 * @return off_t.
 */
off_t file_size(const char* filename) {
  off_t size = 0L;
  FILE* f = fopen(filename,"rb");
  if (f) {
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fclose(f);
  }
  return size;
}

/**
 * @fn off64_t file_size64(FILE* file)
 * @brief Getting the file size.
 * @param file The file.
 * @return off64_t.
 */
off64_t file_size64(const char* filename) {
  off64_t size = 0L;
  FILE* f = fopen64(filename,"rb");
  if (f) {
    fseeko64(f, 0, SEEK_END);
    size = ftello64(f);
    fclose(f);
  }
  return size;
}

/**
 * @fn _Bool file_exists(const char* filename)
 * @brief Test if the file exeists.
 * @param filename The file name to test.
 * @return 1 if the file exists else 0.
 */
_Bool file_exists(const char* filename) {
  FILE* f = fopen(filename,"rb");
  if (!f)
    return 0;
  fclose(f);
  return 1;
}

/**
 * @fn int file_time(char* fname, struct tm* t)
 * @brief Getting the file time.
 * @param fname name of file to get info on
 * @param t return value: access, modific. and creation times
 * @return 0 if the time is available else -1.
 */
int file_time(char* fname, struct tm* t) {
  int ret = -1;
  struct stat s;
  struct tm* fdate;
  time_t tt = 0;

  if(strcmp(fname, "-")) {
    char name[FILE_MAXNAME+1];
    int len = strlen(fname);
    if (len > FILE_MAXNAME) len = FILE_MAXNAME;

    strncpy(name, fname, FILE_MAXNAME-1);
    name[FILE_MAXNAME] = 0;

    if(name[len - 1] == '/') name[len - 1] = '\0';

    if(!stat(name, &s)) {
      tt = s.st_mtime;
      ret = 0;
    }
  }
  fdate = localtime(&tt);
  memcpy(t, fdate, sizeof(struct tm));
  return ret;
}


/**
 * @fn _Bool file_is_large_file(const char* filename)
 * @brief Test if the file is a large file.
 * @param filename The file name to test.
 * @return 1 if the file is a large file else 0.
 */
_Bool file_is_large_file(const char* filename) {
  _Bool large = 0;
  off64_t pos = 0;
  FILE* f = fopen64(filename, "rb");
  if(f != NULL) {
    fseeko64(f, 0, SEEK_END);
    pos = ftello64(f);
    if(pos >= 0xffffffff) large = 1;
    fclose(f);
  }
  return large;
}

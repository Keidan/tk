/**
*******************************************************************************
* @file uz.c
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
#include <tk/sys/uz.h>
#include <tk/sys/log.h>

#define UZ_MAGIC         0xC001214

#define uzlogger(...) ({			\
    logger(LOG_ERR, __VA_ARGS__);		\
    fprintf(stderr, __VA_ARGS__);		\
  })

/**
 * @fn uz_t uz_open(const char filename[FILENAME_MAX])
 * @brief Open a new ZIP file.
 * @param filename ZIP file name.
 * @return The ZIP context else NULL on error.
 */
uz_t uz_open(const char filename[FILENAME_MAX]) {
  uz_t uz = NULL;
  if(!(uz = (uz_t)malloc(sizeof(struct uz_ctx_s)))) {
    uzlogger("Unable to alloc a memory for the zip file '%s'\n", filename);
    return NULL;
  }
  memset(uz, 0, sizeof(struct uz_ctx_s));
  uz->magic = UZ_MAGIC;
  uz->dir_delimiter = UZ_DIR_DELIMITER;
  strcpy(uz->filename, filename);
  /* Open the zip file */
  uz->ctx = unzOpen(uz->filename);
  if(!uz->ctx) {
    uzlogger("Unable to open the zip file '%s'\n", uz->filename);
    uz_close(uz);
    return NULL;
  }
  /* Get info about the zip file */
  if(unzGetGlobalInfo(uz->ctx, &uz->ginfo) != UNZ_OK) {
    uzlogger("Unable to read the global info related to the '%s' zip file\n", uz->filename);
    uz_close(uz);
    return NULL;
  }
  return uz;
}

/**
 * @fn _Bool uz_is_valid(uz_t uz)
 * @brief Check if the inut pointer is valid.
 * @param uz Pointer to test.
 * @return 1 the pointer is valid else 0.
 */
_Bool uz_is_valid(uz_t uz) {
  return uz && uz->magic == UZ_MAGIC && uz->ctx;
}

/**
 * @fn void uz_close(uz_t uz)
 * @brief Close the zip context.
 * @param uz ZIP context.
 */
void uz_close(uz_t uz) {
  if(uz_is_valid(uz)) {
    if(uz->ctx) unzClose(uz->ctx), uz->ctx = NULL;
    uz->magic = 0;
    free(uz);
  }
}

/**
 * @fn _Bool uz_is_dir(uz_t uz, char* path)
 * @brief Test if the input path os a directry.
 * @param uz ZIP context.
 * @param path Path to test.
 * @return 1 if the current path is a directory else 0.
 */
_Bool uz_is_dir(uz_t uz, char* path) {
  if(!path) return 0;
  return path[strlen(path) - 1] == uz->dir_delimiter;
}

/**
 * @fn int uz_get_contents(uz_t uz, uz_file_content_fct uz_file_content)
 * @brief Unzip the ZIP files.
 * @param uz ZIP context.
 * @param uz_file_content Callback to received the uncompressed file datas.
 * @return -1 on failure else 0.
 */
int uz_get_contents(uz_t uz, uz_file_content_fct uz_file_content) {
  uLong i;
  struct uzentry_s entry;
  int ret = 0;
  if(!uz_is_valid(uz)) {
    uzlogger("Invalid zip pointer!\n");
    return -1;
  }
  if(!uz_file_content) {
    uzlogger("Invalid file content callback!\n");
    return -1;
  }
  
  /* Loop to list all files */
  for(i = 0; i < uz->ginfo.number_entry; i++) {
    memset(&entry, 0, sizeof(struct uzentry_s));
    /* Get info about current file. */
    if(unzGetCurrentFileInfo(uz->ctx, &entry.info, entry.name, FILENAME_MAX, NULL, 0, NULL, 0) != UNZ_OK) {
      uzlogger("Could not read file info from the zip file '%s'\n", uz->filename);
      ret = -1;
      break;
    }
    entry.isdir = uz_is_dir(uz, entry.name);
    if(entry.isdir) 
      uz_file_content(uz, entry);
    else {
      // Entry is a file, so extract it.
      if(unzOpenCurrentFile(uz->ctx) != UNZ_OK) {
	uzlogger("Could not open file '%s' into the zip file '%s'\n", entry.name, uz->filename);
	ret = -1;
	break;
      }

      int error = UNZ_OK;
      entry.content = (char*)malloc(entry.info.uncompressed_size);
      if(!entry.content) {
	uzlogger("Unable to alloc a memory for the content of the zipped file '%s'\n", entry.name);
	ret = -1;
	break;
      }
      /* read the file */
      do {
	error = unzReadCurrentFile(uz->ctx, entry.content, entry.info.uncompressed_size);
	if ( error < 0 ) {
	  uzlogger("Could not read file '%s' into the zip file '%s': %d\n", entry.name, uz->filename, error);
	  unzCloseCurrentFile(uz->ctx);
	  break;
	}
      } while ( error > 0 );
    }
    uz_file_content(uz, entry);
    free(entry.content); /* release content */
    unzCloseCurrentFile(uz->ctx);
    /* Go the the next entry listed in the zip file. */
    if((i+1) < uz->ginfo.number_entry) {
      if (unzGoToNextFile(uz->ctx) != UNZ_OK) {
  	uzlogger("Could not read next file from the zip file '%s'\n", uz->filename);
  	break;
      }
    }
  }
  if(ret) uz_close(uz);
  return ret;
}


/**
 * @fn void uz_set_dir_delimiter(uz_t uz, char delimiter)
 * @brief Change the directory delimiter.
 * @param uz ZIP context.
 * @param delimiter The new deimiter.
 */
void uz_set_dir_delimiter(uz_t uz, char delimiter) {
  if(!uz_is_valid(uz)) {
    uzlogger("Invalid zip pointer!\n");
    return;
  }
  uz->dir_delimiter = delimiter;
}


/**
 * @fn char uz_get_dir_delimiter(uz_t uz)
 * @brief Get the current directory delimiter.
 * @param uz ZIP context.
 * @return The deimiter.
 */
char uz_get_dir_delimiter(uz_t uz) {
  if(!uz_is_valid(uz)) {
    uzlogger("Invalid zip pointer!\n");
    return 0;
  }
  return uz->dir_delimiter;
}

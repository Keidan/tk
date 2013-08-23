/**
*******************************************************************************
* @file z.c
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
#include <tk/sys/z.h>
#include <tk/sys/log.h>

#define Z_MAGIC         0xC001214
#define Z_VALID(z)      (z && z->magic == Z_MAGIC)
#define Z_VALID_OPEN(z) (Z_VALID(z) && z->ctx)

#define zlogger(...) ({			\
    logger(LOG_ERR, __VA_ARGS__);		\
    fprintf(stderr, __VA_ARGS__);		\
  })

/**
 * @fn z_t z_new(void)
 * @brief Alloc a new z context.
 * @return The ZIP context else NULL on error.
 */
z_t z_new(void) {
  z_t z = NULL;
  if(!(z = (z_t)malloc(sizeof(struct z_ctx_s)))) {
    zlogger("Unable to alloc a memory for the zip context\n");
    return NULL;
  }
  memset(z, 0, sizeof(struct z_ctx_s));
  z->magic = Z_MAGIC;
  z->dir_delimiter = Z_DIR_DELIMITER;
  return z;
}

/**
 * @fn void z_delete(z_t z)
 * @brief Delete the ZIP context.
 * @param z The pointer to release.
 */
void z_delete(z_t z) {
  if(!Z_VALID(z)) return;
  z_close(z);
  z->magic = 0;
  free(z);
}

/**
 * @fn int z_open(z_t z, const char filename[FILENAME_MAX])
 * @brief Open a new ZIP file.
 * @param z The ZIP context.
 * @param filename ZIP file name.
 * @return 0 on success else -1.
 */
int z_open(z_t z, const char filename[FILENAME_MAX]) {
  if(!Z_VALID(z)) return -1;
  strcpy(z->filename, filename);
  /* Open the zip file */
  z->ctx = unzOpen(z->filename);
  if(!z->ctx) {
    zlogger("Unable to open the zip file '%s'\n", z->filename);
    z_close(z);
    return -1;
  }
  /* Get info about the zip file */
  if(unzGetGlobalInfo(z->ctx, &z->ginfo) != UNZ_OK) {
    zlogger("Unable to read the global info related to the '%s' zip file\n", z->filename);
    z_close(z);
    return -1;
  }
  return 0;
}

/**
 * @fn void z_close(z_t z)
 * @brief Close the zip context.
 * @param z ZIP context.
 */
void z_close(z_t z) {
  if(Z_VALID(z))
    if(z->ctx) unzClose(z->ctx), z->ctx = NULL;
}


/**
 * @fn int z_compress(z_t z, struct z_compress_s init, fifo_t files)
 * @brief Creation of a new ZIP file.
 * @param z The ZIP context.
 * @param init The init structure.
 * @param The file list.
 * @retunr 0 on success else -1.
 */
int z_compress(z_t z, struct z_compress_s init, fifo_t files) {
  return 0;
}


/**
 * @fn _Bool z_is_dir(z_t z, char* path)
 * @brief Test if the input path os a directry.
 * @param z ZIP context.
 * @param path Path to test.
 * @return 1 if the current path is a directory else 0.
 */
_Bool z_is_dir(z_t z, char* path) {
  if(!path) return 0;
  return path[strlen(path) - 1] == z->dir_delimiter;
}

/**
 * @fn int z_get_contents(z_t z, z_file_content_fct z_file_content)
 * @brief Unzip the ZIP files.
 * @param z ZIP context.
 * @param z_file_content Callback to received the uncompressed file datas.
 * @return -1 on failure else 0.
 */
int z_get_contents(z_t z, z_file_content_fct z_file_content) {
  uLong i;
  struct zentry_s entry;
  int ret = 0;
  if(!Z_VALID_OPEN(z)) {
    zlogger("Invalid zip pointer!\n");
    return -1;
  }
  if(!z_file_content) {
    zlogger("Invalid file content callback!\n");
    return -1;
  }
  
  /* Loop to list all files */
  for(i = 0; i < z->ginfo.number_entry; i++) {
    memset(&entry, 0, sizeof(struct zentry_s));
    /* Get info about current file. */
    if(unzGetCurrentFileInfo(z->ctx, &entry.info, entry.name, FILENAME_MAX, NULL, 0, NULL, 0) != UNZ_OK) {
      zlogger("Could not read file info from the zip file '%s'\n", z->filename);
      ret = -1;
      break;
    }
    entry.isdir = z_is_dir(z, entry.name);
    if(entry.isdir) 
      z_file_content(z, entry);
    else {
      // Entry is a file, so extract it.
      if(unzOpenCurrentFile(z->ctx) != UNZ_OK) {
	zlogger("Could not open file '%s' into the zip file '%s'\n", entry.name, z->filename);
	ret = -1;
	break;
      }

      int error = UNZ_OK;
      entry.content = (char*)malloc(entry.info.uncompressed_size);
      if(!entry.content) {
	zlogger("Unable to alloc a memory for the content of the zipped file '%s'\n", entry.name);
	ret = -1;
	break;
      }
      /* read the file */
      do {
	error = unzReadCurrentFile(z->ctx, entry.content, entry.info.uncompressed_size);
	if ( error < 0 ) {
	  zlogger("Could not read file '%s' into the zip file '%s': %d\n", entry.name, z->filename, error);
	  unzCloseCurrentFile(z->ctx);
	  break;
	}
      } while ( error > 0 );
    }
    z_file_content(z, entry);
    free(entry.content); /* release content */
    unzCloseCurrentFile(z->ctx);
    /* Go the the next entry listed in the zip file. */
    if((i+1) < z->ginfo.number_entry) {
      if (unzGoToNextFile(z->ctx) != UNZ_OK) {
  	zlogger("Could not read next file from the zip file '%s'\n", z->filename);
  	break;
      }
    }
  }
  if(ret) z_close(z);
  return ret;
}


/**
 * @fn void z_set_dir_delimiter(z_t z, char delimiter)
 * @brief Change the directory delimiter.
 * @param z ZIP context.
 * @param delimiter The new deimiter.
 */
void z_set_dir_delimiter(z_t z, char delimiter) {
  if(!Z_VALID(z)) {
    zlogger("Invalid zip pointer!\n");
    return;
  }
  z->dir_delimiter = delimiter;
}


/**
 * @fn char z_get_dir_delimiter(z_t z)
 * @brief Get the current directory delimiter.
 * @param z ZIP context.
 * @return The deimiter.
 */
char z_get_dir_delimiter(z_t z) {
  if(!Z_VALID(z)) {
    zlogger("Invalid zip pointer!\n");
    return 0;
  }
  return z->dir_delimiter;
}

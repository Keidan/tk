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
#include <tk/text/string.h>

#define Z_MAGIC               0xC001214
#define Z_VALID(z)            (z && z->magic == Z_MAGIC)
#define Z_VALID_OPEN(z)       (Z_VALID(z) && z->ctx)
#define Z_CAST(z)             ((struct z_s*)z)
#define Z_WRITE_BUFFER_SIZE   16384

struct z_s {
    int magic;
    char filename [FILENAME_MAX];        /* Zip file name */
    char dir_delimiter;
    unzFile ctx;                         /* Internale zip context */
    unz_global_info64 ginfo;               /* Global informations about the zip file */
};

/**
 * @fn static int z_get_file_crc(const char* filenameinzip, void*buf, unsigned long size_buf, unsigned long* result_crc)
 * @brief calculate the CRC32 of a file, because to encrypt a file, we need known the CRC32 of the file before
 * Source:minizip.c from zlib/contrib/minizip
 * @param filenameinzip The filename in the zip.
 * @param buf The content buffer.
 * @param size_buf The buffer size.
 * @param result_crc The crc.
 * @return ZIP_OK on success else ZIP_ERRNO.
 */
static int z_get_file_crc(const char* filenameinzip, void*buf, unsigned long size_buf, unsigned long* result_crc);



/**
 * @fn z_t z_new(void)
 * @brief Alloc a new z context.
 * @return The ZIP context else NULL on error.
 */
z_t z_new(void) {
  struct z_s* z = NULL;
  if(!(z = (struct z_s*)malloc(sizeof(struct z_s)))) {
    logger(LOG_ERR, "Unable to alloc a memory for the zip context\n");
    return NULL;
  }
  memset(z, 0, sizeof(struct z_s));
  z->magic = Z_MAGIC;
  z->dir_delimiter = Z_DIR_DELIMITER;
  return z;
}

/**
 * @fn void z_delete(z_t zip)
 * @brief Delete the ZIP context.
 * @param zip The pointer to release.
 */
void z_delete(z_t zip) {
  struct z_s* z = Z_CAST(zip);
  if(!Z_VALID(z)) return;
  z_close(z);
  z->magic = 0;
  free(z);
}

/**
 * @fn int z_open(z_t zip, const z_file_t filename)
 * @brief Open a new ZIP file.
 * @param zip The ZIP context.
 * @param filename ZIP file name.
 * @return 0 on success else -1.
 */
int z_open(z_t zip, const z_file_t filename) {
  struct z_s* z = Z_CAST(zip);
  if(!Z_VALID(z)) return -1;
  strcpy(z->filename, filename);
  /* Open the zip file */
  z->ctx = unzOpen64(z->filename);
  if(!z->ctx) {
    logger(LOG_ERR, "Unable to open the zip file '%s'\n", z->filename);
    z_close(z);
    return -1;
  }
  /* Get info about the zip file */
  if(unzGetGlobalInfo64(z->ctx, &z->ginfo) != UNZ_OK) {
    logger(LOG_ERR, "Unable to read the global info related to the '%s' zip file\n", z->filename);
    z_close(z);
    return -1;
  }
  return 0;
}

/**
 * @fn void z_close(z_t zip)
 * @brief Close the zip context.
 * @param zip ZIP context.
 */
void z_close(z_t zip){
  struct z_s* z = Z_CAST(zip);
  if(Z_VALID_OPEN(z)) {
    if(z->ctx) unzClose(z->ctx), z->ctx = NULL;
  }
}


/**
 * @fn int z_compress(z_t zip, const z_file_t zname, const char* password, z_clevel_et level, _Bool append, _Bool exclude_path, fifo_t files, _Bool free_file_entry)
 * @brief Creation of a new ZIP file.
 * @param zip The ZIP context.
 * @param zname The zip file name.
 * @param password the zip password else NULL or empty.
 * @param level The compression level.
 * @param append Append mode.
 * @param exclude_path Exclude the file path.
 * @param files The file list.
 * @param free_file_entry Call free after each file entries?.
 * @retunr 0 on success else -1.
 */
int z_compress(z_t zip, const z_file_t zname, const char* password, z_clevel_et level, _Bool append, _Bool exclude_path, fifo_t files, _Bool free_file_entry) {
  struct z_s* z = Z_CAST(zip);
  char filename_try[FILE_MAXNAME+16];
  int size_buf = 0;
  void* buf = NULL;
  zipFile zf;

  size_buf = Z_WRITE_BUFFER_SIZE;
  buf = (void*)malloc(size_buf);
  if (!buf) {
    logger(LOG_ERR, "Error allocating memory\n");
    return -1;
  }

  strncpy(filename_try, zname, sizeof(z_file_t)-1);
  filename_try[sizeof(z_file_t)] = 0;
  if(!string_indexof(filename_try, ".") == -1)
    strcat(filename_try, ".zip");

  zf = zipOpen64(filename_try, (append) ? 2 : 0);
  if (!zf) {
    free(buf);
    logger(LOG_ERR, "Error opening %s\n", filename_try);
    return -1;
  } else
    logger(LOG_DEBUG, "Creating %s\n", filename_try);
  
  while(!fifo_empty(files)) {
     char* filenameinzip = fifo_pop(files);
     logger(LOG_DEBUG, "Trying to add file '%s'\n", filenameinzip);
     FILE * fin;
     int size_read;
     const char *savefilenameinzip;
     zip_fileinfo zi;
     unsigned long crc_file = 0;
     int zip64 = 0;
     memset(&zi, 0, sizeof(zip_fileinfo));
     file_time(filenameinzip, (struct tm*)&zi.tmz_date);

     if(password != NULL && strlen(password))
       if(z_get_file_crc(filenameinzip, buf, size_buf, &crc_file) != ZIP_OK) {
	 zipClose(zf, NULL);
	 free(buf);
	 logger(LOG_ERR, "Error getting the crc for the file %s\n", filenameinzip);
	 if(free_file_entry) free(filenameinzip);
	 return -1;
       }

     zip64 = file_is_large_file(filenameinzip);
     /* The path name saved, should not include a leading slash. */
     /*if it did, windows/xp and dynazip couldn't read the zip file. */
     savefilenameinzip = filenameinzip;
     while(savefilenameinzip[0] == z->dir_delimiter)
       savefilenameinzip++;

     /*should the zip file contain any path at all?*/
     if(exclude_path) {
       const char *tmpptr;
       const char *lastslash = 0;
       for(tmpptr = savefilenameinzip; *tmpptr; tmpptr++) {
	 if(*tmpptr == z->dir_delimiter)
	   lastslash = tmpptr;
       }
       if(lastslash)
	 savefilenameinzip = lastslash+1; // base filename follows last slash.
     }

     if(zipOpenNewFileInZip3_64(zf, savefilenameinzip, &zi,
				NULL, 0, NULL, 0, NULL /* comment*/,
				(level != 0) ? Z_DEFLATED : 0, level,0,
				-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
				(password != NULL && strlen(password)) ? password : NULL, crc_file, zip64) != ZIP_OK) {
	 zipClose(zf, NULL);
	 free(buf);
	 logger(LOG_ERR, "Error in opening %s in zipfile\n", filenameinzip);
	 if(free_file_entry) free(filenameinzip);
	 return -1;
     }

     fin = fopen64(filenameinzip, "rb");
     if(!fin) {
       zipCloseFileInZip(zf);
       zipClose(zf, NULL);
       free(buf);
       logger(LOG_ERR, "Error in opening %s for reading\n", filenameinzip);
       if(free_file_entry) free(filenameinzip);
       return -1;
     }
     do {
       size_read = (int)fread(buf,1,size_buf,fin);
       if(size_read < size_buf)
	 if(!feof(fin)) {
	   logger(LOG_ERR, "Error in reading %s\n",filenameinzip);
	   break;
	 }

       if (size_read > 0) {
	 if(zipWriteInFileInZip(zf, buf, size_read) < 0)  {
	   logger(LOG_ERR, "Error in writing %s in the zipfile\n", filenameinzip);
	   break;
	 }
       }
     } while(size_read > 0);

     if(fin) fclose(fin);
     if(zipCloseFileInZip(zf) != ZIP_OK) {
       logger(LOG_ERR, "Error in closing %s in the zipfile\n", filenameinzip);
       if(free_file_entry) free(filenameinzip);
       break;
     }
     if(free_file_entry) free(filenameinzip);
  }
  if(zipClose(zf, NULL) != ZIP_OK)
    logger(LOG_ERR, "Error in closing %s\n",filename_try);
  free(buf);

  return 0;
}


/**
 * @fn _Bool z_is_dir(z_t zip, char* path)
 * @brief Test if the input path os a directry.
 * @param zip ZIP context.
 * @param path Path to test.
 * @return 1 if the current path is a directory else 0.
 */
_Bool z_is_dir(z_t zip, char* path) {
  if(!path) return 0;
  struct z_s* z = Z_CAST(zip);
  return path[strlen(path) - 1] == z->dir_delimiter;
}

/**
 * @fn int z_uncompress(z_t zip, const char* password, z_uncompress_callback_fct callback)
 * @brief Unzip the ZIP files.
 * @param zip ZIP context.
 * @param password The zip password else NULL or empty.
 * @param callback Callback to received the uncompressed file datas.
 * @return -1 on failure else 0.
 */
int z_uncompress(z_t zip, const char* password, z_uncompress_callback_fct callback) {
  uLong i;
  struct zentry_s entry;
  int ret = 0;
  struct z_s* z = Z_CAST(zip);
  if(!Z_VALID_OPEN(z)) {
    logger(LOG_ERR, "Invalid zip pointer!\n");
    return -1;
  }
  if(!callback) {
    logger(LOG_ERR, "Invalid file content callback!\n");
    return -1;
  }
  
  /* Loop to list all files */
  for(i = 0; i < z->ginfo.number_entry; i++) {
    memset(&entry, 0, sizeof(struct zentry_s));
    /* Get info about current file. */
    if(unzGetCurrentFileInfo64(z->ctx, &entry.info, entry.name, FILENAME_MAX, NULL, 0, NULL, 0) != UNZ_OK) {
      logger(LOG_ERR, "Could not read file info from the zip file '%s'\n", z->filename);
      ret = -1;
      break;
    }
    entry.isdir = z_is_dir(z, entry.name);
    if(entry.isdir) 
      callback(z, entry);
    else {
      // Entry is a file, so extract it.
      if(!password || !strlen(password)) {
	if(unzOpenCurrentFile(z->ctx) != UNZ_OK) {
	  logger(LOG_ERR, "Could not open file '%s' into the zip file '%s'\n", entry.name, z->filename);
	  ret = -1;
	  break;
	}
      } else  {
	if(unzOpenCurrentFilePassword(z->ctx, password) != UNZ_OK) {
	  logger(LOG_ERR, "Could not open file '%s' into the zip file '%s'\n", entry.name, z->filename);
	  ret = -1;
	  break;
	}
      }

      int error = UNZ_OK;
      entry.content = (char*)malloc(entry.info.uncompressed_size);
      if(!entry.content) {
	logger(LOG_ERR, "Unable to alloc a memory for the content of the zipped file '%s'\n", entry.name);
	ret = -1;
	break;
      }
      /* read the file */
      do {
	error = unzReadCurrentFile(z->ctx, entry.content, entry.info.uncompressed_size);
	if ( error < 0 ) {
	  logger(LOG_ERR, "Could not read file '%s' into the zip file '%s': %d\n", entry.name, z->filename, error);
	  unzCloseCurrentFile(z->ctx);
	  break;
	}
      } while ( error > 0 );
    }
    callback(z, entry);
    free(entry.content); /* release content */
    unzCloseCurrentFile(z->ctx);
    /* Go the the next entry listed in the zip file. */
    if((i+1) < z->ginfo.number_entry) {
      if (unzGoToNextFile(z->ctx) != UNZ_OK) {
  	logger(LOG_ERR, "Could not read next file from the zip file '%s'\n", z->filename);
  	break;
      }
    }
  }
  if(ret) z_close(z);
  return ret;
}


/**
 * @fn int z_get_global_zinfo(z_t zip, unz_global_info *ginfo)
 * @brief Get the global zip info (open must be all).
 * @param zip ZIP context.
 * @param ginfo The result infos.
 * @return -1 on error else 0.
 */
int z_get_global_zinfo(z_t zip, unz_global_info *ginfo) {
  struct z_s* z = Z_CAST(zip);
  if(!Z_VALID_OPEN(z)) {
    logger(LOG_ERR, "Invalid zip pointer!\n");
    return -1;
  }
  memcpy(ginfo, &z->ginfo, sizeof(unz_global_info));
  return 0;
}


/**
 * @fn void z_set_dir_delimiter(z_t zip, char delimiter)
 * @brief Change the directory delimiter.
 * @param zip ZIP context.
 * @param delimiter The new deimiter.
 */
void z_set_dir_delimiter(z_t zip, char delimiter) {
  struct z_s* z = Z_CAST(zip);
  if(!Z_VALID(z)) {
    logger(LOG_ERR, "Invalid zip pointer!\n");
    return;
  }
  z->dir_delimiter = delimiter;
}


/**
 * @fn char z_get_dir_delimiter(z_t zip)
 * @brief Get the current directory delimiter.
 * @param zip ZIP context.
 * @return The deimiter.
 */
char z_get_dir_delimiter(z_t zip) {
  struct z_s* z = Z_CAST(zip);
  if(!Z_VALID(z)) {
    logger(LOG_ERR, "Invalid zip pointer!\n");
    return 0;
  }
  return z->dir_delimiter;
}

/**
 * @fn static int z_get_file_crc(const char* filenameinzip, void*buf, unsigned long size_buf, unsigned long* result_crc)
 * @brief calculate the CRC32 of a file, because to encrypt a file, we need known the CRC32 of the file before
 * Source:minizip.c from zlib/contrib/minizip
 * @param filenameinzip The filename in the zip.
 * @param buf The content buffer.
 * @param size_buf The buffer size.
 * @param result_crc The crc.
 * @return ZIP_OK on success else ZIP_ERRNO.
 */
static int z_get_file_crc(const char* filenameinzip, void*buf, unsigned long size_buf, unsigned long* result_crc) {
  unsigned long calculate_crc = 0;
  int err = ZIP_OK;
  FILE * fin = fopen64(filenameinzip,"rb");

  unsigned long size_read = 0;
  unsigned long total_read = 0;
  if(!fin)
    err = ZIP_ERRNO;

  if(err == ZIP_OK)
    do {
      err = ZIP_OK;
      size_read = (int)fread(buf, 1, size_buf, fin);
      if(size_read < size_buf)
	if(!feof(fin)) {
	  logger(LOG_ERR, "Error in reading %s\n",filenameinzip);
	  err = ZIP_ERRNO;
	}
      if(size_read > 0)
	calculate_crc = crc32(calculate_crc, buf, size_read);
      total_read += size_read;

    } while ((err == ZIP_OK) && (size_read > 0));
  if (fin) fclose(fin);

  *result_crc = calculate_crc;
  logger(LOG_DEBUG, "file %s crc %lx\n", filenameinzip, calculate_crc);
  return err;
}

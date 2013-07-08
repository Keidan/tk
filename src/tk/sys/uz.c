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
 * @brief Ouvre un fichier ZIP.
 * @param filename Chemin du fichier ZIP.
 * @return Le context ZIP ou NULL en cas d'erreur.
 */
uz_t uz_open(const char filename[FILENAME_MAX]) {
  uz_t uz = NULL;
  if(!(uz = (uz_t)malloc(sizeof(struct uz_ctx_s)))) {
    uzlogger("Unable to alloc a memory for the zip file '%s'\n", filename);
    return NULL;
  }
  memset(uz, 0, sizeof(struct uz_ctx_s));
  uz->magic = UZ_MAGIC;
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
 * @brief Test si le pointeur est valide.
 * @param uz Context ZIP.
 * @return 0 si il n'est pas valide sinon 1.
 */
_Bool uz_is_valid(uz_t uz) {
  return uz && uz->magic == UZ_MAGIC && uz->ctx;
}


/**
 * @fn void uz_close(uz_t uz)
 * @brief Ferme le context ZIP.
 * @param uz Context ZIP.
 */
void uz_close(uz_t uz) {
  if(uz_is_valid(uz)) {
    if(uz->ctx) unzClose(uz->ctx), uz->ctx = NULL;
    uz->magic = 0;
    free(uz);
  }
}

/**
 * @fn _Bool uz_is_dir(char* path)
 * @brief Test si le fichier est un repertoire ou non.
 * @param path fichier a tester.
 * @return 0 si il n'est pas un repertoire sinon 1.
 */
_Bool uz_is_dir(char* path) {
  if(!path) return 0;
  return path[strlen(path) - 1] == UZ_DIR_DELIMITER;
}


/**
 * @fn int uz_get_contents(uz_t uz, uz_file_content_fct uz_file_content)
 * @brief Recuperation et decompression des fichiers du ZIP.
 * @param uz Context ZIP.
 * @param uz_file_content Fonction appelee apres decompression d'un fichier.
 * @return -1 en cas d'erreur sinon 0.
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
    entry.isdir = uz_is_dir(entry.name);
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

/**
 *******************************************************************************
 * @file sysutils.h
 * @author Keidan
 * @date 03/04/2013
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
#ifndef __SYSUTILS_H__
  #define __SYSUTILS_H__

  #include <stdio.h>

  #define SYSUTILS_1KB   0x400
  #define SYSUTILS_1MB   0x100000
  #define SYSUTILS_1GB   0x40000000


  /**
   * @def SYSUTILS_MAX_SSIZE
   * @brief Taille max du string utilise avec la fonction "sysutils_size_to_string"
   * @see sysutils_size_to_string
   */
  #define SYSUTILS_MAX_SSIZE 15

  typedef enum { SYSUTILS_UNIT_BYTE, SYSUTILS_UNIT_KBYTES, SYSUTILS_UNIT_MBYTES, SYSUTILS_UNIT_GBYTES} sysutils_unit_et;

  void sysutils_get_proc_filename(char filename[FILENAME_MAX], int pid, const char* file);

  long sysutils_get_page_size();
  double sysutils_get_page_size_in(sysutils_unit_et unit);
  long sysutils_get_phy_pages();
  long sysutils_get_available_phy_pages();
  long sysutils_get_nprocessors_configured();
  long sysutils_get_nprocessors_online();
  unsigned long sysutils_get_phy_memory_size();
  double sysutils_get_phy_memory_size_in(sysutils_unit_et unit);
  long long sysutils_jiffies_to_microsecond(long long jiffies);

  /**
   * @fn long sysutils_fsize(FILE* file)
   * @brief Recupere la taille du fichier.
   * @param file Taille.
   * @return Long.
   */
  long sysutils_fsize(FILE* file);


  /**
   * @fn void sysutils_size_to_string(long size, char ssize[SYSUTILS_MAX_SSIZE])
   * @brief Convertie une taille en string avec l'unite.
   * @param size Taille.
   * @param ssize Output
   */
  void sysutils_size_to_string(long size, char ssize[SYSUTILS_MAX_SSIZE]);
#endif /* __SYSUTILS_H__ */

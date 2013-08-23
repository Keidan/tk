/**
 *******************************************************************************
 * @file stools.h
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
#ifndef __STOOLS_H__
  #define __STOOLS_H__

  #include <stdio.h>
  #include <stdint.h>
  #include <tk/sys/log.h>

  #define wicked_cast(TYPE, VALUE) \
    (((union { __typeof__(VALUE) src; TYPE dest; }){ .src = VALUE }).dest)
  #define is_little_endian() wicked_cast(unsigned char, 1u)
  #define is_big_endian() !wicked_cast(unsigned char, 1u)

  /**
   * @def STOOLS_1KO
   * @brief Value of 1 Kb in octets
   * @see bns_utils_size_to_string
   */
  #define STOOLS_1KB   0x400

  /**
   * @def STOOLS_1MO
   * @brief Value of 1 Mb in octets
   * @see bns_utils_size_to_string
   */
  #define STOOLS_1MB   0x100000

  /**
   * @def STOOLS_1GO
   * @brief Value of 1 Gb in octets
   * @see bns_utils_size_to_string
   */
  #define STOOLS_1GB   0x40000000


  /**
   * @def STOOLS_MAX_SSIZE
   * @brief Maximum size of the string used by the "stools_size_to_string" function.
   * @see stools_size_to_string
   */
  #define STOOLS_MAX_SSIZE 15

  typedef enum { STOOLS_UNIT_BYTE, STOOLS_UNIT_KBYTES, STOOLS_UNIT_MBYTES, STOOLS_UNIT_GBYTES} stools_unit_et;

  /**
   * @fn void stools_get_proc_filename(char filename[FILENAME_MAX], int pid, const char* file)
   * @brief add /proc/the pid parameters/the file parameter into filename.
   * @param filename The result file name.
   * @param pid The process pid.
   * @param file The file we need.
   */
  void stools_get_proc_filename(char filename[FILENAME_MAX], int pid, const char* file);

  /**
   * @fn long stools_get_page_size()
   * @brief Get the page size
   * @return long
   */
  long stools_get_page_size();

  /**
   * @fn double stools_get_page_size_in(stools_unit_et unit)
   * @brief Get the page size in a specific format.
   * @param unit The conversion unit.
   * @return double
   */
  double stools_get_page_size_in(stools_unit_et unit);

  /**
   * @fn long stools_get_phy_pages()
   * @brief The number of pages of physical memory. Note that it is possible for the product of this value and the value of _SC_PAGESIZE to overflow.
   * @return long
   */
  long stools_get_phy_pages();

  /**
   * @fn long stools_get_available_phy_pages()
   * @brief The number of currently available pages of physical memory.
   * @return long
   */
  long stools_get_available_phy_pages();

  /**
   * @fn long stools_get_nprocessors_configured()
   * @brief The number of processors configured.
   * @return long
   */
  long stools_get_nprocessors_configured();

  /**
   * @fn long stools_get_nprocessors_online()
   * @brief The number of processors currently online (available).
   * @return long
   */
  long stools_get_nprocessors_online();

  /**
   * @fn  unsigned long stools_get_phy_memory_size()
   * @brief Get the pysical memory size
   * @return unsigned long
   */
  unsigned long stools_get_phy_memory_size();

  /**
   * @fn double stools_get_phy_memory_size_in(stools_unit_et unit)
   * @brief Get the pysical memory size in a specific format.
   * @param unit The conversion unit.
   * @return double
   */
  double stools_get_phy_memory_size_in(stools_unit_et unit);

  /**
   * @fn long long stools_jiffies_to_microsecond(long long jiffies)
   * @brief Convert jiffies to microsecond
   * @return long long
   */
  long long stools_jiffies_to_microsecond(long long jiffies);

  /**
   * @fn void stools_size_to_string(long size, char ssize[STOOLS_MAX_SSIZE])
   * @brief Convert a size into a string (with unit).
   * @param size Size.
   * @param ssize Output
   */
  void stools_size_to_string(long size, char ssize[STOOLS_MAX_SSIZE]);
#endif /* __STOOLS_H__ */

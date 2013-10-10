/**
 *******************************************************************************
 * @file systools.h
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
#ifndef __SYSTOOLS_H__
  #define __SYSTOOLS_H__

  #include <stdio.h>
  #include <stdint.h>
  #include <tk/sys/log.h>

  #define wicked_cast(TYPE, VALUE) \
    (((union { __typeof__(VALUE) src; TYPE dest; }){ .src = VALUE }).dest)
  #define is_little_endian() wicked_cast(unsigned char, 1u)
  #define is_big_endian() !wicked_cast(unsigned char, 1u)

  /**
   * @fn unsigned long systools_msectime();
   * @brief Get the current time in ms.
   * @return The current time.
   */
  unsigned long systools_msectime();

  /**
   * @def SYSTOOLS_1KO
   * @brief Value of 1 Kb in octets
   * @see bns_utils_size_to_string
   */
  #define SYSTOOLS_1KB   0x400

  /**
   * @def SYSTOOLS_1MO
   * @brief Value of 1 Mb in octets
   * @see bns_utils_size_to_string
   */
  #define SYSTOOLS_1MB   0x100000

  /**
   * @def SYSTOOLS_1GO
   * @brief Value of 1 Gb in octets
   * @see bns_utils_size_to_string
   */
  #define SYSTOOLS_1GB   0x40000000


  /**
   * @def SYSTOOLS_MAX_SSIZE
   * @brief Maximum size of the string used by the "systools_size_to_string" function.
   * @see systools_size_to_string
   */
  #define SYSTOOLS_MAX_SSIZE 15

  typedef enum { SYSTOOLS_UNIT_BYTE, SYSTOOLS_UNIT_KBYTES, SYSTOOLS_UNIT_MBYTES, SYSTOOLS_UNIT_GBYTES} systools_unit_et;

  /**
   * @fn void systools_get_proc_filename(char filename[FILENAME_MAX], int pid, const char* file)
   * @brief add /proc/the pid parameters/the file parameter into filename.
   * @param filename The result file name.
   * @param pid The process pid.
   * @param file The file we need.
   */
  void systools_get_proc_filename(char filename[FILENAME_MAX], int pid, const char* file);

  /**
   * @fn long systools_get_page_size()
   * @brief Get the page size
   * @return long
   */
  long systools_get_page_size();

  /**
   * @fn double systools_get_page_size_in(systools_unit_et unit)
   * @brief Get the page size in a specific format.
   * @param unit The conversion unit.
   * @return double
   */
  double systools_get_page_size_in(systools_unit_et unit);

  /**
   * @fn long systools_get_phy_pages()
   * @brief The number of pages of physical memory. Note that it is possible for the product of this value and the value of _SC_PAGESIZE to overflow.
   * @return long
   */
  long systools_get_phy_pages();

  /**
   * @fn long systools_get_available_phy_pages()
   * @brief The number of currently available pages of physical memory.
   * @return long
   */
  long systools_get_available_phy_pages();

  /**
   * @fn long systools_get_nprocessors_configured()
   * @brief The number of processors configured.
   * @return long
   */
  long systools_get_nprocessors_configured();

  /**
   * @fn long systools_get_nprocessors_online()
   * @brief The number of processors currently online (available).
   * @return long
   */
  long systools_get_nprocessors_online();

  /**
   * @fn  unsigned long systools_get_phy_memory_size()
   * @brief Get the pysical memory size
   * @return unsigned long
   */
  unsigned long systools_get_phy_memory_size();

  /**
   * @fn double systools_get_phy_memory_size_in(systools_unit_et unit)
   * @brief Get the pysical memory size in a specific format.
   * @param unit The conversion unit.
   * @return double
   */
  double systools_get_phy_memory_size_in(systools_unit_et unit);

  /**
   * @fn long long systools_jiffies_to_microsecond(long long jiffies)
   * @brief Convert jiffies to microsecond
   * @return long long
   */
  long long systools_jiffies_to_microsecond(long long jiffies);

  /**
   * @fn void systools_size_to_string(long size, char ssize[SYSTOOLS_MAX_SSIZE])
   * @brief Convert a size into a string (with unit).
   * @param size Size.
   * @param ssize Output
   */
  void systools_size_to_string(long size, char ssize[SYSTOOLS_MAX_SSIZE]);
#endif /* __SYSTOOLS_H__ */

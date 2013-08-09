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
  #include <stdint.h>
  #include <tk/sys/log.h>


  /**
   * @def SYSUTILS_1KO
   * @brief Value of 1 Kb in octets
   * @see bns_utils_size_to_string
   */
  #define SYSUTILS_1KB   0x400

  /**
   * @def SYSUTILS_1MO
   * @brief Value of 1 Mb in octets
   * @see bns_utils_size_to_string
   */
  #define SYSUTILS_1MB   0x100000

  /**
   * @def SYSUTILS_1GO
   * @brief Value of 1 Gb in octets
   * @see bns_utils_size_to_string
   */
  #define SYSUTILS_1GB   0x40000000


  /**
   * @def SYSUTILS_MAX_SSIZE
   * @brief Maximum size of the string used by the "sysutils_size_to_string" function.
   * @see sysutils_size_to_string
   */
  #define SYSUTILS_MAX_SSIZE 15

  typedef enum { SYSUTILS_UNIT_BYTE, SYSUTILS_UNIT_KBYTES, SYSUTILS_UNIT_MBYTES, SYSUTILS_UNIT_GBYTES} sysutils_unit_et;

  /**
   * @typedef void (*signal_catch_fct)(void)
   * @brief signal catch callback.
   */
  typedef void (*signal_catch_fct)(void);

  /**
   * @fn void sysutils_exit_action(const struct log_s *linit, signal_catch_fct signal_catch)
   * @brief Add exit callback action (SIGINT & SIGTERM) and start the syslog managment.
   * @param linit Start syslog (if not NULL)
   * @param signal_catch The signal callback.
   */
  void sysutils_exit_action(const struct log_s *linit, signal_catch_fct signal_catch);

  /**
   * @fn void sysutils_get_proc_filename(char filename[FILENAME_MAX], int pid, const char* file)
   * @brief add /proc/the pid parameters/the file parameter into filename.
   * @param filename The result file name.
   * @param pid The process pid.
   * @param file The file we need.
   */
  void sysutils_get_proc_filename(char filename[FILENAME_MAX], int pid, const char* file);

  /**
   * @fn long sysutils_get_page_size()
   * @brief Get the page size
   * @return long
   */
  long sysutils_get_page_size();

  /**
   * @fn double sysutils_get_page_size_in(sysutils_unit_et unit)
   * @brief Get the page size in a specific format.
   * @param unit The conversion unit.
   * @return double
   */
  double sysutils_get_page_size_in(sysutils_unit_et unit);

  /**
   * @fn long sysutils_get_phy_pages()
   * @brief The number of pages of physical memory. Note that it is possible for the product of this value and the value of _SC_PAGESIZE to overflow.
   * @return long
   */
  long sysutils_get_phy_pages();

  /**
   * @fn long sysutils_get_available_phy_pages()
   * @brief The number of currently available pages of physical memory.
   * @return long
   */
  long sysutils_get_available_phy_pages();

  /**
   * @fn long sysutils_get_nprocessors_configured()
   * @brief The number of processors configured.
   * @return long
   */
  long sysutils_get_nprocessors_configured();

  /**
   * @fn long sysutils_get_nprocessors_online()
   * @brief The number of processors currently online (available).
   * @return long
   */
  long sysutils_get_nprocessors_online();

  /**
   * @fn  unsigned long sysutils_get_phy_memory_size()
   * @brief Get the pysical memory size
   * @return unsigned long
   */
  unsigned long sysutils_get_phy_memory_size();

  /**
   * @fn double sysutils_get_phy_memory_size_in(sysutils_unit_et unit)
   * @brief Get the pysical memory size in a specific format.
   * @param unit The conversion unit.
   * @return double
   */
  double sysutils_get_phy_memory_size_in(sysutils_unit_et unit);

  /**
   * @fn long long sysutils_jiffies_to_microsecond(long long jiffies)
   * @brief Convert jiffies to microsecond
   * @return long long
   */
  long long sysutils_jiffies_to_microsecond(long long jiffies);

  /**
   * @fn long sysutils_fsize(FILE* file)
   * @brief Getting the file size.
   * @param file The file.
   * @return Long.
   */
  long sysutils_fsize(FILE* file);

  /**
   * @fn void sysutils_size_to_string(long size, char ssize[SYSUTILS_MAX_SSIZE])
   * @brief Convert a size into a string (with unit).
   * @param size Size.
   * @param ssize Output
   */
  void sysutils_size_to_string(long size, char ssize[SYSUTILS_MAX_SSIZE]);
#endif /* __SYSUTILS_H__ */

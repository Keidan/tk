/**
*******************************************************************************
* @file stools.c
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
#include <tk/sys/stools.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/param.h>
#include <string.h>
#include <math.h>
#include <signal.h>

#define USEC_PER_SEC         1000000UL

/**
 * @fn void stools_size_to_string(long size, char ssize[STOOLS_MAX_SSIZE])
 * @brief Convert a size into a string (with unit).
 * @param size Size.
 * @param ssize Output
 */
void stools_size_to_string(long size, char ssize[STOOLS_MAX_SSIZE]) {
  memset(ssize, 0, STOOLS_MAX_SSIZE);
  float s = size;
  if(size < STOOLS_1KB)
    snprintf(ssize, STOOLS_MAX_SSIZE, "%ld octet%s", size, size > 1 ? "s" : "");
  else if(size < STOOLS_1MB)
    snprintf(ssize, STOOLS_MAX_SSIZE, "%ld Ko", (long)ceil(s/STOOLS_1KB));
  else if(size < STOOLS_1GB)
    snprintf(ssize, STOOLS_MAX_SSIZE, "%ld Mo", (long)ceil(s/STOOLS_1MB));
  else
    snprintf(ssize, STOOLS_MAX_SSIZE, "%ld Go",  (long)ceil(s/STOOLS_1GB));
}

/**
 * @fn void stools_get_proc_filename(char filename[FILENAME_MAX], int pid, const char* file)
 * @brief add /proc/the pid parameters/the file parameter into filename.
 * @param filename The result file name.
 * @param pid The process pid.
 * @param file The file we need.
 */
void stools_get_proc_filename(char filename[FILENAME_MAX], int pid, const char* file) {
  snprintf(filename, FILENAME_MAX, "/proc/%d/%s", pid, file);
}

/**
 * @fn long long stools_jiffies_to_microsecond(long long jiffies)
 * @brief Convert jiffies to microsecond
 * @return long long
 */
long long stools_jiffies_to_microsecond(long long jiffies) {
  long hz = sysconf(_SC_CLK_TCK);
  if (hz <= USEC_PER_SEC && !(USEC_PER_SEC % hz))
    return (USEC_PER_SEC / hz) * jiffies;
  else if (hz > USEC_PER_SEC && !(hz % USEC_PER_SEC))
    return (jiffies + (hz / USEC_PER_SEC) - 1) / (hz / USEC_PER_SEC);
  else
    return (jiffies * USEC_PER_SEC) / hz;
}

/**
 * @fn double stools_get_page_size_in(stools_unit_et unit)
 * @brief Get the page size in a specific format.
 * @param unit The conversion unit.
 * @return double
 */
double stools_get_page_size_in(stools_unit_et unit) {
  switch(unit) {
    case STOOLS_UNIT_BYTE:
      return (double)stools_get_page_size();
    case STOOLS_UNIT_KBYTES:
      return (double)stools_get_page_size() / STOOLS_1KB;
    case STOOLS_UNIT_MBYTES:
      return (double)stools_get_page_size() / STOOLS_1MB;
    case STOOLS_UNIT_GBYTES:
      return (double)stools_get_page_size() / STOOLS_1GB;
  }
  return 0.0;
}

/**
 * @fn long stools_get_page_size()
 * @brief Get the page size
 * @return long
 */
long stools_get_page_size() {
  return sysconf(_SC_PAGE_SIZE);
}

/**
 * @fn long stools_get_phy_pages()
 * @brief The number of pages of physical memory. Note that it is possible for the product of this value and the value of _SC_PAGESIZE to overflow.
 * @return long
 */
long stools_get_phy_pages() {
  return sysconf(_SC_PHYS_PAGES);
}

/**
 * @fn long stools_get_available_phy_pages()
 * @brief The number of currently available pages of physical memory.
 * @return long
 */
long stools_get_available_phy_pages() {
  return sysconf(_SC_AVPHYS_PAGES);
}

/**
 * @fn long stools_get_nprocessors_configured()
 * @brief The number of processors configured.
 * @return long
 */
long stools_get_nprocessors_configured() {
  return sysconf(_SC_NPROCESSORS_CONF);
}

/**
 * @fn long stools_get_nprocessors_online()
 * @brief The number of processors currently online (available).
 * @return long
 */
long stools_get_nprocessors_online() {
  return sysconf(_SC_NPROCESSORS_ONLN);
}

/**
 * @fn  unsigned long stools_get_phy_memory_size()
 * @brief Get the pysical memory size
 * @return unsigned long
 */
unsigned long stools_get_phy_memory_size() {
  return stools_get_phy_pages() * stools_get_page_size();
}

/**
 * @fn double stools_get_phy_memory_size_in(stools_unit_et unit)
 * @brief Get the pysical memory size in a specific format.
 * @param unit The conversion unit.
 * @return double
 */
double stools_get_phy_memory_size_in(stools_unit_et unit) {
  switch(unit) {
    case STOOLS_UNIT_BYTE:
      return (double)stools_get_phy_memory_size();
    case STOOLS_UNIT_KBYTES:
      return (double)stools_get_phy_memory_size() / STOOLS_1KB;
    case STOOLS_UNIT_MBYTES:
      return (double)stools_get_phy_memory_size() / STOOLS_1MB;
    case STOOLS_UNIT_GBYTES:
      return (double)stools_get_phy_memory_size() / STOOLS_1GB;
  }
  return 0.0;
}

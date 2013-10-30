/**
*******************************************************************************
* @file systools.c
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
#include <tk/sys/systools.h>
#include <tk/utils/stringbuffer.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>
#include <signal.h>

#define USEC_PER_SEC         1000000UL

/**
 * @fn int systools_exec(const char* fmt, ...)
 * @brief Execute a system command
 * @param fmt The command and/or the format.
 * @param ... The parameters.
 * @return see "man 3 system"
 */
int systools_exec(const char* fmt, ...) {
 va_list args;
 stringbuffer_t b = stringbuffer_new();
 va_start(args, fmt);
 stringbuffer_vprintf(b, fmt, args);
 va_end(args);
 int r = system(stringbuffer_to_str(b));
 stringbuffer_delete(b);
 return r;
}
/**
 * @fn unsigned long systools_msectime();
 * @brief Get the current time in ms.
 * @return The current time.
 */
unsigned long systools_msectime() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return ((t.tv_sec) * 1000 + t.tv_usec/1000);
}



/**
 * @fn void systools_size_to_string(long size, char ssize[SYSTOOLS_MAX_SSIZE])
 * @brief Convert a size into a string (with unit).
 * @param size Size.
 * @param ssize Output
 */
void systools_size_to_string(long size, char ssize[SYSTOOLS_MAX_SSIZE]) {
  memset(ssize, 0, SYSTOOLS_MAX_SSIZE);
  float s = size;
  if(size < SYSTOOLS_1KB)
    snprintf(ssize, SYSTOOLS_MAX_SSIZE, "%ld octet%s", size, size > 1 ? "s" : "");
  else if(size < SYSTOOLS_1MB)
    snprintf(ssize, SYSTOOLS_MAX_SSIZE, "%ld Ko", (long)ceil(s/SYSTOOLS_1KB));
  else if(size < SYSTOOLS_1GB)
    snprintf(ssize, SYSTOOLS_MAX_SSIZE, "%ld Mo", (long)ceil(s/SYSTOOLS_1MB));
  else
    snprintf(ssize, SYSTOOLS_MAX_SSIZE, "%ld Go",  (long)ceil(s/SYSTOOLS_1GB));
}

/**
 * @fn void systools_get_proc_filename(char filename[FILENAME_MAX], int pid, const char* file)
 * @brief add /proc/the pid parameters/the file parameter into filename.
 * @param filename The result file name.
 * @param pid The process pid.
 * @param file The file we need.
 */
void systools_get_proc_filename(char filename[FILENAME_MAX], int pid, const char* file) {
  snprintf(filename, FILENAME_MAX, "/proc/%d/%s", pid, file);
}

/**
 * @fn long long systools_jiffies_to_microsecond(long long jiffies)
 * @brief Convert jiffies to microsecond
 * @return long long
 */
long long systools_jiffies_to_microsecond(long long jiffies) {
  long hz = sysconf(_SC_CLK_TCK);
  if (hz <= USEC_PER_SEC && !(USEC_PER_SEC % hz))
    return (USEC_PER_SEC / hz) * jiffies;
  else if (hz > USEC_PER_SEC && !(hz % USEC_PER_SEC))
    return (jiffies + (hz / USEC_PER_SEC) - 1) / (hz / USEC_PER_SEC);
  else
    return (jiffies * USEC_PER_SEC) / hz;
}

/**
 * @fn double systools_get_page_size_in(systools_unit_et unit)
 * @brief Get the page size in a specific format.
 * @param unit The conversion unit.
 * @return double
 */
double systools_get_page_size_in(systools_unit_et unit) {
  switch(unit) {
    case SYSTOOLS_UNIT_BYTE:
      return (double)systools_get_page_size();
    case SYSTOOLS_UNIT_KBYTES:
      return (double)systools_get_page_size() / SYSTOOLS_1KB;
    case SYSTOOLS_UNIT_MBYTES:
      return (double)systools_get_page_size() / SYSTOOLS_1MB;
    case SYSTOOLS_UNIT_GBYTES:
      return (double)systools_get_page_size() / SYSTOOLS_1GB;
  }
  return 0.0;
}

/**
 * @fn long systools_get_page_size()
 * @brief Get the page size
 * @return long
 */
long systools_get_page_size() {
  return sysconf(_SC_PAGE_SIZE);
}

/**
 * @fn long systools_get_phy_pages()
 * @brief The number of pages of physical memory. Note that it is possible for the product of this value and the value of _SC_PAGESIZE to overflow.
 * @return long
 */
long systools_get_phy_pages() {
  return sysconf(_SC_PHYS_PAGES);
}

/**
 * @fn long systools_get_available_phy_pages()
 * @brief The number of currently available pages of physical memory.
 * @return long
 */
long systools_get_available_phy_pages() {
  return sysconf(_SC_AVPHYS_PAGES);
}

/**
 * @fn long systools_get_nprocessors_configured()
 * @brief The number of processors configured.
 * @return long
 */
long systools_get_nprocessors_configured() {
  return sysconf(_SC_NPROCESSORS_CONF);
}

/**
 * @fn long systools_get_nprocessors_online()
 * @brief The number of processors currently online (available).
 * @return long
 */
long systools_get_nprocessors_online() {
  return sysconf(_SC_NPROCESSORS_ONLN);
}

/**
 * @fn  unsigned long systools_get_phy_memory_size()
 * @brief Get the pysical memory size
 * @return unsigned long
 */
unsigned long systools_get_phy_memory_size() {
  return systools_get_phy_pages() * systools_get_page_size();
}

/**
 * @fn double systools_get_phy_memory_size_in(systools_unit_et unit)
 * @brief Get the pysical memory size in a specific format.
 * @param unit The conversion unit.
 * @return double
 */
double systools_get_phy_memory_size_in(systools_unit_et unit) {
  switch(unit) {
    case SYSTOOLS_UNIT_BYTE:
      return (double)systools_get_phy_memory_size();
    case SYSTOOLS_UNIT_KBYTES:
      return (double)systools_get_phy_memory_size() / SYSTOOLS_1KB;
    case SYSTOOLS_UNIT_MBYTES:
      return (double)systools_get_phy_memory_size() / SYSTOOLS_1MB;
    case SYSTOOLS_UNIT_GBYTES:
      return (double)systools_get_phy_memory_size() / SYSTOOLS_1GB;
  }
  return 0.0;
}

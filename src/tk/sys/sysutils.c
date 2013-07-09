/**
 *******************************************************************************
 * @file sysutils.c
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
#include <tk/sys/sysutils.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/param.h>
#include <string.h>
#include <math.h>

#define USEC_PER_SEC         1000000UL

/**
 * @fn long sysutils_fsize(FILE* file)
 * @brief Getting the file size.
 * @param file The file.
 * @return Long.
 */
long sysutils_fsize(FILE* file) {
  long size = 0L, old = 0L;
  if (file) {
    old = ftell(file);
    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    fseek(file, old, SEEK_SET);
  }
  return size;
}

/**
 * @fn void sysutils_size_to_string(long size, char ssize[SYSUTILS_MAX_SSIZE])
 * @brief Convert a size into a string (with unit).
 * @param size Size.
 * @param ssize Output
 */
void sysutils_size_to_string(long size, char ssize[SYSUTILS_MAX_SSIZE]) {
  memset(ssize, 0, SYSUTILS_MAX_SSIZE);
  float s = size;
  if(size < SYSUTILS_1KB)
    snprintf(ssize, SYSUTILS_MAX_SSIZE, "%ld octet%s", size, size > 1 ? "s" : "");
  else if(size < SYSUTILS_1MB)
    snprintf(ssize, SYSUTILS_MAX_SSIZE, "%ld Ko", (long)ceil(s/SYSUTILS_1KB));
  else if(size < SYSUTILS_1GB)
    snprintf(ssize, SYSUTILS_MAX_SSIZE, "%ld Mo", (long)ceil(s/SYSUTILS_1MB));
  else
    snprintf(ssize, SYSUTILS_MAX_SSIZE, "%ld Go",  (long)ceil(s/SYSUTILS_1GB));
}

void sysutils_get_proc_filename(char filename[FILENAME_MAX], int pid, const char* file) {
  snprintf(filename, FILENAME_MAX, "/proc/%d/%s", pid, file);
}


long long sysutils_jiffies_to_microsecond(long long jiffies) {
  long hz = sysconf(_SC_CLK_TCK);
  if (hz <= USEC_PER_SEC && !(USEC_PER_SEC % hz))
    return (USEC_PER_SEC / hz) * jiffies;
  else if (hz > USEC_PER_SEC && !(hz % USEC_PER_SEC))
    return (jiffies + (hz / USEC_PER_SEC) - 1) / (hz / USEC_PER_SEC);
  else
    return (jiffies * USEC_PER_SEC) / hz;
}

double sysutils_get_page_size_in(sysutils_unit_et unit) {
  switch(unit) {
    case SYSUTILS_UNIT_BYTE:
      return (double)sysutils_get_page_size();
    case SYSUTILS_UNIT_KBYTES:
      return (double)sysutils_get_page_size() / SYSUTILS_1KB;
    case SYSUTILS_UNIT_MBYTES:
      return (double)sysutils_get_page_size() / SYSUTILS_1MB;
    case SYSUTILS_UNIT_GBYTES:
      return (double)sysutils_get_page_size() / SYSUTILS_1GB;
  }
  return 0.0;
}

long sysutils_get_page_size() {
  return sysconf(_SC_PAGE_SIZE);
}

long sysutils_get_phy_pages() {
  return sysconf(_SC_PHYS_PAGES);
}

long sysutils_get_available_phy_pages() {
  return sysconf(_SC_AVPHYS_PAGES);
}

long sysutils_get_nprocessors_configured() {
  return sysconf(_SC_NPROCESSORS_CONF);
}

long sysutils_get_nprocessors_online() {
  return sysconf(_SC_NPROCESSORS_ONLN);
}

unsigned long sysutils_get_phy_memory_size() {
  return sysutils_get_phy_pages() * sysutils_get_page_size();
}

double sysutils_get_phy_memory_size_in(sysutils_unit_et unit) {
  switch(unit) {
    case SYSUTILS_UNIT_BYTE:
      return (double)sysutils_get_phy_memory_size();
    case SYSUTILS_UNIT_KBYTES:
      return (double)sysutils_get_phy_memory_size() / SYSUTILS_1KB;
    case SYSUTILS_UNIT_MBYTES:
      return (double)sysutils_get_phy_memory_size() / SYSUTILS_1MB;
    case SYSUTILS_UNIT_GBYTES:
      return (double)sysutils_get_phy_memory_size() / SYSUTILS_1GB;
  }
  return 0.0;
}

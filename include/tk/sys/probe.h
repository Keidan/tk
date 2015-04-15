/**
*******************************************************************************
* @file probe.h
* @author Keidan
* @date 11/10/2013
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
#ifndef __PROBE_H__
  #define __PROBE_H__

  #include <tk/io/file.h>


  /**
   * @fn int probe_get_binary(file_name_t probe)
   * @brief Get th e real path of the modprobe application.
   * @param probe The result.
   * @return -1 on error else 0 on success.
   */
  int probe_get_binary(file_name_t probe);

  /**
   * @fn int probe_insert(const char *modname, const char *modprobe, _Bool quiet)
   * @brief Insert a kernel module.
   * @param modname The module name.
   * @param modprobe modprobe application path (probe_get_modprobe executed if NULL).
   * @param quiet Quiet mode.
   * @return -1 on error else 0 on success.
   */
  int probe_insert(const char *modname, const char *modprobe, _Bool quiet);

#endif /* __PROBE_H__ */

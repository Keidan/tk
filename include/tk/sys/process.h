/**
 *******************************************************************************
 * @file process.h
 * @author Keidan
 * @date 27/11/2013
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
#ifndef __PROCESS_H__
  #define __PROCESS_H__

  #include <tk/utils/stringbuffer.h>

  /**
   * @fn stringbuffer_t process_execute(char* fmt, ...)
   * @brief Execute a command in a pipe.
   * @param fmt The command to execute.
   * @param ... The command args.
   * @return The result else NULL on error (for non NULL returns stringbuffer_delete is required).
   */
  stringbuffer_t process_execute(char* fmt, ...);


#endif /* __PROCESS_H__ */

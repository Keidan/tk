/**
*******************************************************************************
* @file udp.h
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
#ifndef __UDP_H__
  #define __UDP_H__

  /**
   * @fn udp_t udp_new()
   * @brief Open an UDP connection.
   * @return The UDP context else NULL on error.
   */
  udp_t udp_new();

  /**
   * @fn void udp_delete(udp_t udp)
   * @brief release the udp context.
   * @param udp The context.
   */
  void udp_delete(udp_t udp);


#endif /* __UDP_H__ */

/**
*******************************************************************************
* @file netitun.h
* @author Keidan
* @date 29/10/2013
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
#ifndef __NETTUN_H__
  #define __NETTUN_H__

  #include <tk/io/file.h>


  typedef enum {
    NETTUN_TUN,
    NETTUN_TAP
  } nettun_type_t;

  struct nettun_s {
      file_name_t name;
      file_name_t owner;
      file_name_t group;
      nettun_type_t type;
  };


  /**
   * @fn int nettun_create(struct nettun_s* nt)
   * @brief Create a tun/tap device.
   * @param nt The device context.
   * @return -1 on error else 0 on success.
   */
  int nettun_create(struct nettun_s* nt);

  /**
   * @fn int nettun_remove(struct nettun_s* nt)
   * @brief Remove the tun/tap device.
   * @param nt The device context.
   * @return -1 on error else 0 on success.
   */
  int nettun_remove(struct nettun_s* nt);

#endif /* __NETTUN_H__ */

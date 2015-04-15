/**
*******************************************************************************
* @file netroute.h
* @author Keidan
* @date 23/10/2013
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
#ifndef __NETROUTE_H__
  #define __NETROUTE_H__

  #include <tk/utils/llist.h>
  #include <tk/io/net/netiface.h>

  #define NETROUTE_IP4_ALL "*.*.*.*"


  struct netroute_item_add_s {
      netiface_name_t src_iface;
      netiface_ip4_t dst;
      netiface_ip4_t dst_mask;
      netiface_ip4_t gateway;
  };

  struct netroute_item_ls_s {
      netiface_name_t iface;
      netiface_ip4_t src;
      netiface_ip4_t dst;
      netiface_ip4_t gateway;
  };

  /**
   * @fn llist_t netroute_ls()
   * @brief List all routes.
   * @return The route list (struct netroute_item_ls_s)
   */
  llist_t netroute_ls();

  /**
   * @fn int netroute_add(struct netroute_item_add_s *item)
   * @bief add a new route entry.
   * @param item The netroute to add.
   * @return -1 on error else 0 on succes.
   */
  int netroute_add(struct netroute_item_add_s *item);

  /**
   * @fn int netroute_del(struct netroute_item_add_s *item)
   * @bief Delete a route entry.
   * @param item The netroute to del.
   * @return -1 on error else 0 on succes.
   */
  int netroute_del(struct netroute_item_add_s *item);
#endif /* __NETROUTE_H__ */

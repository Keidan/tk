/**
*******************************************************************************
* @file netfilter.h
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
#ifndef __NETFILTER_H__
  #define __NETFILTER_H__

  #include <linux/types.h>
  #include <libiptc/libiptc.h>
  #include <tk/io/net/netiface.h>
  #include <tk/utils/llist.h>

  #ifndef NETFILTER_TABLE
    #define NETFILTER_TABLE "filter"
  #endif

  typedef __u16 netfilter_port_t[2];
  typedef char ipt_tablelabel [255];
  typedef char ipt_targetlabel [255];


  struct netfilter_rule_ls_s {
      netiface_ip4_t  src; /* Source IP */
      netiface_ip4_t  dst; /* Destination IP*/   
      __u8            proto; /* Transport layer protocol */
      ipt_chainlabel  chain;
      ipt_targetlabel target;
  };

  typedef void* netfilter_t;

  /**
   * @fn netfilter_t netfilter_new(ipt_tablelabel *tablename)
   * @brief Create the netfilter connection.
   * @param tablename Netfilter table name (see NETFILTER_TABLE).
   * @return The Netfilter handle else NULL on error.
   */
  netfilter_t netfilter_new(ipt_tablelabel tablename);

  /**
   * @fn void netfilter_delete(netfilter_t netf)
   * @brief Delete the netfilter connection.
   * @param netf Netfilter handle.
   */
  void netfilter_delete(netfilter_t netf);

  /**
   * @fn int netfilter_switch(netfilter_t *netf, ipt_tablelabel tablename)
   * @brief Switch the netfilte table.
   * @param netf The nex connection pointer (only if this function return 0)
   * @param tablename The table
   * @return -1 on error else 0 on success.
   */
  int netfilter_switch(netfilter_t *netf, ipt_tablelabel tablename);

  /**
   * @fn int netfilter_commit(netfilter_t netf)
   * @brief Commit pending changes to the kernel table.
   * @param netf Netfilter handle.
   * @return -1 on error else 0 on success.
   */
  int netfilter_commit(netfilter_t netf);

  /**
   * @fn int netfilter_clear(netfilter_t netf, const ipt_chainlabel chainlabel)
   * @brief Removes all entries from a chain
   * @param netf Netfilter handle.
   * @param chainlabel	name of the chain to flush
   * 			NOTE: libiptc.h: typedef char ipt_chainlabel[32]
   * @return -1 on error else 0 on success.
   */
  int netfilter_clear(netfilter_t netf, const ipt_chainlabel chainlabel);

  /**
   * @fn int netfilter_exists(netfilter_t netf, const ipt_chainlabel chainlabel)
   * @brief Checks whether a chain exists
   * @param netf The netfilter handle.
   * @param chainlabel	name of chain to check
   * 			NOTE: libiptc.h: typedef char ipt_chainlabel[32]
   * @return -1 on error, 0 the chain does not exists 1 the chain exists.
   */
  int netfilter_exists(netfilter_t netf, const ipt_chainlabel chainlabel);

  /**
   * @fn const char* netfilter_get_table_name(netfilter_t netf)
   * @brief Get the current table name.
   * @param netf The netfilter handle.
   * @return The name else NULL on error.
   */
  const char* netfilter_get_table_name(netfilter_t netf);

  /**
   * @fn void netfilter_parse_ports(netfilter_port_t from,  netfilter_port_t to)
   * @brief Compares two ports and changes their order if higher value is
   * in the first array element
   * @param from array containing the ports to check
   * @param to array containing the (swapped) ports
   */
  void netfilter_parse_ports(netfilter_port_t from,  netfilter_port_t to);

  /**
   * @fn llist_t netfilter_ls(netfilter_t netf, const ipt_chainlabel mchain)
   * @brief List all rules associated to the specified chain.
   * @param netf The netfilter context.
   * @param mchain The chain to list.
   * @return A list of (struct netfilter_rule_ls_s*)
   */
  llist_t netfilter_ls(netfilter_t netf, const ipt_chainlabel mchain);

#endif /* __NETFILTER_H__ */

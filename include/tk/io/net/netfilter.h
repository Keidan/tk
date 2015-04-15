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
  #include <netinet/in.h>
  #include <netinet/ip.h>
  #include <netinet/in.h>
  #include <netinet/ip_icmp.h>
  #include <netinet/tcp.h>
  #include <netinet/udp.h>
  #include <tk/io/net/netiface.h>
  #include <tk/utils/llist.h>

  #ifndef NETFILTER_TABLE
    #define NETFILTER_TABLE "filter"
  #endif

  #define NETFILTER_TCP IPPROTO_TCP
  #define NETFILTER_UDP IPPROTO_UDP

  typedef char ipt_chainlabel[32];
  typedef char ipt_tablelabel [255];
  typedef char ipt_targetlabel [255];
  typedef char netfilter_ip4cidr_t[19];

  struct netfilter_port_s {
      __u16 min;
      __u16 max;
  };

  struct netfilter_ip_rule_s {
      struct {
	  netfilter_ip4cidr_t ip;
	  netfilter_ip4cidr_t mask;
      } str;
      struct netfilter_port_s port;
   };

  struct netfilter_rule_s {
      struct netfilter_ip_rule_s src;
      struct netfilter_ip_rule_s dst;
      __u8 proto; /* Transport layer protocol */
      struct {
	  netiface_name_t input;
	  netiface_name_t output;
      } ifaces;
      ipt_targetlabel target;
      ipt_chainlabel chain;
  };



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
   * @fn llist_t netfilter_ls(netfilter_t netf, const ipt_chainlabel mchain)
   * @brief List all rules associated to the specified chain.
   * @param netf The netfilter context.
   * @param mchain The chain to list.
   * @return A list of (struct netfilter_rule_ls_s*)
   */
  llist_t netfilter_ls(netfilter_t netf, const ipt_chainlabel mchain);

  /**
   * @fn int netfilter_remove(netfilter_t netf, struct netfilter_rule_s* rule)
   * @brief Prepares a rule in order to be deleted from the Kernel:
   * - does basic sanity checks
   * - calls prepareEntry in order to get an ipt_entry
   * - removes the ipt_entry to kernel
   * @param netf Netfilter handle.
   * @param rule rule which shall be removed
   * @return -1 on error else 0 on success
 */
  int netfilter_remove(netfilter_t netf, struct netfilter_rule_s* rule);

  /**
   * @fn int netfilter_add(netfilter_t netf, struct netfilter_rule_s* rule)
   * @bief Prepares a rule in order to be added to the Kernel:
   * - does basic sanity checks
   * - calls prepareEntry in order to get an ipt_entry
   * - adds the ipt_entry to kernel
   * @param netf Ntfilter handle.
   * @param rule rule which shall be added
   * @return -1 on error else 0 on success
   **/
  int netfilter_add(netfilter_t netf, struct netfilter_rule_s* rule);

#endif /* __NETFILTER_H__ */

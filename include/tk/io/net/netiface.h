/**
*******************************************************************************
* @file netiface.h
* @author Keidan
* @date 23/09/2013
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
#ifndef __NETIFACE_H__
  #define __NETIFACE_H__

  #include <net/if.h>
  #include <net/if_arp.h>
  #include <net/ethernet.h>
  #include <tk/utils/htable.h>
  #include <stdio.h>
  #include <tk/io/net/netiface_status.h>

  #define IFACE_IS_PROMISC(flags)           (flags & IFF_PROMISC)
  #define IFACE_IS_NOTRAILERS(flags)        (flags & IFF_NOTRAILERS)
  #define IFACE_IS_DEBUG(flags)             (flags & IFF_DEBUG)
  #define IFACE_IS_LOOPBACK(flags)          (flags & IFF_LOOPBACK)
  #define IFACE_IS_UP(flags)                (flags & IFF_UP)
  #define IFACE_IS_BROADCAST(flags)         (flags & IFF_BROADCAST)
  #define IFACE_IS_MULTICAST(flags)         (flags & IFF_MULTICAST)
  #define IFACE_IS_MASTER(flags)            (flags & IFF_MASTER)
  #define IFACE_IS_SLAVE(flags)             (flags & IFF_SLAVE)
  #define IFACE_IS_PORTSEL(flags)           (flags & IFF_PORTSEL)
  #define IFACE_IS_AUTOMEDIA(flags)         (flags & IFF_AUTOMEDIA)
  #define IFACE_IS_DYNAMIC(flags)           (flags & IFF_DYNAMIC)
  #define IFACE_IS_RUNNING(flags)           (flags & IFF_RUNNING)
  #define IFACE_IS_POINTOPOINT(flags)       (flags & IFF_POINTOPOINT)
  #define IFACE_IS_NOARP(flags)             (flags & IFF_NOARP)


  #define IFACE_SET_PROMISC(flags, add)     netiface_flags_update(&flags, add, IFF_PROMISC)
  #define IFACE_SET_NOTRAILERS(flags, add)  netiface_flags_update(&flags, add, IFF_NOTRAILERS)
  #define IFACE_SET_DEBUG(flags, add)       netiface_flags_update(&flags, add, IFF_DEBUG)
  #define IFACE_SET_LOOPBACK(flags, add)    netiface_flags_update(&flags, add, IFF_LOOPBACK)
  #define IFACE_SET_UP(flags, add)          netiface_flags_update(&flags, add, IFF_UP)
  #define IFACE_SET_BROADCAST(flags, add)   netiface_flags_update(&flags, add, IFF_BROADCAST)
  #define IFACE_SET_MULTICAST(flags, add)   netiface_flags_update(&flags, add, IFF_MULTICAST)
  #define IFACE_SET_MASTER(flags, add)      netiface_flags_update(&flags, add, IFF_MASTER)
  #define IFACE_SET_SLAVE(flags, add)       netiface_flags_update(&flags, add, IFF_SLAVE)
  #define IFACE_SET_PORTSEL(flags, add)     netiface_flags_update(&flags, add, IFF_PORTSEL)
  #define IFACE_SET_AUTOMEDIA(flags, add)   netiface_flags_update(&flags, add, IFF_AUTOMEDIA)
  #define IFACE_SET_DYNAMIC(flags, add)     netiface_flags_update(&flags, add, IFF_DYNAMIC)
  #define IFACE_SET_RUNNING(flags, add)     netiface_flags_update(&flags, add, IFF_RUNNING)
  #define IFACE_SET_POINTOPOINT(flags, add) netiface_flags_update(&flags, add, IFF_POINTOPOINT)
  #define IFACE_SET_NOARP(flags, add)       netiface_flags_update(&flags, add, IFF_NOARP)


  typedef char netiface_name_t[IF_NAMESIZE];
  typedef char netiface_ip4_t[16];
  typedef char netiface_mac_t[18];
  typedef __u8 netiface_bmac_t[ETH_ALEN];

  typedef void* netiface_t;


  struct netiface_info_s {
      netiface_name_t name;
      netiface_ip4_t ip4;
      netiface_ip4_t bcast;
      netiface_ip4_t mask;
      netiface_mac_t mac;
      int family;
      int metric;
      int mtu;
      short int flags;
      int index; /* read only */
      struct iface_status_s status; /* read only */
  };
  typedef struct netiface_info_s *netiface_info_t;

  typedef enum {
    NETIFACE_LVL_RAW = 0,
    NETIFACE_LVL_TCP,
    NETIFACE_LVL_UDP
  } netiface_sock_level;

  typedef enum { 
    NETIFACE_KEY_NAME = 0, 
    NETIFACE_KEY_FD, 
    NETIFACE_KEY_INDEX 
  } netiface_key_type;

  #define netiface_list_get(ifaces, key) htable_lookup(ifaces, key)

  /**
   * @fn htable_t netiface_list_new(netiface_sock_level level, netiface_key_type type)
   * @brief List all available interfaces.
   * @param level Socket level.
   * @param type The table key type
   * @return The table list (key:see key type, value:netiface_t)
   */
  htable_t netiface_list_new(netiface_sock_level level, netiface_key_type type);

  /**
   * @fn void netiface_list_delete(htable_t table)
   * @brief Clear the iface list.
   * @param table The table to clear.
   */
  void netiface_list_delete(htable_t table);

  /**
   * @fn int netiface_bind(netiface_t iface)
   * @brief Bind to the iface.
   * @param iface The iface.
   * @return -1 on error else 0.
   */
  int netiface_bind(netiface_t iface);

  /**
   * @fn int netiface_get_index(netiface_t iface, int *index)
   * @brief Get the internal iface index.
   * @param iface The iface.
   * @param index The result index.
   * @return 0 on success else -1.
   */
  int netiface_get_index(netiface_t iface, int *index);

  /**
   * @fn int netiface_get_fd(netiface_t iface, int *fd)
   * @brief Get the internal iface fd.
   * @param iface The iface.
   * @param fd The result fd.
   * @return 0 on success else -1.
   */
  int netiface_get_fd(netiface_t iface, int *fd);

  /**
   * @fn int netiface_read(const netiface_t iface, netiface_info_t info)
   * @brief Read some informations from the iface.
   * @param iface The iface handle.
   * @param info The iface informations (no pointer allocated here).
   * @return -1 on error or if the iface is not found else 0 on success.
   */
  int netiface_read(const netiface_t iface, netiface_info_t info);

  /**
   * @fn int netiface_write(const netiface_t iface, const netiface_info_t info)
   * @brief Write some informations from the iface.
   * @param iface The iface handle.
   * @param info The iface informations.
   * @return -1 on error or if the iface is not found else 0 on success.
   */
  int netiface_write(const netiface_t iface, const netiface_info_t info);


  /**
   * @fn _Bool netiface_device_is_up(const netiface_t iface)
   * @brief Test if the current device is up.
   * @param fd Device FD.
   * @return 1 if up else 0..
   */
  _Bool netiface_device_is_up(const netiface_t iface);

  /**
   * @fn __u32 netiface_datas_available(int fd)
   * @brief Get the number of available datas to be read.
   * @param fd Socket FD.
   * @return Available datas.
   */
  __u32 netiface_datas_available(const netiface_t iface);

  /**
   * @fn void netiface_print(FILE* out, const struct netiface_info_s *info, _Bool up_only)
   * @brief Print iface informations.
   * @param out The output stream.
   * @param info Iface informations.
   * @param up_only Print only up device.
   */
  void netiface_print(FILE* out, const struct netiface_info_s *info, _Bool up_only);

  /**
   * @fn void netiface_flags_update(short int* flags, _Bool state, int flag)
   * @brief utils, update the flags value.
   * @param flags The flags list.
   * @param state Add or remove.
   * @param flag The flag to add or remove.
   */
  void netiface_flags_update(short int* flags, _Bool state, int flag);

  /**
   * @fn int netiface_up(const netiface_t iface)
   * @brief UP the iface.
   * @param iface the idface to up.
   * @return -1 on error else 0
   */
  int netiface_up(const netiface_t iface);
#endif  /* __NETIFACE_H__ */

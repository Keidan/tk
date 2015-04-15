/**
 *******************************************************************************
 * @file virtual_cache_item.h
 * @author Keidan
 * @date 12/08/2014
 * @par Project
 * tk
 *
 * @par Copyright
 * Copyright 2011-2014 Keidan, all right reserved
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
#ifndef __VIRTUAL_CACHE_ITEM_H__
  #define __VIRTUAL_CACHE_ITEM_H__

  #include <tk/utils/cache/cache_item.h>



  typedef void* virtual_cache_item_t;

  /**
   * @fn virtual_cache_item_t virtual_cache_item_new(cache_item_name_t name, struct cache_item_data_s item)
   * @brief Alloc a new virtual cache item.
   * @param name The item name.
   * @param item The item.
   * @return the new pointer, else null on error.
   */
  virtual_cache_item_t virtual_cache_item_new(cache_item_name_t name, struct cache_item_data_s item);

  /**
   * @fn void virtual_cache_item_delete(virtual_cache_item_t vci)
   * @brief Release the virtual cache item.
   * @param vci The virtual cache item pointer.
   */
  void virtual_cache_item_delete(virtual_cache_item_t vci);

  /**
   * @fn void virtual_cache_item_set_name(virtual_cache_item_t vci, cache_item_name_t name) 
   * @brief Change the item name.
   * @param vci The virtual cache item pointer.
   */
  void virtual_cache_item_set_name(virtual_cache_item_t vci, cache_item_name_t name);

  /**
   * @fn const char* virtual_cache_item_get_name(virtual_cache_item_t vci)
   * @brief Get the item name.
   * @param vci The virtual cache item pointer.
   */
  const char* virtual_cache_item_get_name(virtual_cache_item_t vci);

  /**
   * @fn void virtual_cache_item_update_count(virtual_cache_item_t vci) 
   * @brief Increment the item count.
   * @param vci The virtual cache item pointer.
   */
  void virtual_cache_item_update_count(virtual_cache_item_t vci);

  /**
   * @fn void virtual_cache_item_decrement_count(virtual_cache_item_t vci)
   * @brief Decrement the item count.
   * @param vci The virtual cache item pointer.
   */
  void virtual_cache_item_decrement_count(virtual_cache_item_t vci);

  /**
   * @fn long virtual_cache_item_get_count(virtual_cache_item_t vci) 
   * @brief Get the item count.
   * @param vci The virtual cache item pointer.
   */
  long virtual_cache_item_get_count(virtual_cache_item_t vci);

  /**
   * @fn void virtual_cache_item_get_item(virtual_cache_item_t vci, struct cache_item_data_s *item) 
   * @brief Get the virtual cache sub item.
   * @param vci The virtual cache item pointer.
   * @param item The output result (free(item->data) is required.
   */
  void virtual_cache_item_get_item(virtual_cache_item_t vci, struct cache_item_data_s *item);
  /**
   * @fn long int virtual_cache_item_get_item_length(virtual_cache_item_t vci)
   * @brief Get the virtual cache sub item length.
   * @param vci The virtual cache item pointer.
   * @return The length
   */
  long int virtual_cache_item_get_item_length(virtual_cache_item_t vci);

  /**
   * @fn void virtual_cache_item_set_item(virtual_cache_item_t vci, struct cache_item_data_s item)
   * @brief Change the virtual cache sub item.
   * @param vci The virtual cache item pointer.
   * @param item The new item.
   */
  void virtual_cache_item_set_item(virtual_cache_item_t vci, struct cache_item_data_s item);

  /**
   * @fn void virtual_cache_item_clear_item(virtual_cache_item_t vci)
   * @brief Clear the item object.
   * @param vci The virtual cache item pointer.
   */
  void virtual_cache_item_clear_item(virtual_cache_item_t vci);

#endif /* __VIRTUAL_CACHE_ITEM_H__ */

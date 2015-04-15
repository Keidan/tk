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
#ifndef __VIRTUAL_CACHE_H__
  #define __VIRTUAL_CACHE_H__

  #include <tk/utils/cache/virtual_cache_item.h>
  #include <tk/utils/llist.h>
  #include <tk/utils/htable.h>

  typedef void* virtual_cache_t;

  /**
   * @fn virtual_cache_t virtual_cache_new(void)
   * @brief Alloc a new virtual cache.
   * @return The virtual cache pointer else NULL on error.
   */
  virtual_cache_t virtual_cache_new(void);

  /**
   * @fn void virtual_cache_delete(virtual_cache_t vc)
   * @brief Release the virtual cache pointer.
   * @param vc The virtual cache pointer.
   */
  void virtual_cache_delete(virtual_cache_t vc);

  /**
   * @fn void virtual_cache_set_logs_enable(virtual_cache_t vc, _Bool b)
   * @brief Update the logs state.
   * @param vc The virtual cache pointer.
   * @param b The new state.
   */
  void virtual_cache_set_logs_enable(virtual_cache_t vc, _Bool b);

  /**
   * @fn _Bool virtual_cache_is_logs_enabled(virtual_cache_t vc)
   * @brief The the logs status.
   * @param vc The virtual cache pointer.
   * @return true/false
   */
  _Bool virtual_cache_is_logs_enabled(virtual_cache_t vc);

  /**
   * @fn _Bool virtual_cache_is_empty(virtual_cache_t vc)
   * @brief Test if the virtual cache is empty
   * @param vc The virtual cache pointer.
   * @return true/false
   */
  _Bool virtual_cache_is_empty(virtual_cache_t vc);

  /**
   * @fn void virtual_cache_set_resources_to_exclude(virtual_cache_t vc, cache_item_name_t resources_to_exclude)
   * @brief Exclude the resoureces if the name ends with a resource list (resources_to_exclude=res1,res2,res3,etc...)
   * @param vc The virtual cache pointer.
   * @param resources_to_exclude The resource to exclude
   */
  void virtual_cache_set_resources_to_exclude(virtual_cache_t vc, cache_item_name_t resources_to_exclude);

  /**
   * @fn htable virtual_cache_get_virtual_cache(virtual_cache_t vc)
   * @brief Get the virtual cache table
   * @param vc The virtual cache pointer.
   * @return the table (key:string_name, value: virtual_cache_item_t)
   */
  htable_t virtual_cache_get_virtual_cache(virtual_cache_t vc);

  /**
   * @fn void virtual_cache_clear(virtual_cache_t vc)
   * @brief Clear the virtual cache entries.
   * @param vc The virtual cache pointer.
   */
  void virtual_cache_clear(virtual_cache_t vc);

  /**
   * @fn _Bool virtual_cache_can_cache(virtual_cache_t vc, cache_item_name_t name)
   * @brief Test if the current name is cacheable.
   * @param vc The virtual cache pointer.
   * @param name The name to test.
   * @return true/false.
   */
  _Bool virtual_cache_can_cache(virtual_cache_t vc, cache_item_name_t name);

  /**
   * @fn _Bool virtual_cache_update(virtual_cache_t vc, cache_item_name_t name, cache_item_load_fct cache_item_load, struct cache_item_data_s *body)
   * @brief Update a cache item and add if the item is not found.
   * @param vc The virtual cache pointer.
   * @param name The item name.
   * @param cache_item_load The load callback.
   * @param body The output result.(free(item->data) is required)
   * @return false if the function failed, true else.
   */
  _Bool virtual_cache_update(virtual_cache_t vc, cache_item_name_t name, cache_item_load_fct cache_item_load, struct cache_item_data_s *body);

  /**
   * @fn void virtual_cache_remove(virtual_cache_t vc, cache_item_name_t name)
   * @brief Remove an item from the virtual cache.
   * @param vc The virtual cache pointer.
   * @param name The item name.
   */
  void virtual_cache_remove(virtual_cache_t vc, cache_item_name_t name);

  /**
   * @fn long int virtual_cache_get_size(virtual_cache_t vc)
   * @brief Get the total size of the virtual cache.
   * @param vc The virtual cache pointer.
   * @return the size.
   */
  long int virtual_cache_get_size(virtual_cache_t vc);

  /**
   * @fn llist_t virtual_cache_get_cache_max_size(virtual_cache_t vc, long int maxCacheSize)
   * @brief Get the max item size.
   * @param vc The virtual cache pointer.
   * @return the list of names.
   */
  llist_t virtual_cache_get_cache_max_size(virtual_cache_t vc, long int maxCacheSize);

  /**
   * @fn int virtual_cache_count_item(virtual_cache_t vc)
   * @brief Count the items into the virtual cache.
   * @param vc The virtual cache pointer.
   * @return The number of items.
   */
  int virtual_cache_count_item(virtual_cache_t vc);


#endif /* __VIRTUAL_CACHE_H__ */

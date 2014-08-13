/**
 *******************************************************************************
 * @file cache.h
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
#ifndef __CACHE_H__
  #define __CACHE_H__

  #include <tk/utils/cache/virtual_cache.h>

  #define MAX_CACHE_SIZE 8 * 1024 * 1024

  typedef void* cache_t;


  /**
   * @fn cache_t cache_new(cache_item_name_t resources_to_exclude)
   * @bried Create a new cache object.
   * @param resources_to_exclude The resources to exclude.
   * @return The cache pointer.
   */
  cache_t cache_new(cache_item_name_t resources_to_exclude);

  /**
   * @fn void cache_delete(cache_t cache)
   * @brief Clear the cache resourses.
   * @param cache The cache pointer.
   */
  void cache_delete(cache_t cache);

  /**
   * @fn void cache_set_logs_enable(cache_t cache, _Bool b)
   * @brief Activate or Desactivate the logs.
   * @param cache The cache pointer.
   * @param b The new status
   */
  void cache_set_logs_enable(cache_t cache, _Bool b);

  /**
   * @fn _Bool cache_is_logs_enabled(cache_t cache)
   * @brief Test if the logs are enabled
   * @param cache The cache pointer.
   * @return true/false.
   */
  _Bool cache_is_logs_enabled(cache_t cache);

  /**
   * @fn void cache_clear(cache_t cache)
   * @brief Clear the cache.
   * @param cache The cache pointer.
   */
  void cache_clear(cache_t cache);

  /**
   * @fn _Bool cache_invalidate(cache_t cache, cache_item_name_t name, cache_item_load_fct cache_item_load)
   * @brief Invalide a cache item.
   * @param cache The cache pointer.
   * @param name The item name.
   * @param cache_item_load The load callback.
   * @return true/false
   */
  _Bool cache_invalidate(cache_t cache, cache_item_name_t name, cache_item_load_fct cache_item_load);

  /**
   * @fn _Bool cache_contains_key(cache_t cache, cache_item_name_t name)
   * @brief Test if the cache contains a specific key.
   * @param cache The cache pointer.
   * @param name The key.
   * @return true/false.
   */
  _Bool cache_contains_key(cache_t cache, cache_item_name_t name);

  /**
   * @fn void cache_remove(cache_t cache, cache_item_name_t name)
   * @brief Remove an item from the cache.
   * @param cache The cache pointer.
   * @param name The item name.
   */
  void cache_remove(cache_t cache, cache_item_name_t name);

  /**
   * @fn _Bool cache_get_item(cache_t cache, cache_item_name_t name, cache_item_load_fct cache_item_load, struct cache_item_data_s *body)
   * @brief Get item from the cache.
   * @param cache The cache pointer.
   * @param name The item name.
   * @param cache_item_load The load function
   * @param body The result item.
   * @return false on error, true else.
   */
  _Bool cache_get_item(cache_t cache, cache_item_name_t name, cache_item_load_fct cache_item_load, struct cache_item_data_s *body);

  /**
   * @fn void cache_release_item(struct cache_item_data_s *body)
   * @brief Release item after a call to get_item
   * @param body The item.
   */
  void cache_release_item(struct cache_item_data_s *body);

  /**
   * @fn int cache_count_items(cache_t cache) 
   * @brief Count the number of items
   * @param cache The cache pointer.
   * @return The number.
   */
  int cache_count_items(cache_t cache);

  /**
   * @fn void cache_set_max_cache_size(cache_t cache, long int max_cache_size)
   * @brief Change the max size of the cache
   * @param cache The cache pointer.
   * @param max_cache_size The new size.
   */
  void cache_set_max_cache_size(cache_t cache, long int max_cache_size);

  /**
   * @fn long int cache_get_max_cache_size(cache_t cache)
   * @brief Get the max size of the cache.
   * @param cache The cache pointer.
   * @return The size
   */
  long int cache_get_max_cache_size(cache_t cache);

  /**
   * @fn long int cache_get_cache_size(cache_t cache)
   * @brief Get the current cache size.
   * @param cache The cache pointer.
   * @return The size.
   */
  long int cache_get_cache_size(cache_t cache);

#endif /* __CACHE_H__ */

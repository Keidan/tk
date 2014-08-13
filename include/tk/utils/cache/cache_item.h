/**
 *******************************************************************************
 * @file cache_item.h
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
#ifndef __CACHE_ITEM_H__
  #define __CACHE_ITEM_H__

  #define CACHE_ITEM_MAX_NAME_LENGTH 255

  typedef char cache_item_name_t[CACHE_ITEM_MAX_NAME_LENGTH];


  struct cache_item_data_s{
      char* data;
      long int length;
  };
 
  /**
   * @typedef cache_item_load_fct
   * @brief Load a named resource.
   * @param name The item name.
   * @param data The item content.
   */
  typedef void (*cache_item_load_fct)(cache_item_name_t  name, struct cache_item_data_s* data);

#endif /* __CACHE_ITEM_H__ */

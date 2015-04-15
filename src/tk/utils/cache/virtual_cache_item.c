/**
 *******************************************************************************
 * @file virtual_cache_item.c
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
#include <tk/utils/cache/virtual_cache_item.h>
#include <tk/sys/log.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct virtual_cache_item_s {
    cache_item_name_t name;
    long int count;
    struct cache_item_data_s item;
};

/**
 * @fn virtual_cache_item_t virtual_cache_item_new(cache_item_name_t name, struct cache_item_data_s item)
 * @brief Alloc a new virtual cache item.
 * @param name The item name.
 * @param item The item.
 * @return the new pointer, else null on error.
 */
virtual_cache_item_t virtual_cache_item_new(cache_item_name_t name, struct cache_item_data_s item) {
  struct virtual_cache_item_s *i = NULL;
  if((i = malloc(sizeof(struct virtual_cache_item_s))) == NULL) {
    logger(LOG_ERR, "not enough memory to alloc the virtual cache item pointer!");
    return NULL;
  }
  memset(i, 0, sizeof(struct virtual_cache_item_s));
  virtual_cache_item_set_name(i, name);
  virtual_cache_item_set_item(i, item);
  return i;
}

/**
 * @fn void virtual_cache_item_delete(virtual_cache_item_t vci)
 * @brief Release the virtual cache item.
 * @param vci The virtual cache item pointer.
 */
void virtual_cache_item_delete(virtual_cache_item_t vci) {
  struct virtual_cache_item_s *i = (struct virtual_cache_item_s *)vci;
  if(!i) return;
  virtual_cache_item_clear_item(vci);
  memset(i, 0, sizeof(struct virtual_cache_item_s));
  free(i);
}

/**
 * @fn void virtual_cache_item_set_name(virtual_cache_item_t vci, cache_item_name_t name) 
 * @brief Change the item name.
 * @param vci The virtual cache item pointer.
 */
void virtual_cache_item_set_name(virtual_cache_item_t vci, cache_item_name_t name) {
  struct virtual_cache_item_s *i = (struct virtual_cache_item_s *)vci;
  if(!i) return;
  strncpy(i->name, name, sizeof(cache_item_name_t));
}

/**
 * @fn const char* virtual_cache_item_get_name(virtual_cache_item_t vci)
 * @brief Get the item name.
 * @param vci The virtual cache item pointer.
 */
const char* virtual_cache_item_get_name(virtual_cache_item_t vci) {
  struct virtual_cache_item_s *i = (struct virtual_cache_item_s *)vci;
  if(!i) return NULL;
  return i->name;
}

/**
 * @fn void virtual_cache_item_update_count(virtual_cache_item_t vci) 
 * @brief Increment the item count.
 * @param vci The virtual cache item pointer.
 */
void virtual_cache_item_update_count(virtual_cache_item_t vci) {
  struct virtual_cache_item_s *i = (struct virtual_cache_item_s *)vci;
  if(!i) return;
  if(i->count < (LONG_MAX-1)) i->count += 2;
}

/**
 * @fn void virtual_cache_item_decrement_count(virtual_cache_item_t vci)
 * @brief Decrement the item count.
 * @param vci The virtual cache item pointer.
 */
void virtual_cache_item_decrement_count(virtual_cache_item_t vci) {
  struct virtual_cache_item_s *i = (struct virtual_cache_item_s *)vci;
  if(!i) return;
  if(i->count > LONG_MIN) i->count --;
}

/**
 * @fn long virtual_cache_item_get_count(virtual_cache_item_t vci) 
 * @brief Get the item count.
 * @param vci The virtual cache item pointer.
 */
long virtual_cache_item_get_count(virtual_cache_item_t vci) {
  struct virtual_cache_item_s *i = (struct virtual_cache_item_s *)vci;
  if(!i) return LONG_MIN;
  return i->count;
}

/**
 * @fn long int virtual_cache_item_get_item_length(virtual_cache_item_t vci)
 * @brief Get the virtual cache sub item length.
 * @param vci The virtual cache item pointer.
 * @return The length
 */
long int virtual_cache_item_get_item_length(virtual_cache_item_t vci) {
  struct virtual_cache_item_s *i = (struct virtual_cache_item_s *)vci;
  if(!i) return 0;
  return i->item.length;
}

/**
 * @fn void virtual_cache_item_get_item(virtual_cache_item_t vci, struct cache_item_data_s *item) 
 * @brief Get the virtual cache sub item.
 * @param vci The virtual cache item pointer.
 * @param item The output result (free(item->data) is required) .
 */
void virtual_cache_item_get_item(virtual_cache_item_t vci, struct cache_item_data_s *item) {
  struct virtual_cache_item_s *i = (struct virtual_cache_item_s *)vci;
  item->data = NULL;
  if(!i) return;
  item->length = i->item.length;
  if(i->item.data) {
    item->data = malloc(item->length + 1);
    bzero(item->data, item->length + 1);
    memcpy(item->data, i->item.data, item->length);
  }
}

/**
 * @fn void virtual_cache_item_set_item(virtual_cache_item_t vci, struct cache_item_data_s item)
 * @brief Change the virtual cache sub item.
 * @param vci The virtual cache item pointer.
 * @param item The new item.
 */
void virtual_cache_item_set_item(virtual_cache_item_t vci, struct cache_item_data_s item) {
  struct virtual_cache_item_s *i = (struct virtual_cache_item_s *)vci;
  if(!i) return;
  if(i->item.data != NULL) virtual_cache_item_clear_item(i);
  
  if(item.data && item.length != 0) {
    i->item.length = item.length;
    i->item.data = malloc(item.length + 1);
    bzero(i->item.data, item.length + 1);
    memcpy(i->item.data, item.data, item.length);
  }
}

/**
 * @fn void virtual_cache_item_clear_item(virtual_cache_item_t vci)
 * @brief Clear the item object.
 * @param vci The virtual cache item pointer.
 */
void virtual_cache_item_clear_item(virtual_cache_item_t vci) {
  struct virtual_cache_item_s *i = (struct virtual_cache_item_s *)vci;
  if(!i) return;
  if(i->item.data != NULL) {
    free(i->item.data), i->item.data = NULL;
    i->item.length = 0;
  }
}

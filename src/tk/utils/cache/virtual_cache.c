/**
 *******************************************************************************
 * @file virtual_cache.c
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
#include <tk/utils/cache/virtual_cache.h>
#include <tk/utils/string.h>
#include <tk/utils/stringtoken.h>
#include <tk/sys/log.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct virtual_cache_s {
    _Bool enable_logs;
    htable_t table;
    cache_item_name_t resources_to_exclude;
};

/**
 * @fn static void virtual_cache_decrement_counts(virtual_cache_t vc)
 * @brief Decraise all counts.
 * @param vc The virtual cache pointer.
 */
static void virtual_cache_decrement_counts(virtual_cache_t vc);

/**
 * @fn static _Bool virtual_cache_contains_key(virtual_cache_t vc, cache_item_name_t name)
 * @brief Test if the specific name is in the virtual cache.
 * @param vc The virtual cache pointer.
 * @param name The name to test.
 * @return true/false.
 */
static _Bool virtual_cache_contains_key(virtual_cache_t vc, cache_item_name_t name);


/**
 * @fn virtual_cache_t virtual_cache_new(void)
 * @brief Alloc a new virtual cache.
 * @return The virtual cache pointer else NULL on error.
 */
virtual_cache_t virtual_cache_new(void) {
  struct virtual_cache_s *v = NULL;
  if((v = malloc(sizeof(struct virtual_cache_s))) == NULL) {
    logger(LOG_ERR, "not enough memory to alloc the virtual cache pointer!");
    return NULL;
  }
  memset(v, 0, sizeof(struct virtual_cache_s));
  v->table = htable_new1(false);
  return v;
}

/**
 * @fn void virtual_cache_delete(virtual_cache_t vc)
 * @brief Release the virtual cache pointer.
 * @param vc The virtual cache pointer.
 */
void virtual_cache_delete(virtual_cache_t vc) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  if(!v) return;
  virtual_cache_clear(v);
  if(v->table) htable_delete(v->table), v->table = NULL;
  free(v);
}

/**
 * @fn void virtual_cache_set_logs_enable(virtual_cache_t vc, _Bool b)
 * @brief Update the logs state.
 * @param vc The virtual cache pointer.
 * @param b The new state.
 */
void virtual_cache_set_logs_enable(virtual_cache_t vc, _Bool b) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  if(!v) return;
  v->enable_logs = b;
}

/**
 * @fn _Bool virtual_cache_is_logs_enabled(virtual_cache_t vc)
 * @brief The the logs status.
 * @param vc The virtual cache pointer.
 * @return true/false
 */
_Bool virtual_cache_is_logs_enabled(virtual_cache_t vc) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  if(!v) return false;
  return v->enable_logs;
}

/**
 * @fn _Bool virtual_cache_is_empty(virtual_cache_t vc)
 * @brief Test if the virtual cache is empty
 * @param vc The virtual cache pointer.
 * @return true/false
 */
_Bool virtual_cache_is_empty(virtual_cache_t vc) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  if(!v || !v->table) return true;
  return v->table->kcount == 0;
}

/**
 * @fn void virtual_cache_set_resources_to_exclude(virtual_cache_t vc, cache_item_name_t resources_to_exclude)
 * @brief Exclude the resoureces if the name ends with a resource list (resources_to_exclude=res1,res2,res3,etc...)
 * @param vc The virtual cache pointer.
 * @param resources_to_exclude The resource to exclude
 */
void virtual_cache_set_resources_to_exclude(virtual_cache_t vc, cache_item_name_t resources_to_exclude) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  if(!v) return;
  strncpy(v->resources_to_exclude, resources_to_exclude, sizeof(cache_item_name_t));
}

/**
 * @fn htable virtual_cache_get_virtual_cache(virtual_cache_t vc)
 * @brief Get the virtual cache table
 * @param vc The virtual cache pointer.
 * @return the table (key:string_name, value: virtual_cache_item_t)
 */
htable_t virtual_cache_get_virtual_cache(virtual_cache_t vc) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  if(!v) return NULL;
  return v->table;
}

/**
 * @fn void virtual_cache_clear(virtual_cache_t vc)
 * @brief Clear the virtual cache entries.
 * @param vc The virtual cache pointer.
 */
void virtual_cache_clear(virtual_cache_t vc) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  virtual_cache_item_t item;
  if(!v) return;
  if(v->table && v->table->kcount != 0) {
    int count, i;
    char** keys = NULL;
    count = htable_get_keys(v->table, &keys);
    for(i = 0; i < count; i++) {
      item = htable_lookup(v->table, keys[i]);
      virtual_cache_item_delete(item);
    }
    if(keys) free(keys);
    htable_clear(v->table);
  }
}

/**
 * @fn _Bool virtual_cache_can_cache(virtual_cache_t vc, cache_item_name_t name)
 * @brief Test if the current name is cacheable.
 * @param vc The virtual cache pointer.
 * @param name The name to test.
 * @return true/false.
 */
_Bool virtual_cache_can_cache(virtual_cache_t vc, cache_item_name_t name) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  if(!v) return false;
  _Bool ret = true;
  if(!strlen(v->resources_to_exclude)) return ret;
  stringtoken_t tokens = stringtoken_init(v->resources_to_exclude, ",");
  char* temp;
  while(stringtoken_has_more_tokens(tokens)) {
    temp = stringtoken_next_token(tokens);
    if(string_endswith(name, temp)) {
      free(temp);
      ret = false;
      break;
    }
    free(temp);
  }
  stringtoken_release(tokens);
  return ret;
}



/**
 * @fn _Bool virtual_cache_update(virtual_cache_t vc, cache_item_name_t name, cache_item_load_fct cache_item_load, struct cache_item_data_s *body)
 * @brief Update a cache item and add if the item is not found.
 * @param vc The virtual cache pointer.
 * @param name The item name.
 * @param cache_item_load The load callback.
 * @param body The output result.(free(item->data) is required)
 * @return false if the function failed, true else.
 */
_Bool virtual_cache_update(virtual_cache_t vc, cache_item_name_t name, cache_item_load_fct cache_item_load, struct cache_item_data_s *body) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  virtual_cache_item_t *item;
  struct cache_item_data_s temp;
  if(!v || !body) return false;
  body->data = NULL, body->length = -1;
  if (strlen(name) == 0) {
    if(v->enable_logs) logger(LOG_ERR, "virtual_cache_update -> Null or empty item name\n");
    return false;
  }
  if (virtual_cache_contains_key(vc, name)) {
    item = htable_lookup(v->table, name);
    if(v->enable_logs)
      logger(LOG_DEBUG, "virtual_cache_update -> Load item %s from cache, item size %ld bytes\n", 
	     name, virtual_cache_item_get_item_length(item));
    virtual_cache_decrement_counts(vc);
    virtual_cache_item_update_count(item);

    virtual_cache_item_get_item(item, body);
    if (body->data == NULL) {
      if(v->enable_logs)
	logger(LOG_ERR, "virtual_cache_update -> Unable to load item %s from cache\n", name);
      return false;
    }
    return true;
  } else {// item not found.
    bzero(&temp, sizeof(struct cache_item_data_s));
    cache_item_load(name, &temp);
    if(v->enable_logs)
      logger(LOG_DEBUG, "virtual_cache_update -> Load item %s from resources, item size %ld bytes\n", name, temp.length);
    if (!temp.data) {
      if(v->enable_logs)
	logger(LOG_ERR, "virtual_cache_update -> Unable to load item %s from resources\n", name);
      return false;
    }
    if(virtual_cache_can_cache(vc, name)) {
      item = virtual_cache_item_new(name, temp);
      free(temp.data);
      virtual_cache_decrement_counts(vc);
      virtual_cache_item_update_count(item);
      htable_add(v->table, name, item, sizeof(virtual_cache_item_t));
      virtual_cache_item_get_item(item, body);
    } else {
      body->length = temp.length;
      body->data = malloc(temp.length + 1);
      bzero(body->data, temp.length + 1);
      memcpy(body->data, temp.data, temp.length);
    }
    return true;
  }
}


/**
 * @fn void virtual_cache_remove(virtual_cache_t vc, cache_item_name_t name)
 * @brief Remove an item from the virtual cache.
 * @param vc The virtual cache pointer.
 * @param name The item name.
 */
void virtual_cache_remove(virtual_cache_t vc, cache_item_name_t name) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  if(!v) return;
  virtual_cache_item_t item = htable_lookup(v->table, name);
  if(item) {
    virtual_cache_item_delete(item);
    htable_remove(v->table, name);
  }
}

/**
 * @fn long int virtual_cache_get_size(virtual_cache_t vc)
 * @brief Get the total size of the virtual cache.
 * @param vc The virtual cache pointer.
 * @return the size.
 */
long int virtual_cache_get_size(virtual_cache_t vc) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  if(!v) return 0L;
  long int sum = 0L;
  int count, i;
  char** keys = NULL;
  virtual_cache_item_t item = NULL;
  count = htable_get_keys(v->table, &keys);
  for(i = 0; i < count; i++) {
    item = htable_lookup(v->table, keys[i]);
    sum += virtual_cache_item_get_item_length(item);
  }
  if(keys) free(keys);
  return sum;
}

/**
 * @fn llist_t virtual_cache_get_cache_max_size(virtual_cache_t vc, long int maxCacheSize)
 * @brief Get the max item size.
 * @param vc The virtual cache pointer.
 * @return the list of names.
 */
llist_t virtual_cache_get_cache_max_size(virtual_cache_t vc, long int maxCacheSize) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  if(!v) return NULL;
  llist_t li = NULL;
  long int sum = 0L, len;
  int count, i;
  char** keys = NULL;
  virtual_cache_item_t item = NULL;
  count = htable_get_keys(v->table, &keys);
  for(i = 0; i < count; i++) {
    item = htable_lookup(v->table, keys[i]);
    len = virtual_cache_item_get_item_length(item);
    if((sum + len) > maxCacheSize)
      continue;
    sum += len;
    li = llist_pushback(li, (void*)virtual_cache_item_get_name(item));
  }
  if(keys) free(keys);
  return li;
}

/**
 * @fn int virtual_cache_count_item(virtual_cache_t vc)
 * @brief Count the items into the virtual cache.
 * @param vc The virtual cache pointer.
 * @return The number of items.
 */
int virtual_cache_count_item(virtual_cache_t vc) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  if(!v) return 0;
  return v->table->kcount;
}

/**
 * @fn static void virtual_cache_decrement_counts(virtual_cache_t vc)
 * @brief Decraise all counts.
 * @param vc The virtual cache pointer.
 */
static void virtual_cache_decrement_counts(virtual_cache_t vc) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  if(!v) return;
  int count, i;
  char** keys = NULL;
  virtual_cache_item_t item;
  count = htable_get_keys(v->table, &keys);
  for(i = 0; i < count; i++) {
    item = htable_lookup(v->table, keys[i]);
    virtual_cache_item_decrement_count(item);
  }
  if(keys) free(keys);
}


/**
 * @fn static _Bool virtual_cache_contains_key(virtual_cache_t vc, cache_item_name_t name)
 * @brief Test if the specific name is in the virtual cache.
 * @param vc The virtual cache pointer.
 * @param name The name to test.
 * @return true/false.
 */
static _Bool virtual_cache_contains_key(virtual_cache_t vc, cache_item_name_t name) {
  struct virtual_cache_s *v = (struct virtual_cache_s*)vc;
  if(!v) return false;
  return htable_has_key(v->table, name);
}

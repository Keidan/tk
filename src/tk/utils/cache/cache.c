/**
 *******************************************************************************
 * @file cache.c
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
#include <tk/utils/cache/cache.h>
#include <tk/sys/log.h>
#include <stdlib.h>
#include <string.h>


struct cache_s {
    _Bool enable_logs;
    long int max_cache_size;
    long int cache_size;
    htable_t real;
    virtual_cache_t virtual;
};

/**
 * @fn static _Bool cache_find_in_cache_size(llist_t li, cache_item_name_t name)
 * @brief Find a specific item into the cache size list.
 * @param li The list.
 * @param name The item name.
 * @return true/false.
 */
static _Bool cache_find_in_cache_size(llist_t li, cache_item_name_t name);





/**
 * @fn cache_t cache_new(cache_item_name_t resources_to_exclude)
 * @bried Create a new cache object.
 * @param resources_to_exclude The resources to exclude.
 * @return The cache pointer.
 */
cache_t cache_new(cache_item_name_t resources_to_exclude) {
  struct cache_s *c = NULL;
  if((c = malloc(sizeof(struct cache_s))) == NULL) {
    logger(LOG_ERR, "not enough memory to alloc the cache pointer!");
    return NULL;
  }
  memset(c, 0, sizeof(struct cache_s));
  c->real = htable_new1(false);
  c->virtual = virtual_cache_new();
  c->max_cache_size = MAX_CACHE_SIZE;
  c->cache_size = 0L;
  c->enable_logs = false;
  virtual_cache_set_resources_to_exclude(c->virtual, resources_to_exclude);
  return c;
}

/**
 * @fn void cache_delete(cache_t cache)
 * @brief Clear the cache resourses.
 * @param cache The cache pointer.
 */
void cache_delete(cache_t cache) {
  struct cache_s *c = (struct cache_s*)cache;
  if(!c) return;
  cache_clear(c);
  if(c->real) htable_delete(c->real), c->real = NULL;
  if(c->virtual) virtual_cache_delete(c->virtual), c->virtual = NULL;
  free(c);
}

/**
 * @fn void cache_set_logs_enable(cache_t cache, _Bool b)
 * @brief Activate or Desactivate the logs.
 * @param cache The cache pointer.
 * @param b The new status
 */
void cache_set_logs_enable(cache_t cache, _Bool b) {
  struct cache_s *c = (struct cache_s*)cache;
  if(!c) return;
  c->enable_logs = b;
  virtual_cache_set_logs_enable(c->virtual, b);
}

/**
 * @fn _Bool cache_is_logs_enabled(cache_t cache)
 * @brief Test if the logs are enabled
 * @param cache The cache pointer.
 * @return true/false.
 */
_Bool cache_is_logs_enabled(cache_t cache) {
  struct cache_s *c = (struct cache_s*)cache;
  if(!c) return false;
  return c->enable_logs;
}

/**
 * @fn void cache_clear(cache_t cache)
 * @brief Clear the cache.
 * @param cache The cache pointer.
 */
void cache_clear(cache_t cache) {
  struct cache_s *c = (struct cache_s*)cache;
  if(!c) return;
  if(c->real->kcount) {
    struct cache_item_data_s *item;
    int count, i;
    char** keys = NULL;
    count = htable_get_keys(c->real, &keys);
    for(i = 0; i < count; i++) {
      item = htable_lookup(c->real, keys[i]);
      if(item->data) free(item->data), item->data = NULL;
    }
    if(keys) free(keys);
    htable_clear(c->real);
  }
  virtual_cache_clear(c->virtual);
}

/**
 * @fn _Bool cache_invalidate(cache_t cache, cache_item_name_t name, cache_item_load_fct cache_item_load)
 * @brief Invalide a cache item.
 * @param cache The cache pointer.
 * @param name The item name.
 * @param cache_item_load The load callback.
 * @return true/false
 */
_Bool cache_invalidate(cache_t cache, cache_item_name_t name, cache_item_load_fct cache_item_load) {
  struct cache_s *c = (struct cache_s*)cache;
  if(!c) return false;
  cache_remove(c, name);
  struct cache_item_data_s body;
  return cache_get_item(c, name, cache_item_load, &body);
}

/**
 * @fn _Bool cache_contains_key(cache_t cache, cache_item_name_t name)
 * @brief Test if the cache contains a specific key.
 * @param cache The cache pointer.
 * @param name The key.
 * @return true/false.
 */
_Bool cache_contains_key(cache_t cache, cache_item_name_t name) {
  struct cache_s *c = (struct cache_s*)cache;
  if(!c) return false;
  return htable_has_key(c->real, name);
}

/**
 * @fn void cache_remove(cache_t cache, cache_item_name_t name)
 * @brief Remove an item from the cache.
 * @param cache The cache pointer.
 * @param name The item name.
 */
void cache_remove(cache_t cache, cache_item_name_t name) {
  struct cache_s *c = (struct cache_s*)cache;
  if(!c) return;
  virtual_cache_remove(c->virtual, name);
  struct cache_item_data_s *item = htable_lookup(c->real, name);
  if(item) {
    if(item->data) free(item->data), item->data = NULL;
    htable_remove(c->real, name);
  }
}

/**
 * @fn static _Bool cache_find_in_cache_size(llist_t li, cache_item_name_t name)
 * @brief Find a specific item into the cache size list.
 * @param li The list.
 * @param name The item name.
 * @return true/false.
 */
static _Bool cache_find_in_cache_size(llist_t li, cache_item_name_t name) {
  llist_t root = li;
  char* n;
  while(root) {
    n = llist_value(root);
    root = root->next;
    if(!strcmp(n, name)) return true;
  }
  return false;
}

/**
 * @fn void cache_release_item(struct cache_item_data_s *body)
 * @brief Release item after a call to get_item
 * @param body The item.
 */
void cache_release_item(struct cache_item_data_s *body) {
  if(body) {
    body->length = 0;
    if(body->data) free(body->data), body->data = NULL;
  }
}

/**
 * @fn _Bool cache_get_item(cache_t cache, cache_item_name_t name, cache_item_load_fct cache_item_load, struct cache_item_data_s *body)
 * @brief Get item from the cache.
 * @param cache The cache pointer.
 * @param name The item name.
 * @param cache_item_load The load function
 * @param body The result item.
 * @return false on error, true else.
 */
_Bool cache_get_item(cache_t cache, cache_item_name_t name, cache_item_load_fct cache_item_load, struct cache_item_data_s *body) {
  struct cache_s *c = (struct cache_s*)cache;
  if(!c) return false;
  struct cache_item_data_s item, *titem;
  virtual_cache_update(c->virtual, name, cache_item_load, &item);
  if(!virtual_cache_can_cache(c->virtual, name)) {
    memcpy(body, &item, sizeof(struct cache_item_data_s));
    return true;
  }
  llist_t li = virtual_cache_get_cache_max_size(c->virtual, c->max_cache_size);
  int count, i;
  char **keys = NULL;
  count = htable_get_keys(c->real, &keys);
  for(i = 0; i < count; i++) {
    titem = htable_lookup(c->real, keys[i]);
    if(cache_find_in_cache_size(li, keys[i])) {
      c->cache_size -= titem->length;
      htable_remove(c->real, keys[i]);
    }
  }
  if(keys) free(keys), keys = NULL;
  llist_t root = li;
  char* tname;
  while(root) {
    tname = llist_value(root);
    root = root->next;
    if (strcmp(tname, name)) {
      if (cache_contains_key(c, tname)) {
	memcpy(body, htable_lookup(c->real, tname), sizeof(struct cache_item_data_s));
	llist_clear(&li);
	return true;
      } else {
	if (item.data != NULL) {
	  c->cache_size += item.length;
	  htable_add(c->real, name, &item, sizeof(struct cache_item_data_s));
	  memcpy(body, &item, sizeof(struct cache_item_data_s));
	  llist_clear(&li);
	  return true;
	} else {
	  cache_item_load(name, &item);
	  if (item.data != NULL) {
	    c->cache_size += item.length;
	    htable_add(c->real, name, &item, sizeof(struct cache_item_data_s));
	    memcpy(body, &item, sizeof(struct cache_item_data_s));
	    llist_clear(&li);
	    return true;
	  } else {
	    memcpy(body, &item, sizeof(struct cache_item_data_s));
	    llist_clear(&li);
	    return true;
	  }
	}
      }
    }
  }
  llist_clear(&li);
  if (item.data == NULL) {
    cache_item_load(name, &item);
    memcpy(body, &item, sizeof(struct cache_item_data_s));
    return true;
  } else
    memcpy(body, &item, sizeof(struct cache_item_data_s));
  return false;
}


/**
 * @fn int cache_count_items(cache_t cache) 
 * @brief Count the number of items
 * @param cache The cache pointer.
 * @return The number.
 */
int cache_count_items(cache_t cache) {
  struct cache_s *c = (struct cache_s*)cache;
  if(!c) return 0;
  return virtual_cache_count_item(c->virtual);
}


/**
 * @fn void cache_set_max_cache_size(cache_t cache, long int max_cache_size)
 * @brief Change the max size of the cache
 * @param cache The cache pointer.
 * @param max_cache_size The new size.
 */
void cache_set_max_cache_size(cache_t cache, long int max_cache_size) {
  struct cache_s *c = (struct cache_s*)cache;
  if(!c) return;
  c->max_cache_size = max_cache_size;
}


/**
 * @fn long int cache_get_max_cache_size(cache_t cache)
 * @brief Get the max size of the cache.
 * @param cache The cache pointer.
 * @return The size
 */
long int cache_get_max_cache_size(cache_t cache) {
  struct cache_s *c = (struct cache_s*)cache;
  if(!c) return 0L;
  return c->max_cache_size;
}


/**
 * @fn long int cache_get_cache_size(cache_t cache)
 * @brief Get the current cache size.
 * @param cache The cache pointer.
 * @return The size.
 */
long int cache_get_cache_size(cache_t cache) {
  struct cache_s *c = (struct cache_s*)cache;
  if(!c) return 0L;
  return virtual_cache_get_size(c->virtual);
}

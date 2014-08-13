/**
*******************************************************************************
* @file htable.c
* @author Keidan
* @date 01/01/2013
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
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <tk/utils/htable.h>


#define DEFAULT_KNUM     65536
#define DEFAULT_KRATIO       4
#define DEFAULT_MKLEN        0



/* internal function */
/**
 * @fn void htable_rebuild_mklen(htable_st *table)
 * @brief Rebuild max key len.
 * @param table The table.
 */
void htable_rebuild_mklen(htable_st *table);

/**
 * @fn htable_el_st* htable_el_new(void)
 * @brief Creation of a new element.
 * @return The created element or NULL if no mememory.
 */
htable_el_st * htable_el_new(void) {
  return calloc(1, sizeof(htable_el_st));
}

/**
 * @fn void htable_el_delete(htable_el_st *element)
 * @brief Delete an element.
 * @param element The element to delete.
 */
void htable_el_delete(htable_el_st *element) {
  if(element) {
    if(element->key)
      free(element->key), element->key = NULL;
    if(element->alloc_value && element->value)
      free(element->value), element->value = NULL;
    free(element);
  }
}

/**
 * @fn htable_st* htable_new(void)
 * @brief Creation of a new htable.
 * @return The new htable or NULL if no memory.
 */
htable_st *htable_new(void) {
  return  htable_new_with_capacity(DEFAULT_KNUM);
}

/**
 * @fn htable_st* htable_new1(_Bool alloc_value)
 * @brief Creation of a new htable.
 * @param alloc_value Alloc the input value or juste copy.
 * @return The new htable or NULL if no memory.
 */
htable_st* htable_new1(_Bool alloc_value) {
  return  htable_new1_with_capacity(alloc_value, DEFAULT_KNUM);
}

/**
 * @fn htable_st* htable_new1_with_capacity(_Bool alloc_value, int capacity)
 * @brief Creation of a new htable.
 * @param alloc_value Alloc the input value or juste copy.
 * @param capacity Set the table capacity
 * @return The new htable or NULL if no memory.
 */
htable_st * htable_new1_with_capacity(_Bool alloc_value, int capacity) {
  htable_st *table = calloc(1, sizeof(htable_st));
  if (!table)
    return NULL;
  table->alloc_value = alloc_value;
  table->knum = capacity;
  table->kratio = DEFAULT_KRATIO;
  table->mklen = DEFAULT_MKLEN;
  table->house = (htable_el_st **) calloc(table->knum, sizeof(htable_el_st *));
  if (!table->house) {
    free(table);
    return NULL;
  }
  return table;
}

/**
 * @fn htable_st* htable_new_with_capacity(int capacity)
 * @brief Creation of a new htable.
 * @param capacity Set the table capacity
 * @return The new htable or NULL if no memory.
 */
htable_st * htable_new_with_capacity(int capacity) {
  return htable_new1_with_capacity(true, capacity);
}

/**
 * @fn void htable_delete(htable_st *table)
 * @brief Delete a htable.
 * @param table The table to delete.
 */
void htable_delete(htable_st *table) {
  if(!table) return;
  size_t i=0;
  for (;i<table->knum;i++) {
    while (table->house[i]) {
      htable_el_st * temp = table->house[i];
      table->house[i] = table->house[i]->next;
      htable_el_delete(temp);
    }
  }
  free(table->house);
  free(table);
}

/**
 * @fn void htable_clear(htable_st *table)
 * @brief Clear a htable.
 * @param table The table to clear.
 */
void htable_clear(htable_st *table) {
  size_t i=0;
  if(!table) return;
  for (;i<table->knum;i++) {
    while (table->house[i]) {
      htable_el_st * temp = table->house[i];
      table->house[i] = table->house[i]->next;
      htable_el_delete(temp);
    }
  }
  table->knum = DEFAULT_KNUM;
  table->kratio = DEFAULT_KRATIO;
  table->mklen = DEFAULT_MKLEN;
  table->kcount = 0;
  free(table->house);
  table->house = (htable_el_st **) calloc(table->knum, sizeof(htable_el_st *));
}

/**
 * @fn int htable_add(htable_st *table, char *key, void *value, size_t vlen)
 * @brief Add a new to element to the htable.
 * @param table The table.
 * @param key Element key to add.
 * @param value The value to add.
 * @param vlen The value length.
 * @return -1 if error else 0.
 */
int htable_add(htable_st *table, char *key, void *value, size_t vlen) {
  if ((table->kcount / table->knum) >= table->kratio)
    htable_resize(table, table->knum*2);
  size_t klen = strlen(key);
  size_t internal_key_length = klen+1;
  size_t internal_value_length = vlen+1;
    
  size_t hash = htable_hash(key, table->knum);
  htable_el_st *element = htable_el_new();
  if (!element)
    return -1; // No Memory
  element->alloc_value = table->alloc_value;
  element->key = malloc(internal_key_length);
  bzero(element->key, internal_key_length);
  if(element->alloc_value) {
    element->value = malloc(internal_value_length);
    bzero(element->value, internal_value_length);
  } else element->value = value;
  if (element->key && element->value) {
    memcpy(element->key, key, klen);
    if(element->alloc_value)
      memcpy(element->value, value, vlen);
  } else {
    if (element->key) {
      free(element->key);
    }
    if (element->value) {
      free(element->value);
    }
    free(element);
    return -1; //No Memory
  }
 
  element->vlen = vlen;
  element->klen = klen;
  element->next = NULL;
  if(klen > table->mklen) table->mklen = klen;
  // find the key position for chaining
  if (!table->house[hash]) {
    table->house[hash] = element;
    table->kcount++;
  } else {
    htable_el_st * temp = table->house[hash];
    while(temp->next) {
      while(temp->next && temp->next->klen!=klen) temp = temp->next;
      if(temp->next) {
        if (!memcmp(temp->next->key, key, klen)) {
          htable_el_st *to_delete = temp->next;
          temp->next = element;
          element->next = to_delete->next;
          htable_el_delete(to_delete);
          // since we are replacing values no need to change kcount
          return 0;
	} else
          temp = temp->next;
      }
    }
    temp->next = element;
    table->kcount++;
  }
  
  return 0;
}

/**
 * @fn int htable_remove(htable_st *table, char *key)
 * @brief Remove an element with the incoming key.
 * @param table The table.
 * @param key The element key.
 * @return -1 if error else 0.
 */
int htable_remove(htable_st *table, char *key) {
  if ((table->knum/ table->kcount) >= table->kratio)
    htable_resize(table, table->knum/2);

  size_t klen = strlen(key);
  size_t hash = htable_hash(key, table->knum);
  if (!table->house[hash])
    return -1; // key not found
  htable_el_st *temp = table->house[hash];
  htable_el_st *prev = temp;
  while(temp) {
    while(temp && temp->klen!=klen) {
      prev = temp;
      temp = temp->next;
    }
    if(temp) {
      if (!memcmp(temp->key, key, klen)) {
	if (prev == table->house[hash])
          table->house[hash] = temp->next;
        else
	  prev->next = temp->next;
        htable_el_delete(temp);
        table->kcount--;
	/* rebuild the max key len */
	htable_rebuild_mklen(table);
        return 0;
      }
      prev=temp;
      temp=temp->next;
    }
  }
  return -1; // key not found
}

/**
 * @fn void* htable_lookup(htable_st *table, char *key)
 * @brief Looking up a key-value pair
 * @param table The table.
 * @param key The key.
 * @return The value or NULL if not found.
 */
void* htable_lookup(htable_st *table, char *key) {
  size_t klen = strlen(key);
  size_t hash = htable_hash(key, table->knum);
  if (!table->house[hash])
    return NULL; // key not found
  htable_el_st *temp = table->house[hash];
  while(temp) {
    while(temp && temp->klen!=klen) 
      temp = temp->next;
    if(temp) {
      if (!memcmp(temp->key, key, klen))
	return temp->value;
      else
	temp = temp->next;
    }
  }
  return NULL; // key not found   
}

/**
 * @fn _Bool htable_has_key(htable_st *table, char *key)
 * @brief Check if the table contains this key.
 * @param table The table.
 * @param key The key.
 * @return true/false
 */
_Bool htable_has_key(htable_st *table, char *key) {
  size_t klen = strlen(key);
  size_t hash = htable_hash(key, table->knum);
  if (!table->house[hash])
    return false; // key not found
  htable_el_st *temp = table->house[hash];
  while(temp) {
    while(temp && temp->klen!=klen)
      temp = temp->next;
    if(temp) {
      if (!memcmp(temp->key, key, klen))
        return true; // key found
      temp=temp->next;
    }
  }
  return false; // key not found   
}

/**
 * @fn size_t htable_get_keys(htable_st *table, char ***keys)
 * @brief Get all keys.
 * @param table The table.
 * @param keys The destination list (free required).
 * @return The keys count.
 */
size_t htable_get_keys(htable_st *table, char ***keys) {
  size_t i = 0;
  size_t count = 0;
  (*keys) = (char**)calloc(table->kcount, sizeof(char*));
  if (!*keys)
    return 0;
  for(i=0;i<table->knum;i++) {
    if (table->house[i]) {
      (*keys)[count++] = table->house[i]->key;
      htable_el_st *temp = table->house[i];
      while(temp->next) {
	if(*keys && (*keys)[count] && temp->next->key)
	  (*keys)[count++] = temp->next->key;
	temp = temp->next;
      }
    }
  }
  return count;
}

/**
 * @fn llist_t htable_get_keys_list(htable_st *table)
 * @brief Get all keys in a llist_t.
 * @param table The table.
 * @return The key list.
 */
llist_t htable_get_keys_list(htable_st *table) {
  llist_t list = NULL;
  size_t i = 0;
  for(i=0;i<table->knum;i++) {
    if (table->house[i]) {
      list = llist_pushback(list, table->house[i]->key);
      htable_el_st *temp = table->house[i];
      while(temp->next) {
	if(temp->next->key)
	  list = llist_pushback(list, temp->next->key);
	temp = temp->next;
      }
    }
  }
  return list;
}

/**
 * @fn size_t htable_get_elements(htable_st *table, htable_el_st ***elements)
 * @brief Get all elements.
 * @param table The table.
 * @param elements The destination list.
 * @return The elements count, the incomming elements pointer is allocated by this function (*elements).
 */
size_t htable_get_elements(htable_st *table, htable_el_st ***elements) {
  size_t i = 0;
  size_t count = 0;
  (*elements) = (htable_el_st **) calloc(table->kcount, sizeof(htable_el_st *));
  if (!*elements)
    return 0;
  for(i=0;i<table->knum;i++) {
    if (table->house[i]) {
      (*elements)[count++] = table->house[i];
      htable_el_st *temp = table->house[i];
      while(temp->next) {
        (*elements)[count++] = temp->next;
        temp = temp->next;
      }
    }
  }
  return count;
}

/**
 * @fn llist_t htable_get_elements_list(htable_st *table)
 * @brief Get all elements.
 * @param table The table.
 * @return The elements list of htable_el_st*.
 */
llist_t htable_get_elements_list(htable_st *table) {
  llist_t list = NULL;
  size_t i = 0;
  for(i=0;i<table->knum;i++) {
    if (table->house[i]) {
      list = llist_pushback(list, table->house[i]);
      htable_el_st *temp = table->house[i];
      while(temp->next) {
	list = llist_pushback(list, temp->next);
        temp = temp->next;
      }
    }
  }
  return list;
}

/**
 * @fn uint32_t htable_hash(char *key, uint32_t max_key)
 * @brief Hash calculation.
 * @param key The key to hash.
 * @param max_key Thge max key (modulos use).
 * @return The calculated hash.
 */
uint32_t htable_hash(char *key, uint32_t max_key) {
  /* djb2 hash */
  size_t klen = strlen(key);
  uint32_t hash = 5381;
  uint32_t i;
  for (i = 0; i < klen; i++)
    hash += ((hash << 5) + (unsigned int)key[i]);
  return hash % max_key;
}

/**
 * @fn htable_resize_et htable_resize(htable_st *table, size_t size)
 * @brief Resize a htable.
 * @param table Htable to resized.
 * @param size The new size.
 * @return see enum htable_resize_et
 */
htable_resize_et htable_resize(htable_st *table, size_t size) {
  htable_el_st **elements;
  size_t count;
  // FIXME traversing the elements twice, change it some time soon
  count = htable_get_elements(table, &elements);
  if (!count)
    return HT_RESIZE_NO_ELEMENTS;
  // keep the current store house in case we dont get more memory
  htable_el_st **temp = table->house;
  table->house = calloc(size, sizeof(htable_el_st *));
  if (!table->house)
    table->house = temp;
  return HT_RESIZE_NO_MEMORY;
  table->knum = size;
  // the new table starts from scratch
  table->kcount = 0;
  while(count>0) {
    htable_el_st *elem = elements[--count];
    htable_add(table, elem->key, elem->value, elem->vlen);
  }
  // free old house house
  free(temp);
  return HT_RESIZE_SUCCESS;
}

/**
 * @fn void htable_rebuild_mklen(htable_st *table)
 * @brief Rebuild max key len.
 * @param table The table.
 */
void htable_rebuild_mklen(htable_st *table) {
  int i, mklen = 0;
  for(i = 0; i < table->knum; i++) {
    if (table->house[i]) {
      htable_el_st *temp = table->house[i];
      while(temp) {
	if(mklen < temp->klen)
	  mklen = temp->klen;
	temp = temp->next;
      }
    }
  }
  if(table->mklen != mklen)
    table->mklen = mklen;;
}

/**
 * @fn int htable_iterate(htable_st *table, htable_iterate_ft fct, void *user)
 * @brief Parse htable entries.
 * @param table The table.
 * @param fct The iterate function.
 * @param user An user pointer.
 * @return -1 if error else 0.
 */
int htable_iterate(htable_st *table, htable_iterate_ft fct, void *user) {
  int i;
  for(i=0;i<table->knum;i++) {
    if (table->house[i]) {
      htable_el_st *temp = table->house[i];
      htable_el_st *prev = NULL;
      while(temp) {
        int r = fct(user, temp->value, temp->key);
        if (r){
          htable_el_st *next = temp->next;
          htable_el_delete(temp);
          if(prev == NULL) table->house[i] = next;
          else prev->next = next;
          temp = next;
	  table->kcount--;
        } else {
	  prev = temp;
	  temp = temp->next;
        }
      }
    }
  }
  return 0;
}


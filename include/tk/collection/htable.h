/**
 *******************************************************************************
 * @file htable.h
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
#ifndef __HTABLE_H__
  #define __HTABLE_H__

  #include <sys/types.h>
  #include <stdint.h>
  #include <stdbool.h>

  typedef struct htable_el_s htable_el_st;

  /**
   * @struct htable_el_s
   * @brief Htable element.
   */
  struct htable_el_s {
      size_t klen;                   /**< The key length. */
      size_t vlen;                   /**< The value length. */
      char   *key;                   /**< The key. */
      void   *value;                 /**< The value. */
      htable_el_st * next;   /**< The next element. */
  };

  /**
   * @struct htable_st
   * @brief Htable object.
   */
  typedef struct htable_s {
      htable_el_st  **house; /**< store house datas. */
      size_t kcount;                 /**< key count. */
      uint32_t knum;                 /**< Key num. */
      size_t kratio;                 /**< Key ratio. */
      size_t mklen;                  /**< Max key len. */
  } htable_st;
  typedef htable_st *htable_t;

  /**
   * @enum htable_resize_et
   * @brief Various result error for resizing.
   */
  typedef enum { 
    HT_RESIZE_NO_MEMORY,             /**< Error no memory for resize. */
    HT_RESIZE_NO_ELEMENTS,           /**< Empty htable. */
    HT_RESIZE_SUCCESS                /**< Resize success. */
  } htable_resize_et;

  /**
   * @typedef int (*htable_iterate_ft)(void *user, void *value, char *key)
   * @brief iterate callback function.
   * @param user[in,out] User pointer.
   * @param value[in,out] The current value.
   * @param key[in,out] The current key.
   * @param klen The key length.
   * @return 1 if you want a deletion after the current call
   */
  typedef int (*htable_iterate_ft)(void *user, void *value, char *key);


  #ifdef __cplusplus
  extern "C" {
  #endif

    /**
     * @fn htable_el_st* htable_el_new(void)
     * @brief Creation of a new element.
     * @return The created element or NULL if no mememory.
     */
    htable_el_st* htable_el_new(void);

    /**
     * @fn void htable_el_delete(htable_el_st *element)
     * @brief Delete an element.
     * @param element The element to delete.
     */
    void htable_el_delete(htable_el_st *element);

    /**
     * @fn uint32_t htable_hash(char *key, uint32_t max_key)
     * @brief Hash calculation.
     * @param key The key to hash.
     * @param klen The length of the key.
     * @param max_key Thge max key (modulos use).
     * @return The calculated hash.
     */
    uint32_t htable_hash(char *key, uint32_t max_key);

    /**
     * @fn htable_st* htable_new()
     * @brief Creation of a new htable.
     * @return The new htable or NULL if no memory.
     */
    htable_st* htable_new();

    /**
     * @fn htable_st* htable_new_with_capacity(int capacity)
     * @brief Creation of a new htable.
     * @param capacity Set the table capacity
     * @return The new htable or NULL if no memory.
     */
    htable_st * htable_new_with_capacity(int capacity);

    /**
     * @fn void htable_delete(htable_st *table)
     * @brief Delete a htable.
     * @param table The table to delete.
     */
    void htable_delete(htable_st *table);

    /**
     * @fn void htable_clear(htable_st *table)
     * @brief Clear a htable.
     * @param table The table to clear.
     */
    void htable_clear(htable_st *table);

    /**
     * @fn int htable_add(htable_st *table, char *key, void *value, size_t vlen)
     * @brief Add a new to element to the htable.
     * @param table The table.
     * @param key Element key to add.
     * @param value The value to add.
     * @param vlen The value length.
     * @return -1 if error else 0.
     */
    int htable_add(htable_st *table, char *key, void *value, size_t vlen);

    /**
     * @fn int htable_remove(htable_st *table, char *key)
     * @brief Remove an element with the incoming key.
     * @param table The table.
     * @param key The element key.
     * @return -1 if error else 0.
     */
    int htable_remove(htable_st *table, char *key);

    /**
     * @fn void* htable_lookup(htable_st *table, char *key)
     * @brief Looking up a key-value pair
     * @param table The table.
     * @param key The key.
     * @param klen The key length.
     * @return The value or NULL if not found.
     */
    void* htable_lookup(htable_st *table, char *key);

    /**
     * @fn int htable_has_key(htable_st *table, char *key)
     * @brief Check if the table contains this key.
     * @param table The table.
     * @param key The key.
     * @param klen The key len.
     * @return 1 if error else 0.
     */
    int htable_has_key(htable_st *table, char *key);

    /**
     * @fn size_t htable_get_keys(htable_st *table, char ***keys)
     * @brief Get all keys.
     * @param table The table.
     * @param keys The destination list (free required).
     * @return The keys count.
     */
    size_t htable_get_keys(htable_st *table, char ***keys);

    /**
     * @fn size_t htable_get_elements(htable_st *table, htable_el_st ***elements)
     * @brief Get all elements.
     * @param table The table.
     * @param elements The destination list.
     * @return The elements count, the incomming elements pointer is allocated by this function (*elements).
     */
    size_t htable_get_elements(htable_st *table, htable_el_st ***elements);

    /**
     * @fn htable_resize_et htable_resize(htable_st *table, size_t size)
     * @brief Resize a htable.
     * @param table Htable to resized.
     * @param size The new size.
     * @return see enum htable_resize_et
     */
    htable_resize_et htable_resize(htable_st *table, size_t size);

    /**
     * @fn int htable_iterate(htable_st *table, htable_iterate_ft fct, void *user)
     * @brief Parse htable entries.
     * @param table The table.
     * @param fct The iterate function.
     * @param user An user pointer.
     * @return -1 if error else 0.
     */
    int htable_iterate(htable_st *table, htable_iterate_ft fct, void *user);
	
  #ifdef __cplusplus
  } /* extern "C" */
  #endif
#endif /* __HTABLE_H__ */

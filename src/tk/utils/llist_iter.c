/**
 *******************************************************************************
 * @file llist_iter.c
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
#include <errno.h>
#include <string.h>
#include <tk/utils/llist_iter.h>
#include <tk/sys/log.h>

typedef struct {
    llist_t __root__;
    llist_t __node__;
} llist_it_ptr_t;


/**
 * @fn llist_iter_t llist_iter_alloc(llist_t list)
 * @brief Allocation of an iterator on the llist.
 * @param list The list.
 * @return Iterator.
 */
llist_iter_t llist_iter_alloc(llist_t list) {
  llist_it_ptr_t *it = NULL;
  if((it = malloc(sizeof(llist_it_ptr_t))) == NULL) {
    logger(LOG_ERR, "Iterator alloc failed!");
    return it;
  }
  it->__root__ = list;
  it->__node__ = list;
  return it;
}

/**
 * @fn void llist_iter_init(llist_iter_t it, llist_t list)
 * @brief Init without allocation (do not call llist_iter_free)
 * @param it Iterator.
 * @param list The list.
 */
void llist_iter_init(llist_iter_t it, llist_t list) {
  llist_it_ptr_t* ptr = ( llist_it_ptr_t*)it;
  ptr->__root__ = list;
  ptr->__node__ = list;
}
/**
 * @fn void llist_iter_free(llist_iter_t it)
 * @brief Release the resources.
 * @param it The iterator to be release.
 */
void llist_iter_free(llist_iter_t it) {
  if(!it) return;
  free(it), it = NULL;
}

/**
 * @fn llist_t llist_iter_next(llist_iter_t it)
 * @brief Get the current iterator and moves to the next.
 * @param it Iterator.
 * @return The current element or NULL.
 */
llist_t llist_iter_next(llist_iter_t it) {
  llist_t tmp = NULL;
  if(!it) return tmp;
  llist_it_ptr_t* ptr = ( llist_it_ptr_t*)it;
  if(!ptr->__node__) return tmp;
  tmp = ptr->__node__;
  ptr->__node__ = ptr->__node__->next;
  return tmp;
}

/**
 * @fn _Bool llist_iter_rewind(llist_iter_t it)
 * @brief Rewind the iterator to the start position into the list
 * @param it Iterator.
 * @return 0 0 if it is not possible to return to the top.
 */
_Bool llist_iter_rewind(llist_iter_t it) {
  if(!it) return 0;
  llist_it_ptr_t* ptr = ( llist_it_ptr_t*)it;
  if(!ptr->__root__) return 0;
  ptr->__node__ = ptr->__root__;
  return 1;
}

/**
 * @fn _Bool llist_iter_has_more(llist_iter_t it)
 * @brief Test if the iterator is at the last position of the list.
 * @param it Iterator.
 * @return 0 if the iteratir is at the last position else 1.
 */
_Bool llist_iter_has_more(llist_iter_t it) {
  if(!it) return 0;
  llist_it_ptr_t* ptr = ( llist_it_ptr_t*)it;
  if(!ptr->__node__) return 0;
  return 1;
}

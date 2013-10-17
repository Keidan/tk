/**
 *******************************************************************************
 * @file llist.c
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tk/utils/llist.h>


llist_t llist_create(void* data, size_t vlen) {
  llist_t n;
  if(!(n=malloc(sizeof(ll_st)))) return NULL;
  if(vlen) {
    n->data = malloc(vlen);
    if(!n->data) {
      free(n);
      return NULL;
    }
    memcpy(n->data, data, vlen);
  } else
    n->data=data;
  n->next=NULL;
  n->head=NULL;
  n->alloc = vlen != 0;
  return n;
}

/**
 * @fn llist_t llist_pushback_and alloc(llist_t list, void* value)
 * @brief Add an item at the last and alloc a pointer for the datas
 * @param list The list
 * @param value The value.
 * @return The list with the new item.
 */
llist_t llist_pushback_and_alloc(llist_t list, void* value, size_t vlen) {
  llist_t newnode;
  newnode = llist_create(value, vlen);
  if(!list) return (newnode->head = newnode);
  newnode->head = list->head;
  newnode->next = list->next;
  list->next = newnode;
  return list;
}
/**
 * @fn llist_t llist_pushback(llist_t list, void* value)
 * @brief Add an item at the last
 * @param list The list
 * @param value The value.
 * @return The list with the new item.
 */
llist_t llist_pushback(llist_t list, void* value) {
  return llist_pushback_and_alloc(list, value, 0);
}

/**
 * @fn llist_t llist_pushfirst(llist_t list, void* value)
 * @brief Add an item at the top.
 * @param list The list
 * @param value The value.
 * @return The list with the new item.
 */
llist_t llist_pushfirst(llist_t list, void* value) {
  llist_t newnode;
  newnode = llist_create(value, 0);
  newnode->next = list;
  return newnode;
}

/**
 * @fn llist_t llist_find(llist_t list, llist_comparator_t comparator, void* value)
 * @brief Search an item in the list.
 * @param list The list
 * @param comparator The comparator
 * @param value The alue to be search
 * @return The item else NULL if not found.
 */
llist_t llist_find(llist_t list, llist_comparator_t comparator, void* data) {
  if(!list || !comparator) return NULL;
  while(list) {
    if(comparator(list->data, data)>0) return list;
    list = list->next;
  }
  return NULL;
}


/**
 * @fn void* llist_value(llist_t list)
 * @brief Get the alue of an element.
 * @param list The list to be retreived.
 * @return The element value else NULL.
 */ 
void* llist_value(llist_t list) {
  return list ? list->data : 0;
}
llist_t llist_head(llist_t list) {
  return list ? list->head : 0;
}

/**
 * @fn uint32_t llist_count(llist_t list)
 * @brief Count the number of elements into the list
 * @param list The list.
 * @return The number of elements.
 */ 
uint32_t llist_count(llist_t list) {
  return list ? (llist_count(list->next) + 1) : 0;
}

 
/**
 * @fn uint32_t llist_count_data(llist_t list)
 * @brief Count the number of elements data (only if non NULL) into the list
 * @param list The list.
 * @return The number of elements data.
 */
uint32_t llist_count_data(llist_t list) {
  uint32_t count = 0;
  llist_t p;
  p = list;
  while(p) {
    if(p->data) count++;
    p = p->next;
  }
  return count;
}

/**
 * @fn _Bool llist_is_empty(llist_t list)
 * @brief Test if the list is empty.
 * @param list The list
 * @return 1 if the list is empty else 0.
 */
_Bool llist_is_empty(llist_t list) {
  return llist_count(list) == 0;
}
/**
 * @fn void llist_print(FILE* std, llist_t list, llist_printable_t printable)
 * @brief Print the list
 * @param std Output stream.
 * @param list The list.
 * @param data User callback for the print print (NULL print the default informations).
 */
void llist_print(FILE* std, llist_t list, llist_printable_t printable) {
  while(list) {
    if(printable == NULL)
      fprintf(std, "llist %#x={ data=%#x, next=%#x }\n", 
	      (uint32_t)list, (uint32_t)list->data, (uint32_t)list->next);
    else printable(list->data);
    list = list->next;
  }
}


/**
 * @fn void llist_foreach(llist_t list, llist_foreach_t fct, void* user_param)
 * @brief Simple foreach
 * @param list The list.
 * @param fct callback.
 * @param user_param User param.
 */
void llist_foreach(llist_t list, llist_foreach_t fct, void* user_param) {
  llist_t tmp;
  _Bool error = 0;
  while(list) {
    tmp = list;
    list = list->next;
    fct(tmp, user_param, &error);
    if(error) return;
  } 
}
/**
 * @fn void llist_remove_by_value(llist_t *list, void* val)
 * @brief Remove an element by its value
 * @param list The list.
 * @param val The value to be remove.
 */
void llist_remove_by_value(llist_t *list, void* val)  {
  if(!list || !*list) return;
  llist_t curr, prev;
  /* For 1st node, indicate there is no previous. */
  prev = NULL;
  /*
   * Visit each node, maintaining a pointer to
   * the previous node we just visited.
   */
  for (curr = *list; 
       curr != NULL;
       prev = curr, curr = curr->next) {
    if (curr->data == val) {  /* Found it. */
      if (prev == NULL) {
        /* Fix beginning pointer. */
        *list = curr->next;
      } else {
        /*
         * Fix previous node's next to
         * skip over the removed node.
         */
        prev->next = curr->next;
      }
      if(curr->alloc && curr->data) free(curr->data), curr->data = NULL;
      /* Deallocate the node. */
      free(curr);

      /* Done searching. */
      return;
    }
  }
}


/**
 * @fn _Bool llist_is_empty_data(llist_t list)
 * @brief Test if the list only contains a NULL datas.
 * @param list The list
 * @return 1 if the list only contains a NULL datas.
 */
_Bool llist_is_empty_data(llist_t list) {
  return llist_count_data(list) == 0;
}

/**
 * @fn void llist_clear(llist_t *list)
 * @brief Clear the list
 * @param list The list.
 */
void llist_clear(llist_t *list) {
  if(!list || !*list) return;
  llist_t node;
  while(*list) {
    node = *list;
    *list = (*list)->next;
    if(node) {
      if(node->alloc && node->data) free(node->data), node->data = NULL;
      free(node);
    }
  } 
  *list = NULL;
}


/**
 * @fn llist_t llist_sort(llist_t list, llist_comparator_t comparator)
 * @brief Simple sort
 * @param list The list.
 * @param comparator The comparator
 * @return The sorted list.
 */
llist_t llist_sort(llist_t list, llist_comparator_t comparator) {
  if(!list || !comparator) return NULL;
  llist_t i, j, min;
  void* temp;
  for( i = list; i->next; i = i->next)  {
    min = i;
    for (j = i->next; j; j = j->next)  {
      if (comparator(j->data, min->data))
	min = j;
    }
    if (min != i) {
      temp = min->data;
      min->data = i->data;
      i->data = temp;
    }
  }
  return list;
}


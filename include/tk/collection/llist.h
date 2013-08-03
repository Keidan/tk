#ifndef __LLIST_H__
  #define __LLIST_H__

  #include <stdio.h>
  #include <stdint.h>


  typedef struct ll_s ll_st;

  struct ll_s {
      void*        data;
      ll_st        *next;
      ll_st        *head;
  };

  typedef ll_st* llist_t;

  typedef void (* llist_printable_t)(void* data);
  typedef _Bool (*llist_comparator_t)(void* v1, void* v2);
  typedef void (*llist_foreach_t)(llist_t node, void* user_param, _Bool* error);

  /**
   * @fn void llist_print(FILE* std, llist_t list, llist_printable_t printable)
   * @brief Print the list
   * @param std Output stream.
   * @param list The list.
   * @param data User callback for the print print (NULL print the default informations).
   */
  void llist_print(FILE* std, llist_t list, llist_printable_t printable);

  /**
   * @fn void llist_foreach(llist_t list, llist_foreach_t fct, void* user_param)
   * @brief Simple foreach
   * @param list The list.
   * @param fct callback.
   * @param user_param User param.
   */
  void llist_foreach(llist_t list, llist_foreach_t fct, void* user_param);

  /**
   * @fn llist_t llist_pushback(llist_t list, void* value)
   * @brief Add an item at the last
   * @param list The list
   * @param value The value.
   * @return The list with the new item.
   */
  llist_t llist_pushback(llist_t list, void* value);

  /**
   * @fn llist_t llist_pushfirst(llist_t list, void* value)
   * @brief Add an item at the top.
   * @param list The list
   * @param value The value.
   * @return The list with the new item.
   */
  llist_t llist_pushfirst(llist_t list, void* value);

  /**
   * @fn llist_t llist_find(llist_t list, llist_comparator_t comparator, void* value)
   * @brief Search an item in the list.
   * @param list The list
   * @param comparator The comparator
   * @param value The alue to be search
   * @return The item else NULL if not found.
   */
  llist_t llist_find(llist_t list, llist_comparator_t comparator, void* value);

  /**
   * @fn void* llist_value(llist_t list)
   * @brief Get the alue of an element.
   * @param list The list to be retreived.
   * @return The element value else NULL.
   */ 
  void* llist_value(llist_t list);
  llist_t llist_head(llist_t list);

  /**
   * @fn uint32_t llist_count(llist_t list)
   * @brief Count the number of elements into the list
   * @param list The list.
   * @return The number of elements.
   */ 
  uint32_t llist_count(llist_t list);
 
  /**
   * @fn uint32_t llist_count_data(llist_t list)
   * @brief Count the number of elements data (only if non NULL) into the list
   * @param list The list.
   * @return The number of elements data.
   */
  uint32_t llist_count_data(llist_t list);

  /**
   * @fn _Bool llist_is_empty(llist_t list)
   * @brief Test if the list is empty.
   * @param list The list
   * @return 1 if the list is empty else 0.
   */
  _Bool llist_is_empty(llist_t list);

  /**
   * @fn _Bool llist_is_empty_data(llist_t list)
   * @brief Test if the list only contains a NULL datas.
   * @param list The list
   * @return 1 if the list only contains a NULL datas.
   */
  _Bool llist_is_empty_data(llist_t list);

  /**
   * @fn void llist_remove_by_value(llist_t *list, void* val)
   * @brief Remove an element by its value
   * @param list The list.
   * @param val The value to be remove.
   */
  void llist_remove_by_value(llist_t *list, void* val);

  /**
   * @fn void llist_clear(llist_t *list)
   * @brief Clear the list
   * @param list The list.
   */
  void llist_clear(llist_t *list);

  /**
   * @fn void llist_clear_all_empty_data(llist_t *list, void* empty_value)
   * @brief Clear all elements with the NULL datas.
   * @param list The list
   */
  void llist_clear_all_empty_data(llist_t *list, void* empty_value);

  /**
   * @fn llist_t llist_sort(llist_t list, llist_comparator_t comparator)
   * @brief Simple sort
   * @param list The list.
   * @param comparator The comparator
   * @return The sorted list.
   */
  llist_t llist_sort(llist_t list, llist_comparator_t comparator);

#endif /* __LLIST_H__ */

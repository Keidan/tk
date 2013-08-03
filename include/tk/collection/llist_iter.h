#ifndef __LLIST_ITER_H__
  
  #define __LLIST_ITER_H__

  #include <tk/collection/llist.h>


  typedef void* llist_iter_t;


  /**
   * @fn llist_iter_t llist_iter_alloc(llist_t list)
   * @brief Allocation of an iterator on the llist.
   * @param list The list.
   * @return Iterator.
   */
  llist_iter_t llist_iter_alloc(llist_t list);


  /**
   * @fn void llist_iter_free(llist_iter_t it)
   * @brief Release the resources.
   * @param it The iterator to be release.
   */
  void llist_iter_free(llist_iter_t it);


  /**
   * @fn llist_t llist_iter_next(llist_iter_t it)
   * @brief Get the current iterator and moves to the next.
   * @param it Iterator.
   * @return The current element or NULL.
   */
  llist_t llist_iter_next(llist_iter_t it);

  /**
   * @fn _Bool llist_iter_has_more(llist_iter_t it)
   * @brief Test if the iterator is at the last position of the list.
   * @param it Iterator.
   * @return 0 if the iteratir is at the last position else 1.
   */
  _Bool llist_iter_has_more(llist_iter_t it);

  /**
   * @fn _Bool llist_iter_rewind(llist_iter_t it)
   * @brief Rewind the iterator to the start position into the list
   * @param it Iterator.
   * @return 0 false if it is not possible to return to the top.
   */
  _Bool llist_iter_rewind(llist_iter_t it);

#endif /* __LLIST_ITER_H__ */

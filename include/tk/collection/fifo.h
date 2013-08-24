/**
 *******************************************************************************
 * @file fifo.h
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
#ifndef __FIFO_H__

  #define __FIFO_H__

  struct fifo_element_s{
    void* value;
    struct fifo_element_s *previous;
    struct fifo_element_s *next;
  };

  struct fifo_list_s{
    struct fifo_element_s *first;
    struct fifo_element_s *last;
  };
  typedef struct fifo_list_s *fifo_t;

  typedef void (*fifo_foreach_fct_t)(void* value, void* user);

  /**
   * @fn fifo_t fifo_new()
   * @brief Alloc new fifo.
   * @return The fifo else NULL on error.
   */
  fifo_t fifo_new();

  /**
   * @fn void fifo_delete(fifo_t fifo)
   * @brief Delete a fifo.
   * @param fifo The fifo.
   */
  void fifo_delete(fifo_t fifo);

  /**
   * @fn void fifo_clear(fifo_t fifo)
   * @brief Clear a fifo.
   * @param fifo The fifo.
   */
  void fifo_clear(fifo_t fifo);

  /**
   * @fn _Bool fifo_empty(fifo_t fifo)
   * @brief Test if the fifo is empty.
   * @param fifo The fifo.
   * @return 1 if the fifo is empty else 0.
   */
  _Bool fifo_empty(fifo_t fifo);

  /**
   * @fn unsigned long fifo_size(fifo_t fifo)
   * @brief Get the fifo size.
   * @param fifo The fifo.
   * @return The size.
   */
  unsigned long fifo_size(fifo_t fifo);

  /**
   * @fn void fifo_push(fifo_t fifo, void* value)
   * @brief Push a new value in the fifo.
   * @param fifo The fifo.
   * @param value The value.
   */
  void fifo_push(fifo_t fifo, void* value);

  /**
   * @fn void* fifo_pop(fifo_t fifo)
   * @brief Pop a value from the fifo
   * @param fifo The fifo.
   * @return The value.
   */
  void* fifo_pop(fifo_t fifo);

  /**
   * @fn void* fifo_get(fifo_t fifo, long position)
   * @brief Get a value from the fifo
   * @param fifo The fifo.
   * @param position The value position
   * @return The value.
   */
  void* fifo_get(fifo_t fifo, long position);

  /**
   * @fn void fifo_foreach(fifo_t fifo, fifo_foreach_fct_t fct, void* user_data)
   * @brief Iterate a fifo.
   * @param fifo The fifo.
   * @param fct Foreach callback.
   * @param user_data A user data.
   */
  void fifo_foreach(fifo_t fifo, fifo_foreach_fct_t fct, void* user_data);


#endif /* __FIFO_H__ */

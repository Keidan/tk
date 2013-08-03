#include <tk/collection/fifo.h>
#include <tk/sys/log.h>
#include <stdlib.h>

/**
 * @fn fifo_t fifo_alloc()
 * @brief Alloc new fifo.
 * @return The fifo else NULL on error.
 */
fifo_t fifo_alloc() {
  fifo_t fifo = (fifo_t)malloc(sizeof(struct fifo_list_s));
  if(!fifo) {
    logger(LOG_ERR, "alloc failed!");
    return NULL;
  }
  fifo->first = NULL;
  fifo->last = NULL;
  return fifo;
}

/**
 * @fn void fifo_free(fifo_t fifo)
 * @brief Delete a fifo.
 * @param fifo The fifo.
 */
void fifo_free(fifo_t fifo) {
  if (fifo && fifo->first) {
    fifo_clear(fifo);
    free(fifo);
  }
}

/**
 * @fn void fifo_clear(fifo_t fifo)
 * @brief Clear a fifo.
 * @param fifo The fifo.
 */
void fifo_clear(fifo_t fifo) {
  struct fifo_element_s* p;
  if (fifo && fifo->first) {
    for (p = fifo->first->next; p != NULL; p = p->next) 
      free(p->previous);
    free(fifo->first);
    fifo->last = fifo->first = NULL;
  }
}

/**
 * @fn _Bool fifo_empty(fifo_t fifo)
 * @brief Test if the fifo is empty.
 * @param fifo The fifo.
 * @return 1 if the fifo is empty else 0.
 */
_Bool fifo_empty(fifo_t fifo) {
  return fifo_size(fifo) == 0;
}

/**
 * @fn void fifo_foreach(fifo_t fifo, fifo_foreach_fct_t fct, void* user_data)
 * @brief Iterate a fifo.
 * @param fifo The fifo.
 * @param fct Foreach callback.
 * @param user_data A user data.
 */
void fifo_foreach(fifo_t fifo, fifo_foreach_fct_t fct, void* user_data) {
  struct fifo_element_s* p;
  if(fifo && fifo->first)
    for(p = fifo->first; p != fifo->last; p = p->next) {
      fct(p->value, user_data);
    }
}

/**
 * @fn unsigned long fifo_size(fifo_t fifo)
 * @brief Get the fifo size.
 * @param fifo The fifo.
 * @return The size.
 */
unsigned long fifo_size(fifo_t fifo) {
  struct fifo_element_s* p;
  unsigned long i = 0;

  if(fifo && fifo->first)
    for(i++, p = fifo->first; p != fifo->last; p = p->next)
      i++;
  return i;
}

/**
 * @fn void fifo_push(fifo_t fifo, void* value)
 * @brief Push a new value in the fifo.
 * @param fifo The fifo.
 * @param value The value.
 */
void fifo_push(fifo_t fifo, void* value) {
  if(!fifo) return;
  struct fifo_element_s* p = (struct fifo_element_s*)malloc(sizeof(struct fifo_element_s));
  p->value = value;
  if(!fifo->first) {
    p->previous = NULL;
    p->next = NULL;
    fifo->first = p;
    fifo->last = p;
  } else {
    fifo->last->next = p;
    p->previous = fifo->last;
    fifo->last = p;
  }
  p->next = NULL;
}

/**
 * @fn void* fifo_pop(fifo_t fifo)
 * @brief Pop a value from the fifo
 * @param fifo The fifo.
 * @return The value.
 */
void* fifo_pop(fifo_t fifo) {
  void * value = NULL;
  if(!fifo || !fifo->first) return NULL;
  value = fifo->first->value;
  if (fifo->first->next) {
    fifo->first = fifo->first->next;
    free(fifo->first->previous);
    fifo->first->previous = NULL;
  } else {
    free(fifo->first);
    fifo->first = fifo->last = NULL;
  }
  return value;
}

/**
 * @fn void* fifo_get(fifo_t fifo, long position)
 * @brief Get a value from the fifo
 * @param fifo The fifo.
 * @param position The value position
 * @return The value.
 */
void* fifo_get(fifo_t fifo, long position) {
  if(!fifo) return NULL;
  struct fifo_element_s* p;
  for(p=fifo->first; position && p; p = p->next)
    position--;
  if(p)
    return p->value;
  else
    return NULL;
}

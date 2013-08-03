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

  fifo_t fifo_alloc();
  void fifo_free(fifo_t fifo);
  void fifo_clear(fifo_t fifo);
  _Bool fifo_empty(fifo_t fifo);
  unsigned long fifo_size(fifo_t fifo);
  void fifo_push(fifo_t fifo, void* value);
  void* fifo_pop(fifo_t fifo);
  void* fifo_get(fifo_t fifo, long position);
  void fifo_foreach(fifo_t fifo, fifo_foreach_fct_t fct, void* userData);


#endif /* __FIFO_H__ */

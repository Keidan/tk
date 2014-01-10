/**
*******************************************************************************
* @file systask.c
* @author Keidan
* @date 10/01/2014
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
#include <tk/sys/systask.h>
#include <tk/sys/log.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>


#define SYSTASK_REF_ID 0xC007B0
#define create_ptr(local, param) struct systask_s *local = (struct systask_s*)param

static void* systask_handler(void* v);

struct systask_s{
    int ref;
    struct {
	pthread_t tid;
	sem_t sem;
	pthread_mutex_t lock;
	pthread_mutex_t cond_mutex;
	pthread_cond_t  cond;
    } thread;
    systask_callback_fct callback;
    void* ptr;
    time_t delay;
    _Bool recursive;
    _Bool start;
    _Bool locked;
    _Bool ready;
    _Bool sleep;
};


/**
 * @fn systask_t systask_new(time_t delay, _Bool recursive, systask_callback_fct callback, void* ptr)
 * @brief Init the task timer.
 * @param delay The task delay in ms.
 * @param recursive recursive mode.
 * @param callback The task callback.
 * @param ptr The user pointer passed to the task callback.
 * @return The task ctx.
 */
systask_t systask_new(time_t delay, _Bool recursive, systask_callback_fct callback, void* ptr) {
  struct systask_s *s = NULL;
  if((s = (struct systask_s*)malloc(sizeof(struct systask_s))) == NULL) {
    logger(LOG_ERR, "Not enough memory to alloc a systask ctx");
    return NULL;
  }
  memset(s, 0, sizeof(struct systask_s));
  s->recursive = recursive;
  s->ref = SYSTASK_REF_ID;
  s->callback = callback;
  s->ptr = ptr;
  systask_set_timeout(s, delay);
  

  pthread_mutex_init(&s->thread.lock, NULL);
  pthread_mutex_init(&s->thread.cond_mutex, NULL);
  pthread_cond_init(&s->thread.cond, NULL);
  sem_init(&s->thread.sem, 0, 0);

  if(pthread_create(&s->thread.tid, NULL, systask_handler, s) != 0) {
    systask_delete(s);
    logger(LOG_ERR, "pthread_create failed: (%d) %s!", errno, strerror(errno));
    return NULL;
  }
  while(!s->ready) nanosleep(&(struct timespec){ 0, 1}, NULL);
  return s;
}

/**
 * @fn void systask_delete(systask_t task)
 * @brief Release the systask ctx.
 * @param task The systask ctx.
 */
void systask_delete(systask_t task) {
  create_ptr(s, task);
  if(!s || s->ref != SYSTASK_REF_ID) return;
  s->ready = 0;
  systask_stop(s);

  pthread_cancel(s->thread.tid);
  pthread_join(s->thread.tid, NULL);
  s->thread.tid = 0;
  
  pthread_mutex_destroy(&s->thread.lock);
  pthread_mutex_destroy(&s->thread.cond_mutex);
  pthread_cond_destroy(&s->thread.cond);
  sem_post(&s->thread.sem);
  sem_destroy(&s->thread.sem);
  s->ref = 0;
  free(s);
}

/**
 * @fn void systask_restart(systask_t task)
 * @brief ReStart the systask timer.
 * @param task The systask ctx.
 */
void systask_restart(systask_t task) {
  systask_stop(task);
  systask_start(task);
}

/**
 * @fn void systask_start(systask_t task)
 * @brief Start the systask timer.
 * @param task The systask ctx.
 */
void systask_start(systask_t task) {
  create_ptr(s, task);
  if(!s || s->ref != SYSTASK_REF_ID || !s->ready) return;
  pthread_mutex_lock(&s->thread.lock);
  if(s->start && !s->locked) {
    s->start = 0;
    pthread_mutex_lock(&s->thread.cond_mutex);
    if(s->sleep) pthread_cond_signal(&s->thread.cond);
    pthread_mutex_unlock(&s->thread.cond_mutex);
  }
  usleep(100);
  s->start = 1;
  sem_post(&s->thread.sem);
  pthread_mutex_unlock(&s->thread.lock);
}

/**
 * @fn void systask_stop(systask_t task)
 * @brief Finish the systask timer.
 * @param task The systask ctx.
 */
void systask_stop(systask_t task) {
  create_ptr(s, task);
  if(!s || s->ref != SYSTASK_REF_ID || !s->ready || !s->start) return;
  pthread_mutex_lock(&s->thread.lock);
  if(s->start && s->locked) {
    s->start = 0;
    sem_post(&s->thread.sem);
  } else {
    s->start = 0;
    pthread_mutex_lock(&s->thread.cond_mutex);
    if(s->sleep) pthread_cond_signal(&s->thread.cond);
    pthread_mutex_unlock(&s->thread.cond_mutex);
  }
  usleep(100);
  pthread_mutex_unlock(&s->thread.lock);
}

/**
 * @fn void systask_set_timeout(systask_t task, time_t delay)
 * @brief Change the task delay.
 * @param task The systask ctx.
 * @param delay The delay in ms.
 */
void systask_set_timeout(systask_t task, time_t delay) {
  create_ptr(s, task);
  if(!s || s->ref != SYSTASK_REF_ID) return;
  s->delay = delay;
}

/**
 * @fn void systask_get_timeout(systask_t task)
 * @brief Get the task delay.
 * @param task The systask ctx.
 * @return The delay in ms.
 */
time_t systask_get_timeout(systask_t task) {
  create_ptr(s, task);
  if(!s || s->ref != SYSTASK_REF_ID) return 0;
  return s->delay;
}


static void* systask_handler(void* ptr) {
  create_ptr(s, ptr);
  s->ready = 1;
  while(s->ready) {
    /* notify locked */
    pthread_mutex_lock(&s->thread.lock);
    s->locked = 1;
    pthread_mutex_unlock(&s->thread.lock);
    /* wait for the next event. */
    sem_wait(&s->thread.sem);
    /* notify unlocked */
    pthread_mutex_lock(&s->thread.lock);
    s->locked = 0;
    pthread_mutex_unlock(&s->thread.lock);
    /* task loop */
    while(s->start) {
      /* using of a preemptible sleep */
      pthread_mutex_lock(&s->thread.cond_mutex);
      s->sleep = 1;

      struct timeval now; 
      gettimeofday(&now, NULL); 
      long long abstime_ns_large = now.tv_usec*1000 + (s->delay*1000000); 
      struct timespec abstime = { 
	now.tv_sec + (abstime_ns_large / 1000000000), 
	abstime_ns_large % 1000000000 
      };
      if(pthread_cond_timedwait(&s->thread.cond, &s->thread.cond_mutex, &abstime) != ETIMEDOUT) {
	s->sleep = 0;
	pthread_mutex_unlock(&s->thread.cond_mutex);
	break;
      }
      s->sleep = 0;
      pthread_mutex_unlock(&s->thread.cond_mutex);
      /* is start is canceled after the timeout */
      pthread_mutex_lock(&s->thread.lock);
      if(!s->start) {
	pthread_mutex_unlock(&s->thread.lock);
	break;
      }
      pthread_mutex_unlock(&s->thread.lock);
      s->callback(s->ptr);
      if(!s->recursive) break;
    }
  }
  pthread_exit(0);
  return NULL;
}

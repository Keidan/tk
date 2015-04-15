/**
 *******************************************************************************
 * @file systask.h
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
#ifndef __SYSTASK_H__
  #define __SYSTASK_H__

  #include <time.h>

  typedef void (*systask_callback_fct)(void* ptr);

  typedef void* systask_t;

  /**
   * @fn systask_t systask_new(time_t delay, _Bool recursive, systask_callback_fct callback, void* ptr)
   * @brief Init the task timer.
   * @param delay The task delay in ms.
   * @param recursive recursive mode.
   * @param callback The task callback.
   * @param ptr The user pointer passed to the task callback.
   * @return The task ctx.
   */
  systask_t systask_new(time_t delay, _Bool recursive, systask_callback_fct callback, void* ptr);

  /**
   * @fn void systask_delete(systask_t task)
   * @brief Release the systask ctx.
   * @param task The systask ctx.
   */
  void systask_delete(systask_t task);

  /**
   * @fn void systask_restart(systask_t task)
   * @brief ReStart the systask timer.
   * @param task The systask ctx.
   */
  void systask_restart(systask_t task);

  /**
   * @fn void systask_start(systask_t task)
   * @brief Start the systask timer.
   * @param task The systask ctx.
   */
  void systask_start(systask_t task);

  /**
   * @fn void systask_stop(systask_t task)
   * @brief Finish the systask timer.
   * @param task The systask ctx.
   */
  void systask_stop(systask_t task);

  /**
   * @fn void systask_set_timeout(systask_t task, time_t delay)
   * @brief Change the task delay.
   * @param task The systask ctx.
   * @param delay The delay in ms.
   */
  void systask_set_timeout(systask_t task, time_t delay);

  /**
   * @fn void systask_get_timeout(systask_t task)
   * @brief Get the task delay.
   * @param task The systask ctx.
   * @return The delay in ms.
   */
  time_t systask_get_timeout(systask_t task);

#endif /* __SYSTASK_H__ */

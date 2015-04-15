/**
 *******************************************************************************
 * @file shell_term.h
 * @author Keidan
 * @date 01/08/2013
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
#ifndef __SHELL_TERM_H__
  #define __SHELL_TERM_H__

  #include <stdio.h>
  #include <stdbool.h>
  #include <stdarg.h>
  #include <ncurses.h>


  #ifndef __SHELL_TERM_DEFAULT_BACKGROUND__
    #define __SHELL_TERM_DEFAULT_BACKGROUND__ STC_BLACK
  #endif /* __SHELL_TERM_DEFAULT_BACKGROUND__ */
  #ifndef __SHELL_TERM_DEFAULT_FOREGROUND__
    #define __SHELL_TERM_DEFAULT_FOREGROUND__ STC_WHITE
  #endif /* __SHELL_TERM_DEFAULT_FOREGROUND__ */

  /* some keyboard keys */
  #define VK_UP            0x103
  #define VK_RIGHT         0x105
  #define VK_LEFT          0x104
  #define VK_DOWN          0x102
  #define VK_BACKSPACE     0x107
  #define VK_BACKSPACE_ALT 0x07F
  #define VK_TAB           0x009
  #define VK_CTRL_A        0x001
  #define VK_CTRL_E        0x005
  #define VK_CTRL_D        0x004
  #define VK_CTRL_K        0x00B
  #define VK_CTRL_L        0x00C
  #define VK_PAGE_DOWN     0x152
  #define VK_PAGE_UP       0x153
  #define VK_DEL           0x14A
  #define VK_ENTER         0x00A
					
  /* some colors */
  typedef enum {
    STC_BLACK = 0,
    STC_RED,
    STC_GREEN,
    STC_YELLOW,
    STC_BLUE,
    STC_MAGENTA,
    STC_CYAN,
    STC_WHITE,
  } stc_et;


  typedef void* shell_term_t;

  /**
   * @fn void shell_term_icanon(_Bool enable)
   * @brief Switch the terminal in canonical read.
   * @param enable canon state.
   */
  void shell_term_icanon(_Bool enable);

  /**
   * @fn shell_term_t shell_term_new()
   * @brief New terminal connection
   */
  shell_term_t shell_term_new(); 

  /**
   * @fn void shell_term_delete(shell_term_t term)
   * @brief Delete the terminal connection
   * @param term The terminal pointer
   */
  void shell_term_delete(shell_term_t term); 

  /**
   * @fn int shell_term_getch()
   * @brief Char request.
   * @return The char
   */
  int shell_term_getch();

  /**
   * @fn int shell_term_wgetch(shell_term_t term)
   * @brief Char request into the current window.
   * @param term The terminal pointer
   * @return The char
   */
  int shell_term_wgetch(shell_term_t term);
	
  /**
   * @fn void shell_term_clrscr()
   * @brief Clear the screen.
   */
  void shell_term_clrscr();

  /**
   * @fn void shell_term_set_xy(shell_term_t term, int x, int y)
   * @brief Change the x and y position of the cursor
   * @param term The terminal pointer
   * @param x X position.
   * @param y Y position.
   */
  void shell_term_set_xy(shell_term_t term, int x, int y);

  /**
   * @fn void shell_term_get_xy(shell_term_t term, int *x, int *y)
   * @brief Get the x and y position of the cursor.
   * @param term The terminal pointer
   * @param x X position.
   * @param y Y position.
   */
  void shell_term_get_xy(shell_term_t term, int *x, int *y);

  /**
   * @fn void shell_term_get_max_size(shell_term_t term, int *width, int *height)
   * @brief Get the maximum terminal size.
   * @param term The terminal pointer
   * @param width The terminal width.
   * @param height The terminal height
   */
  void shell_term_get_max_size(shell_term_t term, int *width, int *height);

  /**
   * @fn void shell_term_restore(shell_term_t term)
   * @brief Restore all effects.
   * @param term The terminal pointer
   */
  void shell_term_restore(shell_term_t term);
				
  /**
   * @fn void shell_term_set_background(shell_term_t term, int background)
   * @brief Change the line background (Please export TERM=xterm-256color)
   * @param term The terminal pointer
   */		
  void shell_term_set_background(shell_term_t term, int background);

  /**
   * @fn void shell_term_set_foreground(shell_term_t term, int foreground)
   * @brief Change the line foreground (Please export TERM=xterm-256color)
   * @param term The terminal pointer
   */
  void shell_term_set_foreground(shell_term_t term, int foreground);
				
  /**
   * @fn void shell_term_set_color_on(shell_term_t term, int background, int foreground)
   * @brief Change the line colors (Please export TERM=xterm-256color)
   * @param term The terminal pointer
   */	     
  void shell_term_set_color_on(shell_term_t term, int background, int foreground);

  /**
   * @fn void shell_term_set_color_off(shell_term_t term)
   * @brief Switch of the terminal colors (Please export TERM=xterm-256color)
   * @param term The terminal pointer
   */
  void shell_term_set_color_off(shell_term_t term);

  /**
   * @fn void shell_term_set_bold(shell_term_t term, _Bool b)
   * @brief Enable/Disable bold effect.
   * @param term The terminal pointer.
   * @param b Effect state.
   */
  void shell_term_set_bold(shell_term_t term, _Bool b);	

  /**
   * @fn void shell_term_set_standout(shell_term_t term, _Bool b)
   * @brief Enable/Disable standout effect.
   * @param term The terminal pointer
   * @param b Effect state.
   */
  void shell_term_set_standout(shell_term_t term, _Bool b);
	
  /**
   * @fn void shell_term_set_underline(shell_term_t term, _Bool b)
   * @brief Enable/Disable underline effect.
   * @param term The terminal pointer
   * @param b Effect state.
   */
  void shell_term_set_underline(shell_term_t term, _Bool b);
	
  /**
   * @fn void shell_term_set_reverse(shell_term_t term, _Bool b)
   * @brief Enable/Disable reverse effect.
   * @param term The terminal pointer
   * @param b Effect state.
   */
  void shell_term_set_reverse(shell_term_t term, _Bool b);
	
  /**
   * @fn void shell_term_set_blink(shell_term_t term, _Bool b)
   * @brief Enable/Disable blink effect.
   * @param term The terminal pointer
   * @param b Effect state.
   */
  void shell_term_set_blink(shell_term_t term, _Bool b);
	
  /**
   * @fn void shell_term_set_invis(shell_term_t term, _Bool b)
   * @brief Enable/Disable invis effect.
   * @param term The terminal pointer
   * @param b Effect state.
   */
  void shell_term_set_invis(shell_term_t term, _Bool b);

  /**
   * @fn void shell_term_printf(shell_term_t term, const char* fmt, ...)
   * @brief Print a string into the terminal.
   * @param term The terminal pointer
   * @param fmt The string format
   * @param ... The arguments
   */
  void shell_term_printf(shell_term_t term, const char* fmt, ...);
#endif /* __SHELL_TERM_H__ */

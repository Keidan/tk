/**
 *******************************************************************************
 * @file shell_term.c
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
#include <tk/shell/shell_term.h>
#include <tk/sys/log.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>

struct shell_term_s {
    WINDOW*        win;
    sem_t          sinit;
    pthread_t      tid;
    int            background;
    int            foreground;
    int            pair_color;
    _Bool          color_supported;
    _Bool          bold;
    _Bool          standout;
    _Bool          underline;
    _Bool          reverse;
    _Bool          blink;
    _Bool          invis;
};

#define if_color_off(term) ({				\
    if(t->pair_color >= 0) {				\
      wattroff(t->win, COLOR_PAIR(t->pair_color));	\
      t->pair_color = -1;				\
    }							\
  })
#define attr_switch(term, name, key) t->name ? wattron(t->win, key) : wattroff(t->win, key)

/**
 * @fn void shell_term_icanon(_Bool enable)
 * @brief Switch the terminal in canonical read.
 * @param enable canon state.
 */
void shell_term_icanon(_Bool enable) {
  // Canonical or non canonical read
  enable ? system("stty icanon") : system("stty -icanon");
}

static void* shell_term_init_thread_cb(void* ptr) {
  struct shell_term_s *t = (struct shell_term_s*)ptr;
  t->color_supported = 1;
  t->win = initscr();
  if (!has_colors() || !can_change_color())
    t->color_supported = 0;
  raw();
  cbreak();
  noecho();
  curs_set(1); /* visible */
  if(t->color_supported) {
    start_color();
    assume_default_colors(__SHELL_TERM_DEFAULT_FOREGROUND__, __SHELL_TERM_DEFAULT_BACKGROUND__);
  }
  keypad(t->win, 1);
  scrollok(t->win, 1);
  sem_post(&t->sinit);
  t->tid = 0;
  pthread_exit(0);
  return NULL;
}

/**
 * @fn shell_term_t shell_term_new()
 * @brief New terminal connection
 */
shell_term_t shell_term_new() {
  struct shell_term_s *term = malloc(sizeof(struct shell_term_s));
  if(!term) {
    logger(LOG_ERR, "Not enough memory.\n");
    return NULL;
  }
  memset(term, 0, sizeof(struct shell_term_s));
  sem_init(&term->sinit, 0, 0);

  term->tid = 0;
  int pthread_retcode = pthread_create(&term->tid, NULL, shell_term_init_thread_cb, term);
  if(pthread_retcode < 0) {
    logger(LOG_ERR, "Unable to create the init thread!\n");
    sem_destroy(&term->sinit);
    free(term);
    return NULL;
  }
  pthread_retcode = pthread_detach(term->tid);
  if(pthread_retcode < 0) {
    pthread_cancel(term->tid);
    logger(LOG_ERR, "Unable to detach the init thread!\n");
    sem_destroy(&term->sinit);
    free(term);
    return NULL;
  }
  sem_wait(&term->sinit);/* wait ncurses is realy loaded */
  sem_destroy(&term->sinit);

  term->pair_color = -1;
  term->background = __SHELL_TERM_DEFAULT_BACKGROUND__;
  term->foreground = __SHELL_TERM_DEFAULT_FOREGROUND__;
  if(!term->color_supported)
    logger(LOG_ERR, "Error: unable to change colors, "
	   "trying setting your TERM type to enable colors."
	   "Please export TERM=xterm-256color\n");
  return term;
}

/**
 * @fn void shell_term_delete(shell_term_t term)
 * @brief Delete the terminal connection
 * @param term The terminal pointer
 */
void shell_term_delete(shell_term_t term) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  if(t) {
    echo();
    //curs_set(1); /* restore */
    delwin(t->win), t->win = NULL;
    endwin();
    free(t);
  }
}

/**
 * @fn void shell_term_print(shell_term_t term, stringbuffer_t buffer)
 * @brief Print a string into the terminal.
 * @param term The terminal pointer
 * @param buffer The string
 */
void shell_term_print(shell_term_t term, stringbuffer_t buffer) {
  if(!term) return;
  struct shell_term_s *t = (struct shell_term_s*)term;
  wprintw(t->win, "%s", stringbuffer_to_str(buffer));
  refresh();
}

/**
 * @fn int shell_term_getch()
 * @brief Char request.
 * @return The char
 */
int shell_term_getch() {
  return getch();
}

/**
 * @fn int shell_term_wgetch(shell_term_t term)
 * @brief Char request into the current window.
 * @param term The terminal pointer
 * @return The char
 */
int shell_term_wgetch(shell_term_t term) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  return wgetch(t->win);
}
	
/**
 * @fn void shell_term_clrscr()
 * @brief Clear the screen.
 */
void shell_term_clrscr() {
  clear();
  refresh();
}

/**
 * @fn void shell_term_set_xy(shell_term_t term, int x, int y)
 * @brief Change the x and y position of the cursor
 * @param term The terminal pointer
 * @param x X position.
 * @param y Y position.
 */
void shell_term_set_xy(shell_term_t term, int x, int y) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  wmove(t->win, y, x);
  refresh();
}

/**
 * @fn void shell_term_get_xy(shell_term_t term, int *x, int *y)
 * @brief Get the x and y position of the cursor.
 * @param term The terminal pointer
 * @param x X position.
 * @param y Y position.
 */
void shell_term_get_xy(shell_term_t term, int *x, int *y) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  getyx(t->win, *y, *x);
}

/**
 * @fn void shell_term_get_max_size(shell_term_t term, int *width, int *height)
 * @brief Get the maximum terminal size.
 * @param term The terminal pointer
 * @param width The terminal width.
 * @param height The terminal height
 */
void shell_term_get_max_size(shell_term_t term, int *width, int *height) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  getmaxyx(t->win, *height, *width);
}

/**
 * @fn void shell_term_restore(shell_term_t term)
 * @brief Restore all effects.
 * @param term The terminal pointer
 */
void shell_term_restore(shell_term_t term) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  if(t->color_supported) if_color_off(t);
  t->bold = 0;
  t->standout = 0;
  t->underline = 0;
  t->reverse = 0;
  t->blink = 0;
  t->invis = 0;
  attr_switch(t, bold, A_BOLD);
  attr_switch(t, standout, A_STANDOUT);
  attr_switch(t, underline, A_UNDERLINE);
  attr_switch(t, reverse, A_REVERSE);
  attr_switch(t, blink, A_BLINK);
  attr_switch(t, invis, A_INVIS);
  refresh();
}
						
/**
 * @fn void shell_term_set_background(shell_term_t term, int background)
 * @brief Change the line background (Please export TERM=xterm-256color)
 * @param term The terminal pointer
 */				
void shell_term_set_background(shell_term_t term, int background) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  shell_term_set_color_on(t, background, t->foreground);
}

/**
 * @fn void shell_term_set_foreground(shell_term_t term, int foreground)
 * @brief Change the line foreground (Please export TERM=xterm-256color)
 * @param term The terminal pointer
 */
void shell_term_set_foreground(shell_term_t term, int foreground) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  shell_term_set_color_on(t, t->background, foreground);
}
			
/**
 * @fn void shell_term_set_color_on(shell_term_t term, int background, int foreground)
 * @brief Change the line colors (Please export TERM=xterm-256color)
 * @param term The terminal pointer
 */				
void shell_term_set_color_on(shell_term_t term, int background, int foreground) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  if(!t->color_supported) return;
  t->background = background;
  t->foreground = foreground;
  if_color_off(t);
  t->pair_color = t->foreground;
  init_pair(t->pair_color, t->foreground, t->background);
  wattron(t->win, COLOR_PAIR(t->pair_color));
  refresh();
}
		
/**
 * @fn void shell_term_set_color_off(shell_term_t term)
 * @brief Switch of the terminal colors (Please export TERM=xterm-256color)
 * @param term The terminal pointer
 */				
void shell_term_set_color_off(shell_term_t term) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  if(!t->color_supported) return;
  if_color_off(t);
  refresh();
}
		
/**
 * @fn void shell_term_set_bold(shell_term_t term, _Bool b)
 * @brief Enable/Disable bold effect.
 * @param term The terminal pointer
 */				
void shell_term_set_bold(shell_term_t term, _Bool bold) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  t->bold = bold;
  attr_switch(t, bold, A_BOLD);
  refresh();
}	

/**
 * @fn void shell_term_set_standout(shell_term_t term, _Bool b)
 * @brief Enable/Disable standout effect.
 * @param term The terminal pointer
 * @param b Effect state.
 */
void shell_term_set_standout(shell_term_t term, _Bool b) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  t->standout = b;
  attr_switch(t, standout, A_STANDOUT);
  refresh();
}	
	
/**
 * @fn void shell_term_set_underline(shell_term_t term, _Bool b)
 * @brief Enable/Disable underline effect.
 * @param term The terminal pointer
 * @param b Effect state.
 */
void shell_term_set_underline(shell_term_t term, _Bool b) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  t->underline = b;
  attr_switch(t, underline, A_UNDERLINE);
  refresh();
}	
	
/**
 * @fn void shell_term_set_reverse(shell_term_t term, _Bool b)
 * @brief Enable/Disable reverse effect.
 * @param term The terminal pointer
 * @param b Effect state.
 */
void shell_term_set_reverse(shell_term_t term, _Bool b) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  t->reverse = b;
  attr_switch(t, reverse, A_REVERSE);
  refresh();
}	
	
/**
 * @fn void shell_term_set_blink(shell_term_t term, _Bool b)
 * @brief Enable/Disable blink effect.
 * @param term The terminal pointer
 * @param b Effect state.
 */
void shell_term_set_blink(shell_term_t term, _Bool b) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  t->blink = b;
  attr_switch(t, blink, A_BLINK);
  refresh();
}	
	
/**
 * @fn void shell_term_set_invis(shell_term_t term, _Bool b)
 * @brief Enable/Disable invis effect.
 * @param term The terminal pointer
 * @param b Effect state.
 */
void shell_term_set_invis(shell_term_t term, _Bool b) {
  struct shell_term_s *t = (struct shell_term_s*)term;
  t->invis = b;
  attr_switch(t, invis, A_INVIS);
  refresh();
}

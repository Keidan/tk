#include <tk/shell/sterm.h>
#include <tk/sys/log.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>

struct sterm_s {
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

#define TERM(term) ((struct sterm_s*)term)

#define if_color_off(term) ({						\
    if(TERM(term)->pair_color >= 0) {					\
      wattroff(TERM(term)->win, COLOR_PAIR(TERM(term)->pair_color));	\
      TERM(term)->pair_color = -1;					\
    }									\
})
#define attr_switch(term, name, key) TERM(term)->name ? wattron(TERM(term)->win, key) : wattroff(TERM(term)->win, key)

void sterm_icanon(_Bool enable) {
  // Canonical or non canonical read
  enable ? system("stty icanon") : system("stty -icanon");
}

void* sterm_init_thread_cb(void* ptr) {
  TERM(ptr)->color_supported = 1;
  TERM(ptr)->win = initscr();
  if (!has_colors() || !can_change_color())
    TERM(ptr)->color_supported = 0;
  raw();
  cbreak();
  noecho();
  curs_set(1); /* visible */
  if(TERM(ptr)->color_supported) {
    start_color();
    assume_default_colors(__STERM_DEFAULT_FOREGROUND__, __STERM_DEFAULT_BACKGROUND__);
  }
  keypad(TERM(ptr)->win, 1);
  scrollok(TERM(ptr)->win, 1);
  sem_post(&TERM(ptr)->sinit);
  TERM(ptr)->tid = 0;
  pthread_exit(0);
  return NULL;
}

sterm_t sterm_open() {
  struct sterm_s *term = malloc(sizeof(struct sterm_s));
  if(!term) {
    fprintf(stderr, "Not enough memory.\n");
    exit(1);
  }
  memset(term, 0, sizeof(struct sterm_s));
  sem_init(&term->sinit, 0, 0);

  term->tid = 0;
  int pthread_retcode = pthread_create(&term->tid, NULL, sterm_init_thread_cb, term);
  if(pthread_retcode < 0) {
    fprintf(stderr, "Unable to create the init thread!\n");
    sem_destroy(&term->sinit);
    free(term);
    return NULL;
  }
  pthread_retcode = pthread_detach(term->tid);
  if(pthread_retcode < 0) {
    pthread_cancel(term->tid);
    fprintf(stderr, "Unable to detach the init thread!\n");
    sem_destroy(&term->sinit);
    free(term);
    return NULL;
  }
  sem_wait(&term->sinit);/* wait ncurses is realy loaded */
  sem_destroy(&term->sinit);

  term->pair_color = -1;
  term->background = __STERM_DEFAULT_BACKGROUND__;
  term->foreground = __STERM_DEFAULT_FOREGROUND__;
  if(!term->color_supported)
    sterm_printf(term, "Error: unable to change colors, "
	   "trying setting your TERM type to enable colors.\n"
	    "Trying to export TERM=xterm-256color\n");
  
  return term;
}
void sterm_close(sterm_t term) {
  if(term) {
    echo();
    //curs_set(1); /* restore */
    delwin(TERM(term)->win), TERM(term)->win = NULL;
    endwin();
    free(term);
  }
}


void sterm_printf(sterm_t term, const char* format, ...) {
  if(!term) return;
  va_list args;
  va_start(args, format);
  vwprintw(TERM(term)->win, format, args);
  va_end(args);
  refresh();
}

int sterm_getch() {
  return getch();
}

int sterm_wgetch(sterm_t term) {
  return wgetch(TERM(term)->win);
}

void sterm_clrscr(sterm_t term) {
  clear();
  refresh();
}

void sterm_set_xy(sterm_t term, int x, int y) {
  wmove(TERM(term)->win, y, x);
  refresh();
}

void sterm_get_xy(sterm_t term, int *x, int *y) {
  getyx(TERM(term)->win, *y, *x);
}

void sterm_get_max_size(sterm_t term, int *width, int *height) {
  getmaxyx(TERM(term)->win, *height, *width);
}

void sterm_restore(sterm_t term) {
  if(TERM(term)->color_supported) if_color_off(term);
  TERM(term)->bold = 0;
  TERM(term)->standout = 0;
  TERM(term)->underline = 0;
  TERM(term)->reverse = 0;
  TERM(term)->blink = 0;
  TERM(term)->invis = 0;
  attr_switch(term, bold, A_BOLD);
  attr_switch(term, standout, A_STANDOUT);
  attr_switch(term, underline, A_UNDERLINE);
  attr_switch(term, reverse, A_REVERSE);
  attr_switch(term, blink, A_BLINK);
  attr_switch(term, invis, A_INVIS);
  refresh();
}
						
void sterm_set_background(sterm_t term, int background) {
  sterm_set_color_on(term, background, TERM(term)->foreground);
}
void sterm_set_foreground(sterm_t term, int foreground) {
  sterm_set_color_on(term, TERM(term)->background, foreground);
}

						
void sterm_set_color_on(sterm_t term, int background, int foreground) {
  if(!TERM(term)->color_supported) return;
  TERM(term)->background = background;
  TERM(term)->foreground = foreground;
  if_color_off(term);
  TERM(term)->pair_color = TERM(term)->foreground;
  init_pair(TERM(term)->pair_color, TERM(term)->foreground, TERM(term)->background);
  wattron(TERM(term)->win, COLOR_PAIR(TERM(term)->pair_color));
  refresh();
}
						
void sterm_set_color_off(sterm_t term) {
  if(!TERM(term)->color_supported) return;
  if_color_off(term);
  refresh();
}
						
void sterm_set_bold(sterm_t term, _Bool bold) {
  TERM(term)->bold = bold;
  attr_switch(term, bold, A_BOLD);
  refresh();
}	

void sterm_set_standout(sterm_t term, _Bool b) {
  TERM(term)->standout = b;
  attr_switch(term, standout, A_STANDOUT);
  refresh();
}	

void sterm_set_underline(sterm_t term, _Bool b) {
  TERM(term)->underline = b;
  attr_switch(term, underline, A_UNDERLINE);
  refresh();
}	

void sterm_set_reverse(sterm_t term, _Bool b) {
  TERM(term)->reverse = b;
  attr_switch(term, reverse, A_REVERSE);
  refresh();
}	

void sterm_set_blink(sterm_t term, _Bool b) {
  TERM(term)->blink = b;
  attr_switch(term, blink, A_BLINK);
  refresh();
}	

void sterm_set_invis(sterm_t term, _Bool b) {
  TERM(term)->invis = b;
  attr_switch(term, invis, A_INVIS);
  refresh();
}

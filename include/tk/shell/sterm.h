#ifndef __STERM_H__
  #define __STERM_H__

  #include <stdio.h>
  #include <ncurses.h>


  #ifndef __STERM_DEFAULT_BACKGROUND__
    #define __STERM_DEFAULT_BACKGROUND__ STC_BLACK
  #endif /* __STERM_DEFAULT_BACKGROUND__ */
  #ifndef __STERM_DEFAULT_FOREGROUND__
    #define __STERM_DEFAULT_FOREGROUND__ STC_WHITE
  #endif /* __STERM_DEFAULT_FOREGROUND__ */

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


  typedef void* sterm_t;


  void sterm_icanon(_Bool enable);

  sterm_t sterm_open(); 
  void sterm_close(sterm_t term); 

  int sterm_getch();
  int sterm_wgetch(sterm_t term);
	
  void sterm_clrscr(sterm_t term);
  void sterm_set_xy(sterm_t term, int x, int y);
  void sterm_get_xy(sterm_t term, int *x, int *y);
  void sterm_get_max_size(sterm_t term, int *width, int *height);
  void sterm_restore(sterm_t term);
						
  void sterm_set_background(sterm_t term, int background);
  void sterm_set_foreground(sterm_t term, int foreground);					     
  void sterm_set_color_on(sterm_t term, int background, int foreground);
  void sterm_set_color_off(sterm_t term);

  void sterm_set_bold(sterm_t term, _Bool b);	
  void sterm_set_standout(sterm_t term, _Bool b);	
  void sterm_set_underline(sterm_t term, _Bool b);	
  void sterm_set_reverse(sterm_t term, _Bool b);	
  void sterm_set_blink(sterm_t term, _Bool b);	
  void sterm_set_invis(sterm_t term, _Bool b);	


  void sterm_printf(sterm_t term, const char* format, ...);
#endif /* __STERM_H__ */

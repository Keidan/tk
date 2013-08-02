#include <stdio.h>
#include <stdlib.h>
#include <tk/shell/sterm.h>
#include <unistd.h>

int main(int argc, char** argv) {
  sterm_t term = sterm_open();
  int x,y;
  sterm_get_xy(term, &x, &y);
  sterm_set_bold(term, 1);
  sterm_printf(term, "X:%d, Y:%d\n", x, y);
  sterm_set_bold(term, 0);
  sterm_printf(term, "X:%d, Y:%d\n", x, y);
  sterm_wgetch(term);
  sterm_close(term);
  return 0;
}

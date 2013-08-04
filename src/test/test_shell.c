#include <stdio.h>
#include <stdlib.h>
#include <tk/shell/shell_term.h>
#include <unistd.h>

int main(int argc, char** argv) {
  shell_term_t term = shell_term_new();
  int x,y;
  shell_term_get_xy(term, &x, &y);
  shell_term_set_bold(term, 1);
  shell_term_printf(term, "X:%d, Y:%d\n", x, y);
  shell_term_set_bold(term, 0);
  shell_term_printf(term, "X:%d, Y:%d\n", x, y);
  shell_term_wgetch(term);
  shell_term_delete(term);
  return 0;
}

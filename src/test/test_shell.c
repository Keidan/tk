#include <stdio.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/shell/shell.h>
#include <unistd.h>


int main(int argc, char** argv) {

  log_init("test_shell", LOG_PID, LOG_USER);

  shell_t s = shell_new();
  shell_term_t term = shell_get_term(s);
  int x,y;
  shell_term_get_xy(term, &x, &y);
  shell_term_set_bold(term, 1);
  //shell_term_printf(term, "X:%d, Y:%d\n", x, y);
  shell_term_set_bold(term, 0);
//  shell_term_printf(term, "X:%d, Y:%d\n", x, y);
  shell_term_wgetch(term);
  shell_delete(s);

  log_close();
  return 0;
}

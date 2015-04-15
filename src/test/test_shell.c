#include <stdio.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/shell/shell.h>
#include <tk/sys/syssig.h>
#include <unistd.h>


int main(int argc, char** argv) {
  syssig_init(log_init_cast_user("test_shell", LOG_PID), NULL);

  shell_t s = shell_new(0);
  shell_set_prompt(s, "$");
  shell_main_loop(s);
  shell_delete(s);

  log_close();
  return 0;
}

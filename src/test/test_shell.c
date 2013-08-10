#include <stdio.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/shell/shell.h>
#include <tk/sys/sysutils.h>
#include <unistd.h>


int main(int argc, char** argv) {

  sysutils_exit_action(log_init_cast("test_shell", LOG_PID, LOG_USER), NULL);

  shell_t s = shell_new(0);
  shell_set_prompt(s, "$");
  shell_main_loop(s);
  shell_delete(s);

  log_close();
  return 0;
}

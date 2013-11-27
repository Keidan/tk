#include <tk/sys/process.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/syssig.h>
#include <unistd.h>

int main(int argc, char** argv) {
  syssig_init(log_init_cast_user("test_process", LOG_PID|LOG_CONS|LOG_PERROR), NULL);

  stringbuffer_t b;
  b = process_execute("ls -als /home/kei");
  printf("The result: \n'%s'\n", stringbuffer_to_str(b));
  stringbuffer_delete(b);

  log_close();
  return 0;
}

#include <tk/sys/process.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/syssig.h>
#include <tk/utils/string.h>
#include <unistd.h>


int main(int argc, char** argv) {
  syssig_init(log_init_cast_user("test_process", LOG_PID|LOG_CONS|LOG_PERROR), NULL);

//  stringbuffer_t b;
//  b = process_exec("ls -als /home/kei");
//  printf("The result: \n'%s'\n", stringbuffer_to_str(b));
//  stringbuffer_delete(b);

  struct pstream_s stream;
  process_exec_p(&stream, "/home/kei/devel/tk/plop.sh", NULL);
  char buf[1024];
  read(stream.fd_out, buf, sizeof(buf));
  printf("'%s'\n", buf);
  if(string_indexof(buf, "(yes/no)") != -1) {
    write(stream.fd_in, "yes\n", strlen("yes\n"));
    read(stream.fd_out, buf, sizeof(buf));
    printf("'%s'\n", buf);
  }
  write(stream.fd_in, "no\n", strlen("no\n"));
  process_kill_p(&stream);
  log_close();
  return 0;
}

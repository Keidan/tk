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
  process_exec_p(&stream, "/bin/bash", "bash");
  char buf[1024];
  read(stream.fd_out, buf, sizeof(buf));
  printf("'%s'\n", buf);
  write(stream.fd_in, "echo \"plop\"", strlen("echo \"plop\""));
  read(stream.fd_out, buf, sizeof(buf));
  printf("'%s'\n", buf);
  write(stream.fd_in, "exit", strlen("exit"));
  read(stream.fd_out, buf, sizeof(buf));
  printf("'%s'\n", buf);
  process_kill_p(&stream);
  log_close();
  return 0;
}

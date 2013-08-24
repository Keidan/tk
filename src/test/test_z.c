#include <tk/sys/z.h>
#include <stdio.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/ssig.h>
#include <unistd.h>


int main(int argc, char** argv) {
  ssig_init(log_init_cast_user("test_z", LOG_PID), NULL);
  z_t z = z_new();
  fifo_t files = fifo_new();
  fifo_push(files, "plop/test_shell");
  fifo_push(files, "plop/test_stringbuffer");
  fifo_push(files, "plop/z/test_z");
  int r = z_compress(z, "plop.zip", "azerty", Z_C_BETTER, 0, 0, files);
  printf("Compression state: %d\n", r);
  fifo_delete(files);

  z_delete(z);

  return 0;
}

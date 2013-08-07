#include <stdio.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/text/stringbuffer.h>
#include <unistd.h>


int main(int argc, char** argv) {

  log_init("test_stringbuffer", LOG_PID, LOG_USER);

  stringbuffer_t b = stringbuffer_new();
  stringbuffer_append(b, "azerty");
  stringbuffer_append(b, "1234567890");
  printf("Buffer after appendx2: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_copy(b, "12");
  printf("Buffer after copy: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_copy(b, "34567");
  printf("Buffer after copy: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_erase(b, 2, 2);
  printf("Buffer after erase: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_erase(b, 0, 1);
  printf("Buffer after erase: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_copy(b, "AZERTYUIOPQSDFGHJKLMWXCVBN");
  printf("Buffer after copy: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_erase2(b, 5);
  printf("Buffer after erase2: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_insert(b, 2, "blabla");
  printf("Buffer after insert: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_copy(b, "AZERTYUIOPQSDFGHJKLMWXCVBN3");
  printf("Buffer after copy: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_copy(b, "AZERTYUIOPQSDFGHJKLMWXCVBN1234567890987654321");
  printf("Buffer after copy: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_copy(b, "AZ12345");
  printf("Buffer after copy: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_insert(b, 2, "ERTY");
  printf("Buffer after insert: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_trim_to_size(b);
  printf("Buffer after trim: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_copy(b, "123");
  printf("Buffer after copy: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_insert(b, 0, "0ab");
  printf("Buffer after insert: '%s'\n", stringbuffer_to_str(b));
  stringbuffer_delete(b);

  log_close();
  return 0;
}

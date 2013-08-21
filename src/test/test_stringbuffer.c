#include <stdio.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/text/stringbuffer.h>
#include <tk/sys/ssig.h>
#include <unistd.h>


int main(int argc, char** argv) {
  ssig_init(log_init_cast_user("test_stringbuffer", LOG_PID), NULL);

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

  stringbuffer_printf(b, "Hellow %s %d%d%d%d %c %x%X%#X %p", "from:klhjkhjkhjkhjk", 10,11,12,13, 'A', 255,255,255, b);
  printf("Buffer after printf: '%s'\n", stringbuffer_to_str(b));

  stringbuffer_delete(b);

  return 0;
}

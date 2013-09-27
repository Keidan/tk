#include <tk/io/net/netiface.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/syssig.h>
#include <unistd.h>


int main(int argc, char** argv) {
  syssig_init(log_init_cast_user("test_iface", LOG_PID), NULL);

  char** keys;
  int i, count;

  htable_t ifaces = netiface_list_new(NETIFACE_LVL_UDP, NETIFACE_KEY_NAME);
  count = htable_get_keys(ifaces, &keys);
  for(i = 0; i < count; i++) {
    netiface_t iface = netiface_list_get(ifaces, keys[i]);
    struct netiface_info_s info;
    netiface_read(iface, &info);
    netiface_print(stdout, info);
  }
  netiface_list_delete(ifaces);
  log_close();
  return 0;
}

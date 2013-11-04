#include <tk/io/net/netiface.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/syssig.h>
#include <unistd.h>


int main(int argc, char** argv) {
  syssig_init(log_init_cast_user("test_iface", LOG_PID|LOG_CONS|LOG_PERROR), NULL);

  char** keys;
  int i, count;

  netiface_create("eth0:1", "10.101.0.10");

  htable_t ifaces = netiface_list_new(NETIFACE_LVL_UDP, NETIFACE_KEY_NAME);
  count = htable_get_keys(ifaces, &keys);
  struct netiface_info_s info;
  /* list all ifaces */
  for(i = 0; i < count; i++) {
    netiface_t iface = netiface_list_get(ifaces, keys[i]);
    netiface_read(iface, &info);
    netiface_print(stdout, &info, 0);
  }
  /* change mac + ip for eth0 */
  //netiface_t iface = netiface_list_get(ifaces, "eth0");
  //netiface_read(iface, &info);
  //strcpy(info.mac, "48:5b:39:4c:cb:43");
  //strcpy(info.mac, "48:5b:39:cc:ca:42");
  //strcpy(info.ip4, "10.101.1.5");
  //netiface_write(iface, &info);
  //netiface_print(stdout, &info, 0);
  /* clear all */
  netiface_list_delete(ifaces);
  log_close();
  return 0;
}

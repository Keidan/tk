#include <tk/io/net/proto/arp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/syssig.h>
#include <unistd.h>


int main(int argc, char** argv) {
  syssig_init(log_init_cast_user("test_arp", LOG_PID|LOG_CONS|LOG_PERROR), NULL);
  int fd;
  htable_t ifaces = netiface_list_new(NETIFACE_LVL_RAW, NETIFACE_KEY_NAME);
  struct netiface_info_s info;
  netiface_t iface = netiface_list_get(ifaces, "wlan0");
  netiface_read(iface, &info);

  netiface_get_fd(iface, &fd);
  netiface_mac_t mac;
  //int ret = arp_resolve_ip(DEF_ARP_DBG, info, "192.168.1.1", &mac);
  int ret = arp_resolve_ip(DEF_ARP_DBG, info, "192.168.43.1", &mac);
  printf("Ret: %d, mac: '%s'\n", ret, mac);

    /* clear all */
  netiface_list_delete(ifaces);
  log_close();
  return 0;
}

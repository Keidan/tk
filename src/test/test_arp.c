#include <tk/io/net/proto/arp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/syssig.h>
#include <unistd.h>


int main(int argc, char** argv) {
  syssig_init(log_init_cast_user("test_arp", LOG_PID|LOG_CONS|LOG_PERROR), NULL);
  
  /* if(!arp_find_from_table("192.168.1.1", &entry)) { */
  /*   printf("ARP table\nIP: %s\nMAC: %s\nFlags:", entry.ip, entry.mac); */
  /*   if(entry.flags & ATE_PERM) printf(" PERM"); */
  /*   if(entry.flags & ATE_PUBL) printf(" PUBLISHED"); */
  /*   if(entry.flags & ATE_USETRAILERS) printf(" TRAILERS"); */
  /*   if(entry.flags & ATE_PROXY) printf(" PROXY"); */
  /*   printf("\n"); */
  /* } */

  /* arp_add_in_table("wlan0", "192.168.1.18", "74:e5:43:5a:ea:ff"); */

  htable_t ifaces = netiface_list_new(NETIFACE_LVL_UDP, NETIFACE_KEY_NAME);
  struct netiface_info_s info;
  netiface_t iface = netiface_list_get(ifaces, "wlan0");
  netiface_read(iface, &info);
  netiface_list_delete(ifaces);
  netiface_mac_t mac;
  int ret = arp_resolve_ip(DEF_ARP_DBG, info, "192.168.1.1", &mac);
  //int ret = arp_resolve_ip(DEF_ARP_DBG, info, "192.168.43.1", &mac);
  printf("Ret: %d, mac: '%s'\n", ret, mac);

  log_close();
  return 0;
}

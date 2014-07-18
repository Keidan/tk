#include <tk/io/net/proto/arp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/syssig.h>
#include <unistd.h>


int main(int argc, char** argv) {
  syssig_init(log_init_cast_user("test_arp", LOG_PID|LOG_CONS|LOG_PERROR), NULL);
  struct arp_entry_s entry;
  if(!arp_find_from_table("192.168.43.1", &entry)) {
    printf("ARP table\nIP: %s\nMAC: %s\nFlags:", entry.ip, entry.mac);
    if(arp_entry_is_inuse(&entry)) printf(" INUSE");
    if(arp_entry_is_permanent(&entry)) printf(" PERM");
    if(arp_entry_is_published(&entry)) printf(" PUBLISHED");
    if(arp_entry_is_trailers(&entry)) printf(" TRAILERS");
    if(arp_entry_is_proxy(&entry)) printf(" PROXY");
    printf("\n");
  }

  /* arp_add_in_table("wlan0", "192.168.1.18", "74:e5:43:5a:ea:ff"); */
  struct netiface_info_s info;
  netiface_get_info_by_name("wlan0", &info);
  netiface_mac_t mac;
  int ret = arp_resolve_ip(DEF_ARP_DBG, info, "192.168.43.1", &mac);
  //int ret = arp_resolve_ip(DEF_ARP_DBG, info, "192.168.43.1", &mac);
  printf("Ret: %d, mac: '%s'\n", ret, mac);

  log_close();
  return 0;
}

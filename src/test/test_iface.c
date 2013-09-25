#include <tk/io/net/netiface.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/syssig.h>
#include <unistd.h>


static void print_iface(struct netiface_info_s info) {
  printf("%s\t", info.name);
  printf("Link encap: ");
  switch(info.family) {
    case ARPHRD_NETROM:             printf("NETROM ");             break;
    case ARPHRD_ETHER:              printf("ETHER ");              break;
    case ARPHRD_EETHER:             printf("EETHER ");             break;
    case ARPHRD_AX25:               printf("AX25 ");               break;
    case ARPHRD_PRONET:             printf("PRONET ");             break;
    case ARPHRD_CHAOS:              printf("CHAOS ");              break;
    case ARPHRD_IEEE802:            printf("IEEE802 ");            break;
    case ARPHRD_ARCNET:             printf("ARCNET ");             break;
    case ARPHRD_APPLETLK:           printf("APPLETLK ");           break;
    case ARPHRD_DLCI:               printf("DLCI ");               break;
    case ARPHRD_ATM:                printf("ATM ");                break;
    case ARPHRD_METRICOM:           printf("METRICOM ");           break;
    case ARPHRD_IEEE1394:           printf("IEEE1394 ");           break;
    case ARPHRD_EUI64:              printf("EUI64 ");              break;
    case ARPHRD_INFINIBAND:         printf("INFINIBAND ");         break;
    case ARPHRD_SLIP:               printf("SLIP ");               break;
    case ARPHRD_CSLIP:              printf("CSLIP ");              break;
    case ARPHRD_SLIP6:              printf("SLIP6 ");              break;
    case ARPHRD_CSLIP6:             printf("CSLIP6 ");             break;
    case ARPHRD_RSRVD:              printf("RSRVD ");              break;
    case ARPHRD_ADAPT:              printf("ADAPT ");              break;
    case ARPHRD_ROSE:               printf("ROSE ");               break;
    case ARPHRD_X25:                printf("X25 ");                break;
    case ARPHRD_HWX25:              printf("HWX25 ");              break;
    case ARPHRD_PPP:                printf("PPP ");                break;
    case ARPHRD_CISCO:              printf("CISCO ");              break;
    case ARPHRD_LAPB:               printf("LAPB ");               break;
    case ARPHRD_DDCMP:              printf("DDCMP ");              break;
    case ARPHRD_RAWHDLC:            printf("RAWHDLC ");            break;
    case ARPHRD_TUNNEL:             printf("TUNNEL ");             break;
    case ARPHRD_TUNNEL6:            printf("TUNNEL6 ");            break;
    case ARPHRD_FRAD:               printf("FRAD ");               break;
    case ARPHRD_SKIP:               printf("SKIP ");               break;
    case ARPHRD_LOOPBACK:           printf("LOOPBACK ");           break;
    case ARPHRD_LOCALTLK:           printf("LOCALTLK ");           break;
    case ARPHRD_FDDI:               printf("FDDI ");               break;
    case ARPHRD_BIF:                printf("BIF ");                break;
    case ARPHRD_SIT:                printf("SIT ");                break;
    case ARPHRD_IPDDP:              printf("IPDDP ");              break;
    case ARPHRD_IPGRE:              printf("IPGRE ");              break;
    case ARPHRD_PIMREG:             printf("PIMREG ");             break;
    case ARPHRD_HIPPI:              printf("HIPPI ");              break;
    case ARPHRD_ASH:                printf("ASH ");                break;
    case ARPHRD_ECONET:             printf("ECONET ");             break;
    case ARPHRD_IRDA:               printf("IRDA ");               break;
    case ARPHRD_FCPP:               printf("FCPP ");               break;
    case ARPHRD_FCAL:               printf("FCAL ");               break;
    case ARPHRD_FCPL:               printf("FCPL ");               break;
    case ARPHRD_FCFABRIC:           printf("FCFABRIC ");           break;
    case ARPHRD_IEEE802_TR:         printf("IEEE802_TR ");         break;
    case ARPHRD_IEEE80211:          printf("IEEE80211 ");          break;
    case ARPHRD_IEEE80211_PRISM:    printf("IEEE80211_PRISM ");    break;
    case ARPHRD_IEEE80211_RADIOTAP: printf("IEEE80211_RADIOTAP "); break;
    case ARPHRD_VOID:               printf("VOID ");               break;
    case ARPHRD_NONE:
    default:
                                    printf("NONE ");               break;
      break;
  }
  if(!IFACE_IS_LOOPBACK(info.flags))
    printf("HWaddr %s", info.mac);
  printf("\n");
  if(IFACE_IS_UP(info.flags)) {
    if(strlen(info.ip4) || strlen(info.bcast) || strlen(info.mask)) {
      printf("\t");
      if(strlen(info.ip4))
	printf("inet adr:%s ", info.ip4);
      if(strlen(info.bcast))
	printf("Bcast:%s ", info.bcast);
      if(strlen(info.mask))
	printf("Mask:%s ", info.mask);
      printf("\n");
    }
  }
  printf("\t");
  printf("%s", IFACE_IS_UP(info.flags) ? "UP " : "DOWN "); 
  if(IFACE_IS_LOOPBACK(info.flags))    printf("LOOPBACK ");
  if(IFACE_IS_BROADCAST(info.flags))   printf("BROADCAST ");
  if(IFACE_IS_RUNNING(info.flags))     printf("RUNNING ");
  if(IFACE_IS_MULTICAST(info.flags))   printf("MULTICAST ");
  if(IFACE_IS_PROMISC(info.flags))     printf("PROMISC ");
  if(IFACE_IS_NOTRAILERS(info.flags))  printf("NOTRAILERS ");
  if(IFACE_IS_DEBUG(info.flags))       printf("DEBUG ");
  if(IFACE_IS_MASTER(info.flags))      printf("MASTER ");
  if(IFACE_IS_SLAVE(info.flags))       printf("SLAVE ");
  if(IFACE_IS_PORTSEL(info.flags))     printf("PORTSEL ");
  if(IFACE_IS_AUTOMEDIA(info.flags))   printf("AUTOMEDIA ");
  if(IFACE_IS_DYNAMIC(info.flags))     printf("DYNAMIC ");
  if(IFACE_IS_POINTOPOINT(info.flags)) printf("POINTOPOINT ");
  if(IFACE_IS_NOARP(info.flags))       printf("NOARP ");
  printf(" MTU:%d ", info.mtu);
  printf(" Metric:%d ", info.metric);
  printf("\n\n");
}


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
    print_iface(info);
  }
  netiface_list_delete(ifaces);
  log_close();
  return 0;
}

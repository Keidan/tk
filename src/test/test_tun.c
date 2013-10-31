#include <tk/io/net/nettun.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/syssig.h>
#include <unistd.h>


int main(int argc, char** argv) {
  syssig_init(log_init_cast_user("test_tune", LOG_PID|LOG_CONS|LOG_PERROR), NULL);
  
  struct nettun_s tun;
  bzero(&tun, sizeof(struct nettun_s));
  printf("Add the tun device\n");
  strcpy(tun.name, "vpn");
  strcpy(tun.owner, "root");
  strcpy(tun.group, "kei");
  tun.type = NETTUN_TUN;
  nettun_create(&tun);
  sleep(10);
  printf("Remove the tun device\n");
  nettun_remove(&tun);
  log_close();
  return 0;
}

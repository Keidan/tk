#include <tk/io/net/netroute.h>
#include <tk/io/net/nettools.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/syssig.h>
#include <unistd.h>
#include <tk/utils/llist_iter.h>

static void rls() {
  llist_t list = netroute_ls();
  llist_iter_t it = llist_iter_alloc(list);
  fprintf(stdout, "Destination\tGateway\tInterface\tSource\n");
  while(llist_iter_has_more(it)) {
    struct netroute_item_ls_s *i = (struct netroute_item_ls_s*)llist_value(llist_iter_next(it));
    printf("%s\t%s\t%s\t%s\n", i->dst, i->gateway, i->iface, i->src);
  }
  llist_iter_free(it);
  llist_clear(&list);
}

int main(int argc, char** argv) {
  syssig_init(log_init_cast_user("test_route", LOG_PID|LOG_CONS|LOG_PERROR), NULL);
  rls();
  struct netroute_item_add_s a;
  bzero(&a, sizeof(struct netroute_item_add_s));
  strcpy(a.dst, "192.168.0.32");
  strcpy(a.gateway, "192.168.0.1");
  netroute_add(&a);
  rls();
  netroute_del(&a);
  rls();
  log_close();
  return 0;
}

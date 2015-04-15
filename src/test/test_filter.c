#include <tk/io/net/netfilter.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/syssig.h>
#include <unistd.h>
#include <tk/utils/llist_iter.h>

int main(int argc, char** argv) {
  syssig_init(log_init_cast_user("test_filter", LOG_PID|LOG_CONS|LOG_PERROR), NULL);

  netfilter_t nf = netfilter_new(NETFILTER_TABLE);
  printf("Forward chain exist: %d\n", netfilter_exists(nf, "FORWARD"));
  printf("Flush all forward: %d\n", netfilter_clear(nf, "FORWARD"));
  printf("Flush all input: %d\n", netfilter_clear(nf, "INPUT"));

  struct netfilter_rule_s rule;
  bzero(&rule, sizeof(struct netfilter_rule_s));
  strcpy(rule.src.str.ip, "192.168.1.1");
  //strcpy(rule.src.str.mask, "255.255.255.0");
  rule.src.port.min = 5060;
  rule.src.port.max = 5065;
  strcpy(rule.ifaces.input, "eth0");
  strcpy(rule.target,"DROP");
  strcpy(rule.chain, "INPUT");
  printf("Add rule: %d\n", netfilter_add(nf, &rule));
  printf("Commit: %d\n", netfilter_commit(nf));


  struct netfilter_rule_s rule2;
  bzero(&rule2, sizeof(struct netfilter_rule_s));
  strcpy(rule2.src.str.ip, "192.0.0.0/8");
  rule2.src.port.min = 80;
  rule2.proto = NETFILTER_TCP;
  strcpy(rule2.ifaces.input, "eth0");
  strcpy(rule2.target,"DROP");
  strcpy(rule2.chain, "INPUT");
  printf("Add rule: %d\n", netfilter_add(nf, &rule2));
  printf("Commit: %d\n", netfilter_commit(nf));

  llist_t list = netfilter_ls(nf, "INPUT");
  llist_iter_t it = llist_iter_alloc(list);
  while(llist_iter_has_more(it)) {
    struct netfilter_rule_ls_s *ls = (struct netfilter_rule_ls_s*)llist_value(llist_iter_next(it));
    printf("Src: %s\n", ls->src);
    printf("Dst: %s\n", ls->dst);
    printf("Pro: %d\n", ls->proto);
    printf("Cha: %s\n", ls->chain);
    printf("Tar: %s\n", ls->target);
  }
  llist_iter_free(it);
  llist_clear(&list);
  printf("Remove rule: %d\n", netfilter_remove(nf, &rule));
  printf("Commit: %d\n", netfilter_commit(nf));
  netfilter_delete(nf);
  log_close();
  return 0;
}

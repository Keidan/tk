/**
*******************************************************************************
* @file netfilter.c
* @author Keidan
* @date 11/10/2013
* @par Project
* tk
*
* @par Copyright
* Copyright 2011-2013 Keidan, all right reserved
*
* This software is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY.
*
* Licence summary : 
*    You can modify and redistribute the sources code and binaries.
*    You can send me the bug-fix
*
* Term of the licence in in the file licence.txt.
*
*******************************************************************************
*/
#include <tk/sys/probe.h>
#include <tk/io/net/nettools.h>
#include <tk/io/net/netfilter.h>
#include <tk/utils/string.h>
#include <tk/sys/log.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

/************ Macros */
/* Here begins some of the code taken from iptables-save.c **************** */
#ifndef IP_PARTS_NATIVE
  #define IP_PARTS_NATIVE(n)			\
    (unsigned int)((n)>>24)&0xFF,		\
    (unsigned int)((n)>>16)&0xFF,		\
    (unsigned int)((n)>>8)&0xFF,		\
    (unsigned int)((n)&0xFF)
#endif
#ifndef IP_PARTS
  #define IP_PARTS(n) IP_PARTS_NATIVE(ntohl(n))
#endif
#ifndef IPT_ALIGN
  #define IPT_ALIGN(s) (((s) + (__alignof__(struct ipt_entry)-1)) & ~(__alignof__(struct ipt_entry)-1))
#endif
#define create_ptr(local, param) struct netfilter_s *local = (struct netfilter_s*)param
#define test_ptr(ptr) (ptr && ptr->magic == NETFILTER_MAGIC)
/*************/
/************ Defines */
#define NETFILTER_MAGIC 0xC007BACE
#define KMOD_IPV4_NAME "ip_tables"
//#define KMOD_IPV6_NAME "ip6_tables"
/*************/
/************ Structures */
struct netfilter_s {
    int magic;
    ipt_tablelabel table;
    struct iptc_handle *h;
};
/*************/
/************Functions */
/**
 * @fn static void netfilter_int2ip(__u32 ip, netiface_ip4_t i)
 * @brief Convert an int to an ip4.
 * @param ip The int to convert.
 * @param i The converted ipv4.
 */
static void netfilter_int2ip(__u32 ip, netiface_ip4_t i);
/**
 * @fn static void netiface_prepare_lrule(const struct ipt_entry *e, struct iptc_handle *h, const char *chain, struct netfilter_rule_ls_s *lrule)
 * @brief Prepare the listed rule entries.
 * @param e Current IPT entry.
 * @param h IPT handle.
 * @param chain The current chain.
 * @param lrule The output rule.
 */
static void netiface_prepare_lrule(const struct ipt_entry *e, struct iptc_handle *h, const char *chain, struct netfilter_rule_ls_s *lrule);
/**
 * @fn static int netfilter_add_to_kernel(struct iptc_handle *h, struct ipt_entry* entry, const ipt_chainlabel chainlabel)
 * @brief adds an ipt_entry to the specified chain - finally ;-)
 * @param h IPTC handle.
 * @param entry the entry which should be adedded
 * @param chainlabel the name of the chain from which the entry shall be deleted
 * @param table the handle for the table from which the entry shall be deleted
 * @return -1 on error else 0 on sucess.
 */
static int netfilter_add_to_kernel(struct iptc_handle *h, struct ipt_entry* entry, const ipt_chainlabel chainlabel);
/**
 * @fn static int netfilter_remove_from_kernel(struct iptc_handle *h, struct ipt_entry* entry, unsigned char *matchmask, const ipt_chainlabel chainLabel)
 * @brief Deletes an ipt_entry from the specified chain
 * @param h IPTC handle.
 * @param entry the entry which should be deleted
 * @param matchmask libiptc needs a mask telling what parts of an entry to consider when searching for the entry which shall bel deleted.
 * @param chainlabel the name of the chain from which the entry shall be deleted
 * @param table the handle for the table from which the entry shall be deleted
 * @return -1 on error else 0 on sucess.
 */
static int netfilter_remove_from_kernel(struct iptc_handle *h, struct ipt_entry* entry, unsigned char *matchmask, const ipt_chainlabel chainlabel);
/**
 * @fn static void netfilter_parse_ports(__u16 from[2],  __u16 to[2])
 * @brief Compares two ports and changes their order if higher value is
 * in the first array element
 * @param from array containing the ports to check
 * @param to array containing the (swapped) ports
 */
static void netfilter_parse_ports(__u16 from[2],  __u16 to[2]);
/**
 * @fn static struct ipt_entry* netfilter_build_entry(struct ipt_ip* ip, struct ipt_entry_match* match, struct ipt_entry_target* target, int *entrysize)
 * @brief Builds an ipt_entry consisting of ipt_ip, ipt_match, ipt_entry
 * @param *ip ipt_ip containing IP, Mask and Interfacenames
 * @param *match ipt_match containing match information i.e. ports
 * @param *target ipt_target containing ACTION , i.c. "ACCEPT"
 * @return ipt_entry* on success else NULL
 */
static struct ipt_entry* netfilter_build_entry(struct ipt_ip* ip, struct ipt_entry_match* match, struct ipt_entry_target* target, int *entrysize);
/**
 * @fn int netfilter_prepare_entry(struct netfilter_rule_s *rule, const char* targetlabel, struct ipt_entry** entry)
 * @brief Prepares a rule in order to be added to the Kernel:
 * - creates ipt_entry from ipt_ip, ipt_match, ipt_target with the
 *   contents of the given rule
 *
 * @param rule	rule which shall be added.
 * @param targetlabel The label of the target.
 * @param entry Where to put the created ipt_entry
 * @return length of the entry on success else -1 on error.
 */
int netfilter_prepare_entry(struct netfilter_rule_s *rule, const char* targetlabel, struct ipt_entry** entry);
/**
 * @fn struct ipt_entry_target* netfilter_build_target(const char* action)
 * @brief Builds an ipt_target
 * @param action a valid action i.e "ACCEPT", "DROP", "REJECT"
 * @return  ipt_entry_target* on success else NULL on error.
 */
struct ipt_entry_target* netfilter_build_target(const char* action);

/**
 * @fn static struct ipt_ip* netfilter_build_ip(struct netfilter_rule_s* rule)
 * @brief Creates an ipt_ip and fills it with content from given rule
 * @param rule Rule containing information to parse
 * @return ipt_ip* on success else NULL on error
 */
static struct ipt_ip* netfilter_build_ip(struct netfilter_rule_s* rule);

/**
 * @fn static struct ipt_entry_match* netfilter_build_match(struct netfilter_rule_s* rule)
 * @brief Creates an ipt_entry_match and fills it with content from given rule
 * @param rule Rule containing information to parse
 * @return ipt_entry_match* on success else NULL on error
 **/
static struct ipt_entry_match* netfilter_build_match(struct netfilter_rule_s* rule);
 /**
  * @fn static struct ipt_entry_match* netfilter_build_match_tcp(struct netfilter_rule_s* rule)
  * @brief Prepares the match (for TCP) so that it can be added to an ipt_entry
  * @param rule Rule containing information to parse
  * @return ipt_entry_match* on success else NULL on error
  */
static struct ipt_entry_match* netfilter_build_match_tcp(struct netfilter_rule_s* rule);
 /**
  * @fn static struct ipt_entry_match* netfilter_build_match_udp(struct netfilter_rule_s* rule)
  * @brief Prepares the match (for UDP) so that it can be added to an ipt_entry
  * @param rule Rule containing information to parse
  * @return ipt_entry_match* on success else NULL on error
  */
static struct ipt_entry_match* netfilter_build_match_udp(struct netfilter_rule_s* rule);
/*************/
/************Global fields */
static pthread_mutex_t hard_lock = PTHREAD_MUTEX_INITIALIZER;

/**
 * @fn void netfilter_delete(netfilter_t netf)
 * @brief Delete the netfilter connection.
 * @param netf Netfilter handle.
 */
void netfilter_delete(netfilter_t netf) {
  create_ptr(nf, netf);
  if(!test_ptr(nf)) return;
  pthread_mutex_lock(&hard_lock);
  if(nf->h) iptc_free(nf->h), nf->h = NULL;
  nf->magic = 0;
  free(nf);
  pthread_mutex_unlock(&hard_lock);
}

/**
 * @fn netfilter_t netfilter_new(ipt_tablelabel tablename)
 * @brief Create the netfilter connection.
 * @param tablename Netfilter table name (see NETFILTER_TABLE).
 * @return The Netfilter handle else NULL on error.
 */
netfilter_t netfilter_new(ipt_tablelabel tablename) {
  struct netfilter_s* nf;
  size_t len = 0;
  if(tablename) len = strlen(tablename);
  if(!len) {
    logger(LOG_ERR, "Invalid table name!\n");
    return NULL;
  }
  pthread_mutex_lock(&hard_lock);
  nf = malloc(sizeof(struct netfilter_s));
  if(!nf) {
    pthread_mutex_unlock(&hard_lock);
    logger(LOG_ERR, "Unable to alloc a memory for the netfilter context!\n");
    return NULL;    
  }
  bzero(nf, sizeof(struct netfilter_s));
  
  nf->magic = NETFILTER_MAGIC;
  strcpy(nf->table, tablename);
  nf->h = iptc_init(nf->table);
  if (!nf->h) {
    probe_insert(KMOD_IPV4_NAME, NULL, 0);
    usleep(250000);
    nf->h = iptc_init(nf->table);
    if (!nf->h) {
      probe_insert(KMOD_IPV4_NAME, NULL, 0);
      usleep(500000);
      nf->h = iptc_init(nf->table);
    }
  }
  if (!nf->h) {
    pthread_mutex_unlock(&hard_lock);
    logger(LOG_EMERG, "# ERROR: Initialization of kernel table failed: %s\n", iptc_strerror(errno));
    netfilter_delete(nf);
    return NULL;
  }
  pthread_mutex_unlock(&hard_lock);
  return nf;
}

/**
 * @fn int netfilter_commit(netfilter_t netf)
 * @brief Commit pending changes to the kernel table.
 * @param netf Netfilter handle.
 * @return -1 on error else 0 on success.
 */
int netfilter_commit(netfilter_t netf) {
  create_ptr(nf, netf);
  if(!test_ptr(nf)) return -1;
  pthread_mutex_lock(&hard_lock);
  int ret = 0;
  if(nf->h) ret = iptc_commit(nf->h);
  pthread_mutex_unlock(&hard_lock);
  if(!ret) {
    logger(LOG_ERR, "# ERROR: Couldn't commit changes to kernel table: %s\n", iptc_strerror(errno));
    return -1;
  }
  return 0;
}

/**
 * @fn int netfilter_switch(netfilter_t *netf, ipt_tablelabel tablename)
 * @brief Switch the netfilte table.
 * @param netf The nex connection pointer (only if this function return 0)
 * @param tablename The table
 * @return -1 on error else 0 on success.
 */
int netfilter_switch(netfilter_t *netf, ipt_tablelabel tablename) {
  netfilter_t temp;
  if(!netf) return -1;
  create_ptr(nf, *netf);
  if(!test_ptr(nf)) return -1;
  temp = netfilter_new(tablename);
  if(temp) {
    netfilter_delete(*netf);
    *netf = temp;
    return 0;
  }
  return 0;
}

/**
 * @fn int netfilter_clear(netfilter_t netf, const ipt_chainlabel chainlabel)
 * @brief Removes all entries from a chain
 * @param netf Netfilter handle.
 * @param chainlabel	name of the chain to flush
 * 			NOTE: libiptc.h: typedef char ipt_chainlabel[32]
 * @return -1 on error else 0 on success.
 */
int netfilter_clear(netfilter_t netf, const ipt_chainlabel chainlabel) {
  int ret = 0;
  create_ptr(nf, netf);
  if(!test_ptr(nf)) return -1;
  pthread_mutex_lock(&hard_lock);
  if(nf->h) ret = iptc_flush_entries(chainlabel, nf->h);
  pthread_mutex_unlock(&hard_lock);
  if(!ret) {
    logger(LOG_ERR, "# ERROR: Flushing of chain \"%s\" failed: %s\n", chainlabel, iptc_strerror(errno));
    return -1;
  }
  return 0;
}

/**
 * @fn int netfilter_exists(netfilter_t netf, const ipt_chainlabel chainlabel)
 * @brief Checks whether a chain exists
 * @param netf The netfilter handle.
 * @param chainlabel	name of chain to check
 * 			NOTE: libiptc.h: typedef char ipt_chainlabel[32]
 * @return -1 on error, 0 the chain does not exists 1 the chain exists.
 */
int netfilter_exists(netfilter_t netf, const ipt_chainlabel chainlabel) {
  int ret = 1;
  create_ptr(nf, netf);
  if(!test_ptr(nf)) return -1;
  pthread_mutex_lock(&hard_lock);
  if(nf->h) ret = iptc_is_chain(chainlabel, nf->h);
  pthread_mutex_unlock(&hard_lock);
  return ret;
 }

/**
 * @fn const char* netfilter_get_table_name(netfilter_t netf)
 * @brief Get the current table name.
 * @param netf The netfilter handle.
 * @return The name else NULL on error.
 */
const char* netfilter_get_table_name(netfilter_t netf) {
  create_ptr(nf, netf);
  if(!test_ptr(nf)) return NULL;
  return nf->table;
}

/**
 * @fn static void netfilter_parse_ports(__u16 from[2],  __u16 to[2])
 * @brief Compares two ports and changes their order if higher value is
 * in the first array element
 * @param from array containing the ports to check
 * @param to array containing the (swapped) ports
 **/
static void netfilter_parse_ports(__u16 from[2],  __u16 to[2]) {

  if(!from[0]) {
    to[0] = 0;
    // if B not null ? (then) A==C : (else) A==D
    to[1] = from[1] ? from[1] : 0xffff;	
    return;
  }
  to[0] = from[0];
  if(!from[1]) {
    to[1] = to[0];
    return;
  } else if(from[1] < from[0]) {
    to[0] = from[1];
    to[1] = from[0];
    return;
  }
  to[1] = from[1];
}

/**
 * @fn llist_t netfilter_ls(netfilter_t netf, const ipt_chainlabel mchain)
 * @brief List all rules associated to the specified chain.
 * @param netf The netfilter context.
 * @param mchain The chain to list.
 * @return A list of (struct netfilter_rule_ls_s*)
 */
llist_t netfilter_ls(netfilter_t netf, const ipt_chainlabel mchain) {
  const struct ipt_entry *e1;
  const char *chain = NULL;
  struct netfilter_rule_ls_s rule;
  llist_t rules = NULL;
  create_ptr(nf, netf);
  if(!test_ptr(nf)) return NULL;
  if(!mchain || !strlen(mchain)) {
    logger(LOG_ERR, "# ERROR: ChainLabel points to NULL\n");
    return NULL;
  }
  if(!nf->h) {
    logger(LOG_ERR, "# ERROR: Table points to NULL\n");
    return NULL;
  }
  /* print chains and their rules */
  for (chain = iptc_first_chain(nf->h); chain; chain = iptc_next_chain(nf->h)) {
    if(strcmp(chain, mchain)) continue;
    for (e1 = iptc_first_rule(chain, nf->h); e1; e1 = iptc_next_rule(e1, nf->h))  {
      netiface_prepare_lrule(e1, nf->h, chain, &rule);
      rules = llist_pushback_and_alloc(rules, &rule, sizeof(struct netfilter_rule_ls_s));
    }
  }
  return rules;
}

/**
 * @fn static void netiface_prepare_lrule(const struct ipt_entry *e, struct iptc_handle *h, const char *chain, struct netfilter_rule_ls_s *lrule)
 * @brief Prepare the listed rule entries.
 * @param e Current IPT entry.
 * @param h IPT handle.
 * @param chain The current chain.
 * @param lrule The output rule.
 */
static void netiface_prepare_lrule(const struct ipt_entry *e, struct iptc_handle *h, const char *chain, struct netfilter_rule_ls_s *lrule) {
  const char *target_name;
  bzero(lrule, sizeof(struct netfilter_rule_ls_s));
  /* print chain name */
  strcpy(lrule->chain, chain);

  /* Print IP part. */
  netfilter_int2ip(e->ip.src.s_addr, lrule->src);
  netfilter_int2ip(e->ip.dst.s_addr, lrule->dst);
  lrule->proto = e->ip.proto;
  /* Print target name */
  target_name = iptc_get_target(e, h);
  if (target_name && (*target_name != '\0'))
    strcpy(lrule->target, target_name);
}


/**
 * @fn static int netfilter_add_to_kernel(struct iptc_handle *h, struct ipt_entry* entry, const ipt_chainlabel chainlabel)
 * @brief adds an ipt_entry to the specified chain - finally ;-)
 * @param h IPTC handle.
 * @param entry the entry which should be adedded
 * @param chainlabel the name of the chain from which the entry shall be deleted
 * @param table the handle for the table from which the entry shall be deleted
 * @return -1 on error else 0 on sucess.
 */
static int netfilter_add_to_kernel(struct iptc_handle *h, struct ipt_entry* entry, const ipt_chainlabel chainlabel) {
  int ret;
  pthread_mutex_lock(&hard_lock);
  ret = iptc_append_entry(chainlabel, entry, h);
  pthread_mutex_unlock(&hard_lock);
  if (ret == 1)
    return 0;
  logger(LOG_ERR, "# ERROR: Adding of entry to table failed!\n");
  return -1;
}
/**
 * @fn static int netfilter_remove_from_kernel(struct iptc_handle *h, struct ipt_entry* entry, unsigned char *matchmask, const ipt_chainlabel chainLabel)
 * @brief Deletes an ipt_entry from the specified chain
 * @param h IPTC handle.
 * @param entry the entry which should be deleted
 * @param matchmask libiptc needs a mask telling what parts of an entry to consider when searching for the entry which shall bel deleted.
 * @param chainlabel the name of the chain from which the entry shall be deleted
 * @param table the handle for the table from which the entry shall be deleted
 * @return -1 on error else 0 on sucess.
 */
static int netfilter_remove_from_kernel(struct iptc_handle *h, struct ipt_entry* entry, unsigned char *matchmask, const ipt_chainlabel chainlabel) {
  int ret;
  pthread_mutex_lock(&hard_lock);
  ret = iptc_delete_entry(chainlabel, entry, matchmask, h);
  pthread_mutex_unlock(&hard_lock);
  if (ret == 1)
    return 0;
  logger(LOG_ERR, "# ERROR: Removing of entry to table failed!\n");
  return -1;
}

/**
 * @fn static void netfilter_int2ip(__u32 ip, netiface_ip4_t i)
 * @brief Convert an int to an ip4.
 * @param ip The int to convert.
 * @param i The converted ipv4.
 */
static void netfilter_int2ip(__u32 ip, netiface_ip4_t i) {
  if (!ip) {
    strcpy(i, "???.???.???.???");
    return;
  }
  bzero(i, sizeof(netiface_ip4_t));
  sprintf(i, "%u.%u.%u.%u", IP_PARTS(ip));
}

/**
 * @fn int netfilter_remove(netfilter_t netf, struct netfilter_rule_s* rule)
 * @brief Prepares a rule in order to be deleted from the Kernel:
 * - does basic sanity checks
 * - calls prepareEntry in order to get an ipt_entry
 * - removes the ipt_entry to kernel
 * @param netf Netfilter handle.
 * @param rule rule which shall be removed
 * @return -1 on error else 0 on success
 */
int netfilter_remove(netfilter_t netf, struct netfilter_rule_s *rule) {
  int ret = 0;
  int esize = 0;
  unsigned char *matchmask;
  struct ipt_entry *entry;		/* pointer to entry */
  create_ptr(nf, netf);
  if(!test_ptr(nf)) return -1;

  /* Sanity checks: rule, chainLabel and table */
  if(!rule) {
    logger(LOG_ERR, "# ERROR: Rule points to NULL\n");
    return -1;
  }
  if(!rule->chain) {
    logger(LOG_ERR, "# ERROR: ChainLabel points to NULL\n");
    return -1;
  }
  if(!nf->h) {
    logger(LOG_ERR, "# ERROR: Table points to NULL\n");
    return -1;
  }
  if((ret = netfilter_prepare_entry(rule, rule->target, &entry)) < 0) {
    logger(LOG_ERR, "# ERROR: Couldn't create ipt_entry\n");
    return -1;
  }

  esize = ret;
  /* libiptc needs a mask telling what parts of an entry
   * to consider when searching for the entry which shall bel deleted.
   *
   * For an exact match a unsigned char* set to 0xFF with the length of the
   * entry is needed.
   * What a coincidence that prepareEntry returnes the length value ;-) */
  
  if((matchmask = ((unsigned char *)malloc(esize))) == NULL) {
    free(entry);
    logger(LOG_ERR, "# ERROR: Couldn't allocate memory for matchmask!\n");
    return -1;
  }

  /* set all bits of the matchmask to '1' */
  memset(matchmask, 0xFF, esize);

  /* ready to remove the rule from the kernel */
  ret = netfilter_remove_from_kernel(nf->h, entry, matchmask, rule->chain);

  /* cleanups */
  free(entry);
  free(matchmask);
  return ret;
}


/**
 * @fn int netfilter_add(netfilter_t netf, struct netfilter_rule_s* rule)
 * @bief Prepares a rule in order to be added to the Kernel:
 * - does basic sanity checks
 * - calls prepareEntry in order to get an ipt_entry
 * - adds the ipt_entry to kernel
 * @param netf Ntfilter handle.
 * @param rule rule which shall be added
 * @return -1 on error else 0 on success
 **/
int netfilter_add(netfilter_t netf, struct netfilter_rule_s* rule) {
  int ret = 0;
  struct ipt_entry *entry;		/* pointer to entry */
  create_ptr(nf, netf);
  if(!test_ptr(nf)) return -1;

  /* Sanity checks: rule, chainLabel, targetLabel and table */
  if(!rule) {
    logger(LOG_ERR, "# ERROR: Rule points to NULL\n");
    return -1;
  }
  if(!rule->chain) {
    logger(LOG_ERR, "# ERROR: ChainLabel points to NULL\n");
    return -1;
  }
  if(!rule->target) {
    logger(LOG_ERR, "# ERROR: TargetLabel points to NULL\n");
    return -1;
  }
  if(!nf->h) {
    logger(LOG_ERR, "# ERROR: Table points to NULL\n");
    return -1;
  }
  
  if((ret = netfilter_prepare_entry(rule, rule->target, &entry)) < 0) {
    logger(LOG_ERR, "# ERROR: Couldn't create ipt_entry\n");
    return false;
  }

  /* ready to add the rule to the kernel */
  ret = netfilter_add_to_kernel(nf->h, entry, rule->chain);
  /* cleanups */
  free(entry);
  return ret;
}

/**
 * @fn static struct ipt_entry* netfilter_build_entry(struct ipt_ip* ip, struct ipt_entry_match* match, struct ipt_entry_target* target, int *entrysize)
 * @brief Builds an ipt_entry consisting of ipt_ip, ipt_match, ipt_entry
 * @param ip ipt_ip containing IP, Mask and Interfacenames
 * @param match ipt_match containing match information i.e. ports
 * @param target ipt_target containing ACTION , i.c. "ACCEPT"
 * @return ipt_entry* on success else NULL
 */
static struct ipt_entry* netfilter_build_entry(struct ipt_ip* ip, struct ipt_entry_match* match, struct ipt_entry_target* target, int *entrysize) {
  struct ipt_entry *entry;
  int tsize = 0, esize = 0, msize = 0;

  /* some basic sanity checks */
  if(target == NULL) {
    logger(LOG_ERR, "# ERROR: BuildEntry was called with NULL target\n");
    if(match != NULL)
      free(match), match = NULL;
    if(ip != NULL)
      free(ip), ip = NULL;
    return NULL;
  }

  if(ip == NULL) {
    logger(LOG_ERR, "# ERROR: BuildEntry was called with NULL ipt_ip\n");
    free(target);
    if(match != NULL)
      free(match), match = NULL;
    return NULL;
  }

  /* get size of target */
  tsize = target->u.target_size;

  /* get size of match */
  if(match != NULL)
    msize = match->u.match_size;
	
  /* size of an ipt_entry computes as sum of size of entry itself + match(es) + target */
  esize = IPT_ALIGN(sizeof(*entry) + msize + tsize);
  /* return size in entrysize - delete function will be happy to know that value */
  *entrysize = esize;

  if((entry = ((struct ipt_entry *)malloc(esize))) == NULL) {
    logger(LOG_ERR, "# ERROR: Couldn't allocate memory for ipt_entry\n");
    free(target);
    if(match != NULL)
      free(match), match = NULL;
    free(ip), ip = NULL;
    return NULL;
  }

  /* clear memory of entry & fill in offsets */
  memset(entry, 0, esize);
  entry->next_offset = esize;
  entry->target_offset = esize - tsize;

  /* now we concatenate the ipt_entry with target and match.
   * Very ugly, in deed!
   */

  if (match != NULL)
    memcpy(entry->elems, match, msize);

  memcpy(entry->elems + msize, target, tsize);
  memcpy(&(entry->ip), ip, sizeof(*ip));

  /* free used memory of ip, match and target */
  if(match != NULL)
    free(match), match = NULL;
  free(ip), ip = NULL;
  free(target);
  return entry;
}

/**
 * @fn int netfilter_prepare_entry(struct netfilter_rule_s *rule, const char* targetlabel, struct ipt_entry** entry)
 * @brief Prepares a rule in order to be added to the Kernel:
 * - creates ipt_entry from ipt_ip, ipt_match, ipt_target with the
 *   contents of the given rule
 *
 * @param rule	rule which shall be added.
 * @param argetlabel The label of the target.
 * @param *entry Where to put the created ipt_entry
 * @return length of the entry on success else -1 on error.
 */
int netfilter_prepare_entry(struct netfilter_rule_s *rule, const char* targetlabel, struct ipt_entry** entry) {
  /* *** Step 1/5 - Initialization *** */
  int entrysize = 0;
  struct ipt_ip *ip;			/* pointer to ipt_ip */
  struct ipt_entry_target *target;	/* pointer to target */
  struct ipt_entry_match *match;	/* pointer to match */
  
  /* *** Step 2/5 - create ipt_ip and fill it with content *** */
  if ((ip = netfilter_build_ip(rule)) == NULL) {
    logger(LOG_ERR, "# ERROR: Couldn't create ipt_entry!\n" );
    return -1;
  }

  /* *** Step 3/5 - create ipt_target and fill it with content *** */
  if((target = netfilter_build_target(targetlabel)) == NULL) {
    if(ip) free(ip);
    logger(LOG_ERR, "# ERROR: Couldn't create target!\n");
    return -1;
  }

  /* *** Step 4/5 - create ipt_match and fill it with content *** */
  match = netfilter_build_match(rule);

  /* *** Step 5/5 - create ipt_entry and add it to the table *** */
  if ((*entry = netfilter_build_entry(ip, match, target, &entrysize)) == NULL) {
    if(ip) free(ip);
    if(target) free(target);
    if(match) free(match);
    logger(LOG_ERR, "# ERROR: Couldn't create ipt_entry\n");
    return -1;
  }

  return entrysize;
}

/**
 * @fn struct ipt_entry_target* netfilter_build_target(const char* action)
 * @brief Builds an ipt_target
 * @param action a valid action i.e "ACCEPT", "DROP", "REJECT"
 * @return  ipt_entry_target* on success else NULL on error.
 */
struct ipt_entry_target* netfilter_build_target(const char* action) {
  int tsize;
  struct ipt_entry_target *target;

  /* ipt_entry has to be aligned for kernel convenience */
  tsize = IPT_ALIGN ( sizeof ( struct ipt_entry_target ) + 4 );
  if((target = ((struct ipt_entry_target *)malloc ( tsize ))) == NULL) {
    logger(LOG_ERR, "# ERROR: Couldn't allocate memory for ipt_entry_target\n");
    return NULL;
  }
  /* initialize target */
  memset(target, 0, tsize);

  /*set size of target*/
  target->u.target_size = tsize;
  target->u.user.target_size = tsize;

  /* fill in actual target, i.e. "ACCEPT", "DROP", ... */
  strncpy(target->u.user.name, action, XT_FUNCTION_MAXNAMELEN - 1);
  return target;
}


/**
 * @fn static struct ipt_ip* netfilter_build_ip(struct netfilter_rule_s* rule)
 * @brief Creates an ipt_ip and fills it with content from given rule
 * @param rule Rule containing information to parse
 * @return ipt_ip* on success else NULL on error
 */
static struct ipt_ip* netfilter_build_ip(struct netfilter_rule_s* rule) {
  struct ipt_ip *ip = NULL;
  int idx;
  netiface_ip4_t sip;
  if((ip = ((struct ipt_ip *)malloc (sizeof (*ip)))) == NULL) {
    logger(LOG_ERR, "# ERROR: Couldn't allocate memory for ipt_ip\n");
    return NULL;
  }

  /* fill ipt_ip ip with content */
  memset(ip, 0, sizeof(*ip));//null the structure
  idx = string_indexof(rule->src.str.ip, "/");
  if(idx == -1) {
    if(rule->src.str.ip != 0 && strlen(rule->src.str.ip))
      nettools_ip_to_inaddr(rule->src.str.ip, &ip->src);
    if(rule->src.str.mask != 0 && strlen(rule->src.str.mask))
      nettools_ip_to_inaddr(rule->src.str.mask, &ip->smsk);
    else {
      idx = nettools_get_cidr(rule->src.str.ip);
      nettools_ip_to_inaddr(nettools_get_mask_by_cidr(idx), &ip->smsk);
    }
  } else {
    strncpy(sip, rule->src.str.ip, idx);
    nettools_ip_to_inaddr(sip, &ip->src);
    nettools_ip_to_inaddr(nettools_get_mask_by_cidr(string_parse_int(rule->src.str.ip + idx + 1, 32)), &ip->smsk);
  }

  idx = string_indexof(rule->dst.str.ip, "/");
  if(idx == -1) {
    if(rule->dst.str.ip != 0 && strlen(rule->dst.str.ip))
      nettools_ip_to_inaddr(rule->dst.str.ip, &ip->dst);
    if(rule->dst.str.mask != 0 && strlen(rule->dst.str.mask))
      nettools_ip_to_inaddr(rule->dst.str.mask, &ip->dmsk);
    else {
      idx = nettools_get_cidr(rule->dst.str.ip);
      nettools_ip_to_inaddr(nettools_get_mask_by_cidr(idx), &ip->dmsk);
    }
  } else {
    strncpy(sip, rule->dst.str.ip, idx);
    nettools_ip_to_inaddr(sip, &ip->dst);
    nettools_ip_to_inaddr(nettools_get_mask_by_cidr(string_parse_int(rule->dst.str.ip + idx + 1, 32)), &ip->dmsk);
  }
  ip->proto = rule->proto;

  if(rule->ifaces.input && strlen(rule->ifaces.input))
    strncpy(ip->iniface, rule->ifaces.input, IFNAMSIZ);	//incoming interfacename
  if(rule->ifaces.output && strlen(rule->ifaces.output))
    strncpy(ip->outiface, rule->ifaces.output, IFNAMSIZ);	//outgoing interfacename
  return ip;
}


/**
 * @fn static struct ipt_entry_match* netfilter_build_match(struct netfilter_rule_s* rule)
 * @brief Creates an ipt_entry_match and fills it with content from given rule
 * @param rule Rule containing information to parse
 * @return ipt_entry_match* on success else NULL on error
 **/
static struct ipt_entry_match* netfilter_build_match(struct netfilter_rule_s* rule) {
  struct ipt_entry_match *match = NULL;

  /* matches are protocol specific extensions, so we split up from here
   * in protocol specific functions, each handling the corresponding transport protocol
   */
  switch(rule->proto) {
    case IPPROTO_TCP: {
      /* we only need to create a match, if a port is specified */
      if(rule->src.port.min || rule->src.port.max || rule->dst.port.max || rule->dst.port.max)
	if((match = netfilter_build_match_tcp(rule)) == NULL)
	  logger(LOG_ERR, "# ERROR: Couldn't create tcp match\n");
      break;
    }
    case IPPROTO_UDP: {
      /* we only need to create a match, if a port is specified */
      if(rule->src.port.min || rule->src.port.max || rule->dst.port.min || rule->dst.port.max)
	if((match = netfilter_build_match_udp(rule)) == NULL)
	  logger(LOG_ERR, "# ERROR: Couldn't create tcp match\n");
      
      break;
    }
      
    default: {
      /* match = NULL should be save for every protocol */
      match = NULL;
    }
  }
  return match;
}


 /**
  * @fn static struct ipt_entry_match* netfilter_build_match_tcp(struct netfilter_rule_s* rule)
  * @brief Prepares the match (for TCP) so that it can be added to an ipt_entry
  * @param rule Rule containing information to parse
  * @return ipt_entry_match* on success else NULL on error
  */
static struct ipt_entry_match* netfilter_build_match_tcp(struct netfilter_rule_s* rule) {
  int msize;
  struct ipt_tcp tcp;
  struct ipt_entry_match *match = NULL;
  __u16 ps[2] = {rule->src.port.min, rule->src.port.max};
  __u16 pd[2] = {rule->dst.port.min, rule->dst.port.max};
  /* match has to be aligned for kernel convenience */
  msize = IPT_ALIGN(sizeof(*match) + sizeof(tcp));

  if((match = ((struct ipt_entry_match *)malloc(msize))) == NULL) {
    logger(LOG_ERR, "# ERROR: Couldn't allocate memory for ipt_entry_match\n");
    return NULL;
  }

  /* copy needed information */
  memset(match, 0, msize);
  match->u.match_size = msize;
  strcpy(match->u.user.name, "tcp");

  memset(&tcp, 0, sizeof(tcp));

  /* Change ports if higher portno is in Xpts[0] and lower one in Xpts[1] */
  if(rule->src.port.min != 0) netfilter_parse_ports(ps, tcp.spts);
  else tcp.spts[0] = 0, tcp.spts[1] = 0xffff;
  if(rule->dst.port.min != 0) netfilter_parse_ports(pd, tcp.dpts);
  else tcp.dpts[0] = 0, tcp.dpts[1] = 0xffff;
  /* copy needed information into match */
  memcpy(&match->data, &tcp, sizeof(tcp));
  
  return match;
}

 /**
  * @fn static struct ipt_entry_match* netfilter_build_match_udp(struct netfilter_rule_s* rule)
  * @brief Prepares the match (for IDNP) so that it can be added to an ipt_entry
  * @param rule Rule containing information to parse
  * @return ipt_entry_match* on success else NULL on error
  */
static struct ipt_entry_match* netfilter_build_match_udp(struct netfilter_rule_s* rule) {
  int msize;
  struct ipt_udp udp;
  struct ipt_entry_match *match = NULL;
  __u16 ps[2] = {rule->src.port.min, rule->src.port.max};
  __u16 pd[2] = {rule->dst.port.min, rule->dst.port.max};

  /* match has to be aligned for kernel convenience */
  msize = IPT_ALIGN(sizeof(*match) + sizeof(udp));
  
  if(( match = ((struct ipt_entry_match *)malloc(msize))) == NULL) {
    logger(LOG_ERR, "# ERROR: Couldn't allocate memory for ipt_entry_match\n");
    return NULL;
  }

  /* copy needed information */
  memset(match, 0, msize);
  match->u.match_size = msize;
  strcpy(match->u.user.name, "udp");

  memset(&udp, 0, sizeof(udp));

  /* Change ports if higher portno is in Xpts[0] and lower one in Xpts[1] */
  if(rule->src.port.min != 0) netfilter_parse_ports(ps, udp.spts);
  else udp.spts[0] = 0, udp.spts[1] = 0xffff;
  if(rule->dst.port.min != 0) netfilter_parse_ports(pd, udp.dpts);
  else udp.dpts[0] = 0, udp.dpts[1] = 0xffff;
  /* copy needed information into match*/
  memcpy(&match->data, &udp, sizeof(udp));
  return match;
}

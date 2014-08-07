#include <tk/io/net/netiface_status.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tk/utils/string.h>
#include <tk/utils/stringtoken.h>


/**
 * @fn static long netiface_status_extract_long(stringtoken_t tokens, long def)
 * @brief Extract long value from token.
 * @param tokens The token.
 * @param def The default value.
 * @return the value.
 */
static long netiface_status_extract_long(stringtoken_t tokens, long def);

/**
 * @fn static int netiface_status_extract_int(stringtoken_t tokens, int def)
 * @brief Extract int value from token.
 * @param tokens The token.
 * @param def The default value.
 * @return the value.
 */
static int netiface_status_extract_int(stringtoken_t tokens, int def);

/**
 * @fn int netiface_status_load_wireless(const char* iface_name, struct wireless_s *wireless)
 * @brief Load the wireless informations.
 * @param iface_name The iface name.
 * @param wireless The result informations.
 * @return -1 on error else 0 on success.
 */
int netiface_status_load_wireless(const char* iface_name, struct wireless_s *wireless) {
  memset(wireless, 0, sizeof(struct wireless_s));
  wireless->internal.wireless = 0;
  wireless->internal.missed_beacon = wireless->internal.status = wireless->internal.we21 = -1;
  FILE *f = fopen(PROCFS_NET_WIRELESS, "r");
  if (f) {
    char* line = NULL;
    size_t sz = 0;
    int cpt = 0;
    while (getline(&line, &sz, f) != -1) {
      if((cpt++) <= 1) //skip title lines
	continue;
      stringtoken_t tokens = stringtoken_init(line, ":. ");
      char* devname = stringtoken_next_token(tokens);
      if(!strcmp(devname, iface_name)) {
	wireless->internal.status = netiface_status_extract_int(tokens, 0);
	wireless->quality.link = netiface_status_extract_int(tokens, 0);
	wireless->quality.level = netiface_status_extract_int(tokens, 0);
	wireless->quality.noise = netiface_status_extract_int(tokens, 0);
	wireless->discarded_packets.nwid = netiface_status_extract_int(tokens, 0);
	wireless->discarded_packets.crypt = netiface_status_extract_int(tokens, 0);
	wireless->discarded_packets.frag = netiface_status_extract_int(tokens, 0);
	wireless->discarded_packets.retry = netiface_status_extract_int(tokens, 0);
	wireless->discarded_packets.misc = netiface_status_extract_int(tokens, 0);
	wireless->internal.missed_beacon = netiface_status_extract_int(tokens, 0);
	if(stringtoken_has_more_tokens(tokens))
	  wireless->internal.we21 = netiface_status_extract_int(tokens, 0);
	wireless->internal.wireless = 1;
	stringtoken_release(tokens);
	free(devname);
	break;
      }
      free(devname);
      stringtoken_release(tokens);
    }
    if(line) free(line);
    fclose(f);
  } else return -1;
  return 0;
}

/**
 * @fn int netiface_status_load_packets(const char* iface_name, struct packets_s *packets)
 * @brief Load the packets informations.
 * @param iface_name The iface name.
 * @param packets The result informations.
 * @return -1 on error else 0 on success.
 */
int netiface_status_load_packets(const char* iface_name, struct packets_s *packets){
  memset(packets, 0, sizeof(struct packets_s));
  packets->valid = 0;
  FILE *f = fopen(PROCFS_NET_DEV, "r");
  if (f) {
    char* line = NULL;
    size_t sz = 0;
    int cpt = 0;
    while (getline(&line, &sz, f) != -1) {
      if((cpt++) <= 1) //skip title lines
	continue;
      stringtoken_t tokens = stringtoken_init(line, ":. ");
      char* devname = stringtoken_next_token(tokens);
      if(!strcmp(devname, iface_name)) {
	packets->rx.bytes = netiface_status_extract_long(tokens, 0L);
	packets->rx.packets = netiface_status_extract_long(tokens, 0L);
	packets->rx.errors = netiface_status_extract_int(tokens, 0);
	packets->rx.dropped = netiface_status_extract_int(tokens, 0);
	packets->rx.fifo_errors = netiface_status_extract_int(tokens, 0);
	packets->rx.frame_errors = netiface_status_extract_int(tokens, 0);
	packets->rx.compressed = netiface_status_extract_int(tokens, 0);
	packets->rx.multicast = netiface_status_extract_int(tokens, 0);
									
	packets->tx.bytes = netiface_status_extract_long(tokens, 0L);
	packets->tx.packets = netiface_status_extract_long(tokens, 0L);
	packets->tx.errors = netiface_status_extract_int(tokens, 0);
	packets->tx.dropped = netiface_status_extract_int(tokens, 0);
	packets->tx.fifo_errors = netiface_status_extract_int(tokens, 0);
	packets->tx.collisions = netiface_status_extract_int(tokens, 0);
	packets->tx.carrier_errors = netiface_status_extract_int(tokens, 0);
	packets->tx.compressed = netiface_status_extract_int(tokens, 0);
	packets->valid = 1;
	stringtoken_release(tokens);
	if(devname) free(devname);
	break;
      }
      if(devname) free(devname);
      stringtoken_release(tokens);
    }
    if(line) free(line);
    fclose(f);
  } else return -1;
  return 0;
}


/**
 * @fn static long netiface_status_extract_long(stringtoken_t tokens, long def)
 * @brief Extract long value from token.
 * @param tokens The token.
 * @param def The default value.
 * @return the value.
 */
static long netiface_status_extract_long(stringtoken_t tokens, long def) {
  long l = 0L;
  char* temp = stringtoken_next_token(tokens);
  l = string_parse_long(temp, def);
  free(temp);
  return l;
}


/**
 * @fn static int netiface_status_extract_int(stringtoken_t tokens, int def)
 * @brief Extract int value from token.
 * @param tokens The token.
 * @param def The default value.
 * @return the value.
 */
static int netiface_status_extract_int(stringtoken_t tokens, int def) {
  int l = 0;
  char* temp = stringtoken_next_token(tokens);
  l = string_parse_int(temp, def);
  free(temp);
  return l;
}

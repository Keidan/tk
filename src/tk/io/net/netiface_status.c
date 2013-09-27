#include <tk/io/net/netiface_status.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tk/utils/string.h>
#include <tk/utils/stringtoken.h>

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
	wireless->internal.status = string_parse_int(stringtoken_next_token(tokens), 0);
	wireless->quality.link = string_parse_int(stringtoken_next_token(tokens), 0);
	wireless->quality.level = string_parse_int(stringtoken_next_token(tokens), 0);
	wireless->quality.noise = string_parse_int(stringtoken_next_token(tokens), 0);
	wireless->discarded_packets.nwid = string_parse_int(stringtoken_next_token(tokens), 0);
	wireless->discarded_packets.crypt = string_parse_int(stringtoken_next_token(tokens), 0);
	wireless->discarded_packets.frag = string_parse_int(stringtoken_next_token(tokens), 0);
	wireless->discarded_packets.retry = string_parse_int(stringtoken_next_token(tokens), 0);
	wireless->discarded_packets.misc = string_parse_int(stringtoken_next_token(tokens), 0);
	wireless->internal.missed_beacon = string_parse_int(stringtoken_next_token(tokens), 0);
	if(stringtoken_has_more_tokens(tokens))
	  wireless->internal.we21 = string_parse_int(stringtoken_next_token(tokens), 0);
	wireless->internal.wireless = 1;
	stringtoken_release(tokens);
	break;
      }
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
	packets->rx.bytes = string_parse_long(stringtoken_next_token(tokens), 0);
	packets->rx.packets = string_parse_long(stringtoken_next_token(tokens), 0);
	packets->rx.errors = string_parse_int(stringtoken_next_token(tokens), 0);
	packets->rx.dropped = string_parse_int(stringtoken_next_token(tokens), 0);
	packets->rx.fifo_errors = string_parse_int(stringtoken_next_token(tokens), 0);
	packets->rx.frame_errors = string_parse_int(stringtoken_next_token(tokens), 0);
	packets->rx.compressed = string_parse_int(stringtoken_next_token(tokens), 0);
	packets->rx.multicast = string_parse_int(stringtoken_next_token(tokens), 0);
									
	packets->tx.bytes = string_parse_long(stringtoken_next_token(tokens), 0);
	packets->tx.packets = string_parse_long(stringtoken_next_token(tokens), 0);
	packets->tx.errors = string_parse_int(stringtoken_next_token(tokens), 0);
	packets->tx.dropped = string_parse_int(stringtoken_next_token(tokens), 0);
	packets->tx.fifo_errors = string_parse_int(stringtoken_next_token(tokens), 0);
	packets->tx.collisions = string_parse_int(stringtoken_next_token(tokens), 0);
	packets->tx.carrier_errors = string_parse_int(stringtoken_next_token(tokens), 0);
	packets->tx.compressed = string_parse_int(stringtoken_next_token(tokens), 0);
	packets->valid = 1;
	stringtoken_release(tokens);
	break;
      }
      stringtoken_release(tokens);
    }
    if(line) free(line);
    fclose(f);
  } else return -1;
  return 0;
}

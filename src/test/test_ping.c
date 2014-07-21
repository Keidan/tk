#include <tk/io/net/proto/ping.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/syssig.h>
#include <unistd.h>

void ping_event_handler(ping_t p, struct ping_event_data_s data) {
  printf("Ping result: %d - %s(%s)\n", data.result, data.host, data.ip);
  if(data.result == PING_RESULT_SUCCESS)
    printf("Ping ok (seq %d) in %d msec\n", data.seq, data.timestamp);
}

int main(int argc, char** argv) {
  syssig_init(log_init_cast_user("test_ping", LOG_PID|LOG_CONS|LOG_PERROR), NULL);

  ping_t p = ping_new("wlan0");
  if(!p) return 0;
  ping_set_event_handler(p, ping_event_handler, NULL);
  ping_start(p, "192.168.43.1", 150000);

  while(1) sleep(1);
  ping_delete(p);
  log_close();
  return 0;
}

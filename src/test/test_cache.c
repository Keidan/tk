#include <tk/utils/cache/cache.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/syssig.h>


void cache_item_load(cache_item_name_t  name, struct cache_item_data_s* data) {
  data->data = malloc(255);
  sprintf(data->data, "%s_data", name);
  data->length = strlen(data->data);
  printf("Item size: %d\nItem data: '%s'\n", data->length, data->data);
}

int main(int argc, char** argv) {
  syssig_init(log_init_cast_user("test_cache", LOG_PID|LOG_CONS|LOG_PERROR), NULL);


  cache_t cache = cache_new("");
  cache_set_logs_enable(cache, true);

  struct cache_item_data_s body;
  cache_get_item(cache, "plop", cache_item_load, &body);
  cache_release_item(&body);

  cache_get_item(cache, "plop", cache_item_load, &body);
  printf("Item size: %d\nItem data: '%s'\n", body.length, body.data);
  cache_release_item(&body);

  cache_delete(cache);

  log_close();
  return 0;
}

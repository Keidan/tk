#include <tk/io/lua.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tk/sys/log.h>
#include <tk/sys/syssig.h>
#include <unistd.h>

int main(int argc, char** argv) {
  syssig_init(log_init_cast_user("test_lua", LOG_PID|LOG_CONS|LOG_PERROR), NULL);
  int ret;
  lua_t lua = lua_new("res/print.lua");
  lua_call_method_str(lua, "Callback", (uint8_t*)"azerty", 6);


  lua_call_method(lua, "GetLuaValue");
  lua_prepare_next_result(lua);
  lua_next_result_int(lua, &ret);
  printf("Result from lua: %d\n", ret);

  lua_delete(lua);
  return 0;
}

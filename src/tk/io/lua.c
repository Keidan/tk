/**
 *******************************************************************************
 * @file lua.c
 * @author Keidan
 * @date 23/05/2014
 * @par Project
 * tk
 *
 * @par Copyright
 * Copyright 2011-2014 Keidan, all right reserved
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
#include <tk/io/lua.h>
#include <tk/sys/log.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define create(local, param) lua_State * local = (lua_State *)param


/**
 * @fn lua_t lua_new(const char* luafile)
 * @brief Starts the Lua engine and load a lua script.
 * @param luafile The Lua file
 * @return The Lua context
 */
lua_t lua_new(const char* luafile) {
  lua_State *lua = NULL;
  /* Create Lua state variable */
  lua = luaL_newstate();
  if(!lua) {
    logger(LOG_ERR, "%s: Not enough memory to Create Lua state variable.", __func__);
    return NULL;
  }
  /* Load Lua libraries */
  luaL_openlibs(lua);

  /* load only */
  if (luaL_loadfile(lua, luafile)) { /* Load but don't run the Lua script */
    logger(LOG_ERR, "%s: Unable to load the Lua file: '%s'", __func__, lua_tostring(lua, -1));
    lua_delete(lua);
    return NULL;
  }
  if (lua_pcall(lua, 0, 0, 0)) { 
    logger(LOG_ERR, "%s: Unable to initialize the Lua pcall: '%s'", __func__, lua_tostring(lua, -1));
    lua_delete(lua);
    return NULL;
  }
  return lua;
}

/**
 * @fn void lua_delete(lua_t lua)
 * @brief Close the Lua engine.
 * @param lua The Lua context
 */
void lua_delete(lua_t lua) {
  //create(l, lua);
  //if(l) lua_close(l), l = NULL;
}

/**
 * @fn int lua_call_method(lua_t lua, const char* method_name)
 * @brief Call Lua function without argument.
 * @param lua Lua context.
 * @param method_name The method name.
 * @return -1 on error, 0 else.
 */
int lua_call_method(lua_t lua, const char* method_name) {
  create(l, lua);
  lua_pushnil(l);
  lua_getglobal(l, method_name);
  int error = lua_pcall(l, 0, 1, 0);
  if(error) {
    logger(LOG_ERR, "%s: Fail calling function '%s'. Error %d ('%s')", __func__, method_name, error, lua_tostring(lua, -1));
    return -1;
  }
  return 0;
}


/**
 * @fn int lua_call_method_str(lua_t lua, const char* method_name, uint8_t* buffer, uint32_t length)
 * @brief Call Lua function with a string argument.
 * @param lua Lua context.
 * @param method_name The method name.
 * @param buffer The argument.
 * @param length The argument length.
 * @return -1 on error, 0 else.
 */
int lua_call_method_str(lua_t lua, const char* method_name, uint8_t* buffer, uint32_t length) {
  create(l, lua);
  lua_pushnil(l);
  lua_getglobal(l, method_name);
  lua_pushlstring(l, (char*)buffer, length);
  int error = lua_pcall(l, 1, 1, 0);
  if(error) {
    logger(LOG_ERR, "%s: Fail calling function '%s'. Error %d ('%s')", __func__, method_name, error, lua_tostring(lua, -1));
    return -1;
  }
  return 0;
}

/**
 * @fn void lua_prepare_next_result(lua_t lua);
 * @brief Prepare for the "lua_next_result" calls.
 * @param lua Lua context.
 */
void lua_prepare_next_result(lua_t lua) {
  create(l, lua);
  lua_pushnil(l);
}

/**
 * @fn void lua_next_result_number(lua_t lua, double* value)
 * @brief Advance the Lua pointer to the next result.
 * @param lua Lua context.
 * @param value The result value
 */
void lua_next_result_number(lua_t lua, double* value) {
  create(l, lua);
  lua_next(l, -2);
  *value = lua_tonumber(l, -1);
  lua_pop(l, 1);
}

/**
 * @fn void lua_next_result_int(lua_t lua, int* value)
 * @brief Advance the Lua pointer to the next result.
 * @param lua Lua context.
 * @param value The result value
 */
void lua_next_result_int(lua_t lua, int* value) {
  create(l, lua);
  lua_next(l, -2);
  *value = lua_tointeger(l, -1);
  lua_pop(l, 1);
}

/**
 * @fn void lua_next_result_bool(lua_t lua, _Bool* value)
 * @brief Advance the Lua pointer to the next result.
 * @param lua Lua context.
 * @param value The result value
 */
void lua_next_result_bool(lua_t lua, _Bool* value) {
  create(l, lua);
  lua_next(l, -2);
  *value = !!lua_toboolean(l, -1);
  lua_pop(l, 1);
}

/**
 * @fn void lua_next_result_str(lua_t lua, char* value)
 * @brief Advance the Lua pointer to the next result.
 * @param lua Lua context.
 * @param value The result value
 */
void lua_next_result_str(lua_t lua, char* value) {
  create(l, lua);
  lua_next(l, -2);
  const char* ret = lua_tostring(l, -1);
  memcpy(value, ret, strlen(ret));
  lua_pop(l, 1);
}

/**
 *******************************************************************************
 * @file lua.h
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
#ifndef __LUA_H__
  #define __LUA_H__

  #include <stdint.h>

  typedef void* lua_t;

  typedef enum {
    LUA_NUM = 0,
    LUA_BOOL,
    LUA_INT,
    LUA_STR
  }lua_type_et;


  /**
   * @fn lua_t lua_new(const char* luafile)
   * @brief Starts the Lua engine and load a lua script.
   * @param luafile The Lua file
   * @return The Lua context
   */
  lua_t lua_new(const char* luafile);

  /**
   * @fn void lua_delete(lua_t lua)
   * @brief Close the Lua engine.
   * @param lua The Lua context
   */
  void lua_delete(lua_t lua);

  /**
   * @fn int lua_call_method(lua_t lua, const char* method_name)
   * @brief Call Lua function without argument.
   * @param lua Lua context.
   * @param method_name The method name.
   * @return -1 on error, 0 else.
   */
  int lua_call_method(lua_t lua, const char* method_name);

  /**
   * @fn int lua_call_method_str(lua_t lua, const char* method_name, uint8_t* buffer, uint32_t length)
   * @brief Call Lua function with a string argument.
   * @param lua Lua context.
   * @param method_name The method name.
   * @param buffer The argument.
   * @param length The argument length.
   * @return -1 on error, 0 else.
   */
  int lua_call_method_str(lua_t lua, const char* method_name, uint8_t* buffer, uint32_t length);

  /**
   * @fn void lua_prepare_next_result(lua_t lua);
   * @brief Prepare for the "lua_next_result" calls.
   * @param lua Lua context.
   */
  void lua_prepare_next_result(lua_t lua);

  /**
   * @fn void lua_next_result_str(lua_t lua, char* value)
   * @brief Advance the Lua pointer to the next result.
   * @param lua Lua context.
   * @param value The result value
   */
  void lua_next_result_str(lua_t lua, char* value);

  /**
   * @fn void lua_next_result_number(lua_t lua, double* value)
   * @brief Advance the Lua pointer to the next result.
   * @param lua Lua context.
   * @param value The result value
   */
  void lua_next_result_number(lua_t lua, double* value);

  /**
   * @fn void lua_next_result_int(lua_t lua, int* value)
   * @brief Advance the Lua pointer to the next result.
   * @param lua Lua context.
   * @param value The result value
   */
  void lua_next_result_int(lua_t lua, int* value);

  /**
   * @fn void lua_next_result_bool(lua_t lua, _Bool* value)
   * @brief Advance the Lua pointer to the next result.
   * @param lua Lua context.
   * @param value The result value
   */
  void lua_next_result_bool(lua_t lua, _Bool* value);


#endif /* __LUA_H__ */

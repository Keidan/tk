#check for config
include(CheckIncludeFile)
include(CheckTypeSize)

set(test_zlib 1)
set(test_termios 1)
set(test_ncurses 1)
set(test_ipct 1)
set(test_rtnetlink 1)
set(test_lua 1)
set(build_test 1)

if("${DISABLE_TEST}" MATCHES "yes")
  set(build_test 0)
endif()
if("${DISABLE_ZLIB}" MATCHES "yes")
  set(test_zlib 0)
endif()
if("${DISABLE_TERMIOS}" MATCHES "yes")
  set(test_termios 0)
endif()
if("${DISABLE_NCURSES}" MATCHES "yes")
  set(test_ncurses 0)
endif()
if("${DISABLE_IPCT}" MATCHES "yes")
  set(test_ipct 0)
endif()
if("${DISABLE_RTNETLINK}" MATCHES "yes")
  set(test_rtnetlink 0)
endif()
if("${DISABLE_LUA}" MATCHES "yes")
  set(test_lua 0)
endif()

if("${test_zlib}" MATCHES "1")
  check_include_file(zlib.h HAVE_ZLIB_H)
endif()
if("${test_termios}" MATCHES "1")
  check_include_file(termios.h HAVE_TERMIOS_H)
endif()
if("${test_ncurses}" MATCHES "1")
  check_include_file(ncurses.h HAVE_NCURSES_H)
endif()
if("${test_ipct}" MATCHES "1")
  check_include_file(libiptc/libiptc.h HAVE_IPTC_H)
endif()
if("${test_rtnetlink}" MATCHES "1")
  check_include_file(linux/rtnetlink.h HAVE_RTNETLINK_H)
endif()
if("${test_lua}" MATCHES "1")
  if(EXISTS "/usr/include/lua52/")
    set(dir "lua52")
  elseif(EXISTS "/usr/include/lua5.2/")
    set(dir "lua5.2")
  elseif(EXISTS "/usr/include/lua-5.2/")
    set(dir "lua-5.2")
  else()
    set(dir "lua")
  endif()
  set(LUA_LIBRARY ${dir})
  include_directories("/usr/include/${dir}")
  link_directories("${TK_LIB_DIR}")
  check_include_file(${dir}/lua.h HAVE_LUA_H)
  check_include_file(${dir}/lauxlib.h HAVE_LAUXLIB_H)
  check_include_file(${dir}/lualib.h HAVE_LUALIB_H)
endif()

if(HAVE_ZLIB_H)
  set(CMAKE_EXTRA_INCLUDE_FILES zconf.h)
  check_type_size(z_crc_t HAVE_Z_CRC_T)
  set(CMAKE_EXTRA_INCLUDE_FILES)
  if(NOT HAVE_Z_CRC_T)
    add_definitions("-Dz_crc_t=unsigned long")
  endif(NOT HAVE_Z_CRC_T)
endif(HAVE_ZLIB_H)

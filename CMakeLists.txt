cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(tk C)

if("${DISTRIBUTION}" MATCHES "debug")
  set(DISTRIBUTION "debug")
  set(CMAKE_C_FLAGS "-Wall -O0 -g -fPIC")
  add_definitions(-DDEBUG)
else()
  set(CMAKE_C_FLAGS "-Wall -Werror -O2")
  set(DISTRIBUTION "release")
endif()
message("-- Target mode: ${DISTRIBUTION}")

# update directories
set(TK_SRC_DIR ${CMAKE_SOURCE_DIR}/src/tk)
set(TEST_SRC_DIR ${CMAKE_SOURCE_DIR}/src/test)
set(ZLIB_SRC_DIR ${CMAKE_SOURCE_DIR}/src/zlib-minizip)
set(TK_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# global config
include_directories("${TK_INCLUDE_DIR}")
include_directories("${TK_INCLUDE_DIR}/zlib-minizip")

# add custom targets
add_custom_target(clean-all
   COMMAND ${CMAKE_BUILD_TOOL} clean
   COMMAND ${CMAKE_COMMAND} -P cmake/clean-all.cmake
)
add_custom_target(doxygen
   COMMAND ${CMAKE_COMMAND} -P cmake/doxygen.cmake
)

# To produce x86_64 binaries
if("${CMAKE_BUILD_TYPE}" MATCHES "x86_64")
  # the name of the target operating system
  set(CMAKE_SYSTEM_NAME Linux)

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")

  set(TK_LIB_DIR ${CMAKE_SOURCE_DIR}/lib/${DISTRIBUTION}/x86_64)
  set(LIBRARY_OUTPUT_PATH ${TK_LIB_DIR})
  set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${DISTRIBUTION}/x86_64)
  file(MAKE_DIRECTORY ${TK_LIB_DIR})
# To produce x86 binaries
elseif("${CMAKE_BUILD_TYPE}" MATCHES "x86")
  # the name of the target operating system
  set(CMAKE_SYSTEM_NAME Linux)
 
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")

  set(TK_LIB_DIR ${CMAKE_SOURCE_DIR}/lib/${DISTRIBUTION}/x86)
  set(LIBRARY_OUTPUT_PATH ${TK_LIB_DIR})
  set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${DISTRIBUTION}/x86)
  file(MAKE_DIRECTORY ${TK_LIB_DIR})
# To produce xscale binaries
elseif("${CMAKE_BUILD_TYPE}" MATCHES "xscale")
  set(CMAKE_SYSTEM_NAME Linux)
  set(CMAKE_SYSTEM_VERSION 1)

  set(CMAKE_X_HOME /usr/local/arm-linux-4.4.2-v4)
  # specify the cross compiler
  set(CMAKE_C_COMPILER ${CMAKE_X_HOME}/bin/xscale-none-linux-gnueabi-gcc)
   
  # where is the target environment 
  set(CMAKE_FIND_ROOT_PATH  ${CMAKE_X_HOME})

  # search for programs in the build host directories
  set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
  # for libraries and headers in the target directories
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

  add_definitions(-DTARGET_BIGENDIAN)

  set(TK_LIB_DIR ${CMAKE_SOURCE_DIR}/lib/${DISTRIBUTION}/xscale)
  set(LIBRARY_OUTPUT_PATH ${TK_LIB_DIR})
  set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${DISTRIBUTION}/xscale)
  file(MAKE_DIRECTORY ${TK_LIB_DIR})
  set(inc_base  "${CMAKE_X_HOME}/xscale-none-linux-gnueabi/include")
  include_directories(
    "${inc_base}"
    "${inc_base}/ncurses"
    )
else()
  message(FATAL_ERROR "Invalid usage please use: cmake -DCMAKE_BUILD_TYPE=[x86|x86_64|xscale]")
endif()
link_directories("${TK_LIB_DIR}")

#check for includes
include(CheckIncludeFile)
check_include_file(zlib.h HAVE_ZLIB_H)
check_include_file(termios.h HAVE_TERMIOS_H)
check_include_file(ncurses.h HAVE_NCURSES_H)

# update the file
configure_file(${CMAKE_SOURCE_DIR}/cmake/config-cmake.h.in ${CMAKE_SOURCE_DIR}/include/tk/config.h)

# add minizp if zlib is found
if(HAVE_ZLIB_H)
  add_subdirectory(${ZLIB_SRC_DIR})
endif()
add_subdirectory(${TK_SRC_DIR})
add_subdirectory(${TEST_SRC_DIR})

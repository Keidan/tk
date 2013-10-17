#check for config
include(CheckIncludeFile)
include(CheckTypeSize)

check_include_file(zlib.h HAVE_ZLIB_H)
check_include_file(termios.h HAVE_TERMIOS_H)
check_include_file(ncurses.h HAVE_NCURSES_H)
check_include_file(libiptc/libiptc.h HAVE_IPTC_H)

if(HAVE_ZLIB_H)
  set(CMAKE_EXTRA_INCLUDE_FILES zconf.h)
  check_type_size(z_crc_t HAVE_Z_CRC_T)
  set(CMAKE_EXTRA_INCLUDE_FILES)
  if(NOT HAVE_Z_CRC_T)
    add_definitions("-Dz_crc_t=unsigned long")
  endif(NOT HAVE_Z_CRC_T)
endif(HAVE_ZLIB_H)

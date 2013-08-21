#tk.cmake

# Build TK env
exec_program(${CMAKE_COMMAND} lib/tk
  ARGS
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DDISTRIBUTION=${DISTRIBUTION} 
)
# Creation of tk targets
add_custom_target(libtk 
  COMMAND make -C lib/tk
)
add_custom_target(libtk_clean-all 
  COMMAND make -C lib/tk clean-all
)
set(tk_prefix lib/tk/)
add_dependencies(bns libtk)
add_dependencies(clean-all libtk_clean-all)

set(extra_clean 
  ${tk_prefix}lib/${DISTRIBUTION}/${CMAKE_BUILD_TYPE}/libtk.so 
  ${tk_prefix}src/tk/CMakeFiles/tk.dir/text/string.c.o
  ${tk_prefix}src/tk/CMakeFiles/tk.dir/text/stringbuffer.c.o
  ${tk_prefix}src/tk/CMakeFiles/tk.dir/text/stringtoken.c.o
  ${tk_prefix}src/tk/CMakeFiles/tk.dir/sys/log.c.o
  ${tk_prefix}src/tk/CMakeFiles/tk.dir/sys/stools.c.o
  ${tk_prefix}src/tk/CMakeFiles/tk.dir/sys/ssig.c.o
  ${tk_prefix}src/tk/CMakeFiles/tk.dir/sys/proc.c.o
  ${tk_prefix}src/tk/CMakeFiles/tk.dir/io/net/ntools.c.o
  ${tk_prefix}src/tk/CMakeFiles/tk.dir/io/net/nprint.c.o
  ${tk_prefix}src/zlib-minizip/CMakeFiles/zlib-minizip.dir/unzip.c.o
  ${tk_prefix}src/zlib-minizip/CMakeFiles/zlib-minizip.dir/ioapi.c.o
)
if(HAVE_TERMIOS_H)
  set(extra_clean ${extra_clean}
    ${tk_prefix}src/tk/CMakeFiles/tk.dir/io/sr.c.o
  )
endif(HAVE_TERMIOS_H)
if(HAVE_NCURSES_H)
  set(extra_clean ${extra_clean}
    ${tk_prefix}src/tk/CMakeFiles/tk.dir/shell/shell.c.o
    ${tk_prefix}src/tk/CMakeFiles/tk.dir/shell/shell_term.c.o
  )
endif(HAVE_NCURSES_H)

if(HAVE_ZLIB_H)
  set(extra_clean ${extra_clean}
    ${tk_prefix}src/tk/CMakeFiles/tk.dir/sys/z.c.o
  )
endif(HAVE_ZLIB_H)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${extra_clean}")


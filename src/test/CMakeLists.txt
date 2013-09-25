# shell case
if(HAVE_NCURSES_H)
  set(bin_src 
    ${TEST_SRC_DIR}/test_shell.c 
    )
  add_executable(test_shell ${bin_src})
  target_link_libraries(test_shell tk)
endif(HAVE_NCURSES_H)

#stringbuffer case
set(bin_src 
  ${TEST_SRC_DIR}/test_stringbuffer.c 
)
add_executable(test_stringbuffer ${bin_src})
target_link_libraries(test_stringbuffer tk)

# zlib case
if(HAVE_ZLIB_H)
  set(bin_src 
    ${TEST_SRC_DIR}/test_z.c 
    )
  add_executable(test_z ${bin_src})
  target_link_libraries(test_z tk)
endif(HAVE_ZLIB_H)


#iface case
set(bin_src 
  ${TEST_SRC_DIR}/test_iface.c 
)
add_executable(test_iface ${bin_src})
target_link_libraries(test_iface tk)

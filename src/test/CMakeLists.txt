
set(bin_src 
  ${TEST_SRC_DIR}/test_shell.c 
)
add_executable(test_shell ${bin_src})
target_link_libraries(test_shell tk)


set(bin_src 
  ${TEST_SRC_DIR}/test_stringbuffer.c 
)
add_executable(test_stringbuffer ${bin_src})
target_link_libraries(test_stringbuffer tk)

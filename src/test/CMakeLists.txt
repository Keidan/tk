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

#arp case
set(bin_src 
  ${TEST_SRC_DIR}/test_arp.c 
)
add_executable(test_arp ${bin_src})
target_link_libraries(test_arp tk)

# netfilter case
if(HAVE_IPTC_H)
  set(bin_src 
    ${TEST_SRC_DIR}/test_filter.c 
  )
  add_executable(test_filter ${bin_src})
  target_link_libraries(test_filter tk)
endif(HAVE_IPTC_H)

# netroute case
if(HAVE_RTNETLINK_H)
  set(bin_src 
    ${TEST_SRC_DIR}/test_route.c 
  )
  add_executable(test_route ${bin_src})
  target_link_libraries(test_route tk)
endif(HAVE_RTNETLINK_H)

set(bin_src 
  ${TEST_SRC_DIR}/test_tun.c 
  )
add_executable(test_tun ${bin_src})
target_link_libraries(test_tun tk)

#process case
set(bin_src 
  ${TEST_SRC_DIR}/test_process.c 
)
add_executable(test_process ${bin_src})
target_link_libraries(test_process tk)

#ping case
set(bin_src 
  ${TEST_SRC_DIR}/test_ping.c 
)
add_executable(test_ping ${bin_src})
target_link_libraries(test_ping tk)

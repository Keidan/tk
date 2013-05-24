
set(lib_src 
  ${TK_SRC_DIR}/text/string.c 
  ${TK_SRC_DIR}/text/stringtoken.c 
  ${TK_SRC_DIR}/sys/log.c 
  ${TK_SRC_DIR}/sys/sysutils.c 
  ${TK_SRC_DIR}/sys/proc.c 
  ${TK_SRC_DIR}/net/netutils.c 
  ${TK_SRC_DIR}/net/netprint.c 
)
add_library(tk SHARED ${lib_src})
target_link_libraries(tk m)

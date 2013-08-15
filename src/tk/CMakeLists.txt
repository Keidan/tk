
# add generic sources
set(lib_src 
  ${TK_SRC_DIR}/text/string.c 
  ${TK_SRC_DIR}/text/stringtoken.c
  ${TK_SRC_DIR}/text/stringbuffer.c 
  ${TK_SRC_DIR}/collection/htable.c 
  ${TK_SRC_DIR}/collection/llist.c 
  ${TK_SRC_DIR}/collection/llist_iter.c 
  ${TK_SRC_DIR}/collection/fifo.c
  ${TK_SRC_DIR}/sys/log.c 
  ${TK_SRC_DIR}/sys/syssig.c 
  ${TK_SRC_DIR}/sys/sysutils.c 
  ${TK_SRC_DIR}/sys/proc.c
  ${TK_SRC_DIR}/io/net/netutils.c 
  ${TK_SRC_DIR}/io/net/netprint.c
)
# generic link
set(link m pthread)

# add serial management if termios is found
if(HAVE_TERMIOS_H)
  set(lib_src ${lib_src}
    ${TK_SRC_DIR}/io/sr.c
  )
endif(HAVE_TERMIOS_H)
# add shell if ncurses is found
if(HAVE_NCURSES_H)
  set(link ${link} ncurses)
  set(lib_src ${lib_src}
    ${TK_SRC_DIR}/shell/shell_term.c
    ${TK_SRC_DIR}/shell/shell.c
  )
endif(HAVE_NCURSES_H)
# add minizp if zlib is found
if(HAVE_ZLIB_H)
  set(link ${link} zlib-minizip)
  set(lib_src ${lib_src} 
    ${TK_SRC_DIR}/sys/z.c
  )
endif(HAVE_ZLIB_H)


add_library(tk SHARED ${lib_src})
target_link_libraries(tk ${link})

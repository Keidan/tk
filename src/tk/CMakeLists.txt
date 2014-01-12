
# add generic sources
set(lib_src 
  ${TK_SRC_DIR}/utils/string.c 
  ${TK_SRC_DIR}/utils/stringtoken.c
  ${TK_SRC_DIR}/utils/stringbuffer.c 
  ${TK_SRC_DIR}/utils/bytebuffer.c 
  ${TK_SRC_DIR}/utils/htable.c 
  ${TK_SRC_DIR}/utils/llist.c 
  ${TK_SRC_DIR}/utils/llist_iter.c 
  ${TK_SRC_DIR}/utils/fifo.c
  ${TK_SRC_DIR}/sys/log.c 
  ${TK_SRC_DIR}/sys/syssig.c 
  ${TK_SRC_DIR}/sys/systools.c 
  ${TK_SRC_DIR}/sys/proc.c
  ${TK_SRC_DIR}/sys/probe.c
  ${TK_SRC_DIR}/sys/process.c
  ${TK_SRC_DIR}/sys/systask.c
  ${TK_SRC_DIR}/io/file.c
  ${TK_SRC_DIR}/io/net/nettools.c
  ${TK_SRC_DIR}/io/net/netsocket.c
  ${TK_SRC_DIR}/io/net/nettun.c
  ${TK_SRC_DIR}/io/net/netprint.c
  ${TK_SRC_DIR}/io/net/netiface.c
  ${TK_SRC_DIR}/io/net/netiface_status.c
  ${TK_SRC_DIR}/io/net/proto/arp.c
  ${TK_SRC_DIR}/io/net/proto/ping.c
)
# generic link
set(link m pthread rt)

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
    ${TK_SRC_DIR}/utils/z.c
  )
endif(HAVE_ZLIB_H)
# add netfilter if iptc is found
if(HAVE_IPTC_H)
  set(lib_src ${lib_src} 
    ${TK_SRC_DIR}/io/net/netfilter.c
  )
  set(link ${link} ip4tc)
endif(HAVE_IPTC_H)
# add netroute if rtnetlink is found
if(HAVE_RTNETLINK_H)
  set(lib_src ${lib_src} 
    ${TK_SRC_DIR}/io/net/netroute.c
  )
endif(HAVE_RTNETLINK_H)

add_library(tk SHARED ${lib_src})
target_link_libraries(tk ${link})

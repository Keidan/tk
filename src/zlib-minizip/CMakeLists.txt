
set(lib_src 
  ${ZLIB_SRC_DIR}/ioapi.c 
  ${ZLIB_SRC_DIR}/unzip.c
  ${ZLIB_SRC_DIR}/zip.c
)
add_library(zlib-minizip SHARED ${lib_src})
target_link_libraries(zlib-minizip z)

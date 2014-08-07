#clean-all.cmake
set(cmake_generated ${CMAKE_BINARY_DIR}/CMakeCache.txt
                    ${CMAKE_BINARY_DIR}/cmake_install.cmake  
                    ${CMAKE_BINARY_DIR}/Makefile
                    ${CMAKE_BINARY_DIR}/CMakeFiles
                    ${CMAKE_BINARY_DIR}/src/zlib-minizip/cmake_install.cmake  
                    ${CMAKE_BINARY_DIR}/src/zlib-minizip/Makefile
                    ${CMAKE_BINARY_DIR}/src/zlib-minizip/CMakeFiles
                    ${CMAKE_BINARY_DIR}/src/tk/cmake_install.cmake  
                    ${CMAKE_BINARY_DIR}/src/tk/Makefile
                    ${CMAKE_BINARY_DIR}/src/tk/CMakeFiles
                    ${CMAKE_BINARY_DIR}/src/test/cmake_install.cmake  
                    ${CMAKE_BINARY_DIR}/src/test/Makefile
                    ${CMAKE_BINARY_DIR}/src/test/CMakeFiles
                    ${CMAKE_BINARY_DIR}/bin/x86
                    ${CMAKE_BINARY_DIR}/bin/x86_64
                    ${CMAKE_BINARY_DIR}/bin/xscale
		    ${CMAKE_BINARY_DIR}/doc
)

foreach(file ${cmake_generated})
  if (EXISTS ${file})
     file(REMOVE_RECURSE ${file})
  endif()
endforeach(file)


execute_process(
  COMMAND find . -type f -name "*~" -exec rm {} \;
) 


FIND_PACKAGE( ROOT  REQUIRED )



file(GLOB src_files  "*.cxx" "*.cc")



INCLUDE_DIRECTORIES(. ../extern/tclap-1.2.1/include ../extern/UNew ${ROOT_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )




add_executable(read_in  ${src_files})






target_link_libraries(read_in ${ROOT_LIBRARIES})



INSTALL(TARGETS read_in  
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
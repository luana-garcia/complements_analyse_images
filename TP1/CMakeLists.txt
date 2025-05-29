cmake_minimum_required(VERSION 2.8)

project(inti)

find_package(TCL)

include_directories(${CMAKE_SOURCE_DIR}/include
                   ${TK_INCLUDE_PATH}
                   ${TCL_INCLUDE_PATH})

set(CMAKE_CXX_FLAGS "-DNDEBUG -O3 ${CMAKE_CXX_FLAGS}")

SET(CMAKE_VERBOSE_MAKEFILE ON)
add_definitions(-DUSE_NON_CONST -DUSE_COMPOSITELESS_PHOTO_PUT_BLOCK)
add_executable(intiwish src/photo.cc src/tkMain.c src/morfo.cc src/Image.cc src/tpmorpho1.cc src/tpmorpho2.cc)
add_custom_command(TARGET intiwish POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy intiwish ${CMAKE_SOURCE_DIR}/.)

target_link_libraries(intiwish  ${TCL_LIBRARY} ${TK_LIBRARY})

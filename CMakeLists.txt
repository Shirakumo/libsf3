cmake_minimum_required(VERSION 3.2...3.30)
project(sf3 C)

option(BUILD_VIEWER "Build the file viewer" ON)
option(BUILD_SHARED_LIBS "Build the shared library" ON)
option(BUILD_TESTER "Build the tester application" ON)
option(BUILD_DOCS "Build the documentation via Doxygen" ON)

file(GLOB HEADERS "${PROJECT_SOURCE_DIR}/src/*.h")
install(FILES ${HEADERS} TYPE INCLUDE)

if(BUILD_SHARED_LIBS)
  add_library(sf3 SHARED
    "src/sf3_lib.c")
  set_property(TARGET sf3 PROPERTY C_STANDARD 99)
  include(CheckIncludeFile)
  target_compile_options(sf3 PRIVATE -fvisibility=hidden -O3 -g)
  target_compile_definitions(sf3 PRIVATE SF3_BUILD=1)
  check_include_file("sys/mman.h" HAVE_MMAN_H)
  if(HAVE_MMAN_H)
    target_compile_definitions(sf3 PRIVATE HAVE_MMAN_H=1)
  endif()
  check_include_file("sys/stat.h" HAVE_STAT_H)
  if(HAVE_STAT_H)
    target_compile_definitions(sf3 PRIVATE HAVE_STAT_H=1)
  endif()
  install(TARGETS sf3)
endif()

if(BUILD_VIEWER)
  add_executable(sf3_viewer
    "src/viewer.c")
  set_property(TARGET sf3_viewer PROPERTY C_STANDARD 99)
  target_compile_options(sf3_viewer PRIVATE -fvisibility=hidden -g)
  target_link_libraries(sf3_viewer PRIVATE sf3)
  install(TARGETS sf3_viewer)
endif()

if(BUILD_TESTER)
  add_executable(sf3_tester
    "src/test.c")
  set_property(TARGET sf3_tester PROPERTY C_STANDARD 99)
  target_compile_options(sf3_tester PRIVATE -fvisibility=hidden -g)
endif()

if(BUILD_DOCS)
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    add_custom_target(docs ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/man TYPE MAN)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html TYPE DOC)
  else(DOXYGEN_FOUND)
    message(STATUS "Cannot build documentation")
  endif(DOXYGEN_FOUND)
endif()

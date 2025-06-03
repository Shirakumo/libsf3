cmake_minimum_required(VERSION 3.2...3.30)
project(sf3 C)

option(BUILD_VIEWER "Build the file viewer" ON)

install(FILES "src/*.h" DESTINATION include/)

if(BUILD_VIEWER)
  add_executable(sf3_viewer
    "src/viewer.c")
  set_property(TARGET sf3_viewer PROPERTY C_STANDARD 99)
  target_compile_options(sf3_viewer PRIVATE -fvisibility=hidden -g)
  install(TARGETS sf3_viewer DESTINATION bin/)
endif()

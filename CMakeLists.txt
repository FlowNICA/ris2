project(RISOVALKA2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE debug)

cmake_minimum_required(VERSION 4.0 FATAL_ERROR)

find_package(ROOT REQUIRED)
find_package(QnTools QUIET)

configure_file(docs/Doxyfile.in ${CMAKE_SOURCE_DIR}/docs/Doxyfile @ONLY)
set( CALC_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/src )

if(QnTools_FOUND)
  include_directories( ${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS} ${QnTools_INCLUDE_DIR}/QnTools )

  set( QnTools_LIB_PATH QnTools::Base )
  set( QnTools_INCLUDE_PATH ${QnTools_INCLUDE_DIR}/QnTools )
  configure_file(src/main.cc.in ${CMAKE_SOURCE_DIR}/src/main.cc @ONLY)

  add_executable(draw src/main.cc)
  target_link_libraries(draw PRIVATE QnTools::Base ROOT::Tree ROOT::RIO ROOT::MathMore )
else()
  configure_file(src/main_no_qntools.cc.in ${CMAKE_SOURCE_DIR}/src/main.cc @ONLY)
  add_executable(draw src/main.cc)
  target_link_libraries(draw PRIVATE ROOT::Tree ROOT::RIO ROOT::MathMore )
endif()

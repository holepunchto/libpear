cmake_minimum_required(VERSION 3.31)

find_package(cmake-fetch REQUIRED PATHS node_modules/cmake-fetch)

project(pear C CXX ASM)

set(PEAR_RESTART_AFTER_BOOTSTRAP ON CACHE BOOL "Restart the appling once the Pear runtime has bootstrapped")

fetch_package("github:holepunchto/libappling")
fetch_package("github:holepunchto/libfx")

add_library(pear OBJECT)

set_target_properties(
  pear
  PROPERTIES
  C_STANDARD 99
  POSITION_INDEPENDENT_CODE ON
)

target_sources(
  pear
  PRIVATE
    src/pear.c
)

target_include_directories(
  pear
  INTERFACE
    include
  PRIVATE
    $<TARGET_PROPERTY:appling,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:fx,INTERFACE_INCLUDE_DIRECTORIES>
)

target_compile_definitions(
  pear
  PRIVATE
    PEAR_RESTART_AFTER_BOOTSTRAP=$<BOOL:${PEAR_RESTART_AFTER_BOOTSTRAP}>
)

add_library(pear_static STATIC)

set_target_properties(
  pear_static
  PROPERTIES
  OUTPUT_NAME pear
  PREFIX lib
)

target_link_libraries(
  pear_static
  PUBLIC
    pear
  PRIVATE
    appling_static
    fx_static
)

install(TARGETS pear_static)

if(PROJECT_IS_TOP_LEVEL)
  enable_testing()

  add_subdirectory(test)
endif()

cmake_minimum_required(VERSION 3.25)

find_package(cmake-bare REQUIRED PATHS node_modules/cmake-bare)
find_package(cmake-fetch REQUIRED PATHS node_modules/cmake-fetch)

project(pear C CXX)

if(MSVC)
  add_compile_options(/MT$<$<CONFIG:Debug>:d>)
endif()

fetch_package("github:holepunchto/bare@1.13.1" SOURCE_DIR bare_dir)
fetch_package("github:holepunchto/libappling" SOURCE_DIR appling_dir)
fetch_package("github:holepunchto/libfx")

add_library(pear STATIC)

set_target_properties(
  pear
  PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  OUTPUT_NAME pear
  PREFIX lib
)

target_sources(
  pear
  PRIVATE
    src/pear.c
  PRIVATE
    $<TARGET_OBJECTS:appling>
    $<TARGET_OBJECTS:appling_bootstrap>
    $<TARGET_OBJECTS:bare>
    $<TARGET_OBJECTS:base64>
    $<TARGET_OBJECTS:compact>
    $<TARGET_OBJECTS:crc>
    $<TARGET_OBJECTS:fs>
    $<TARGET_OBJECTS:fx>
    $<TARGET_OBJECTS:hex>
    $<TARGET_OBJECTS:log>
    $<TARGET_OBJECTS:path>
    $<TARGET_OBJECTS:quickbit>
    $<TARGET_OBJECTS:simdle>
    $<TARGET_OBJECTS:sodium>
    $<TARGET_OBJECTS:udx>
    $<TARGET_OBJECTS:url>
    $<TARGET_OBJECTS:utf>
)

target_include_directories(
  pear
  PUBLIC
    include
)

target_link_libraries(
  pear
  PRIVATE
    appling
    appling_bootstrap
    bare
    base64
    compact
    crc
    fs
    fx
    hex
    log
    path
    quickbit
    simdle
    sodium
    udx
    url
    utf
)

link_bare_modules(
  pear
  EXCLUDE
    bare-crypto
    bare-dns
    bare-inspector
    bare-pipe
    bare-repl
    bare-signals
    bare-tcp
    bare-tls
    bare-tty
  WORKING_DIRECTORY "${bare_dir}"
)

link_bare_modules(
  pear
  WORKING_DIRECTORY "${appling_dir}"
)

install(TARGETS pear)

if(PROJECT_IS_TOP_LEVEL)
  enable_testing()

  add_subdirectory(test)
endif()

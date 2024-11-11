cmake_minimum_required(VERSION 3.25)

find_package(cmake-fetch REQUIRED PATHS node_modules/cmake-fetch)

project(pear C)

if(MSVC)
  add_compile_options(/MT$<$<CONFIG:Debug>:d>)
endif()

fetch_package("github:holepunchto/libappling")
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
)

target_include_directories(
  pear
  PUBLIC
    include
)

target_link_libraries(
  pear
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
    $<TARGET_OBJECTS:napi>
    $<TARGET_OBJECTS:path>
    $<TARGET_OBJECTS:quickbit>
    $<TARGET_OBJECTS:simdle>
    $<TARGET_OBJECTS:sodium>
    $<TARGET_OBJECTS:udx>
    $<TARGET_OBJECTS:uv>
  PUBLIC
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
    napi
    path
    quickbit
    simdle
    sodium
    udx
    uv
)

install(TARGETS pear)

cmake_minimum_required(VERSION 3.25)

project(pear)

if(NOT TARGET appling)
  add_subdirectory(vendor/libappling EXCLUDE_FROM_ALL)
endif()

if(NOT TARGET fx)
  add_subdirectory(vendor/libfx EXCLUDE_FROM_ALL)
endif()

add_library(pear STATIC)

set_target_properties(
  pear
  PROPERTIES
  POSITION_INDEPENDENT_CODE ON
)

target_sources(
  pear
  PRIVATE
    src/pear.c
    $<TARGET_OBJECTS:appling>
    $<TARGET_OBJECTS:appling_bootstrap>
    $<TARGET_OBJECTS:bare>
    $<TARGET_OBJECTS:fx>
    $<TARGET_OBJECTS:log>
    $<TARGET_OBJECTS:path>
    $<TARGET_OBJECTS:fs>
)

target_include_directories(
  pear
  PUBLIC
    include
)

target_link_libraries(
  pear
  PUBLIC
    appling
    appling_bootstrap
    bare
    fx
    log
    path
    fs
)

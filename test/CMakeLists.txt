list(APPEND tests
  keet
)

foreach(test IN LISTS tests)
  add_executable(${test} ${test}.c)

  target_link_libraries(
    ${test}
    PRIVATE
      # Link the resulting static archive directly to ensure that all symbols
      # have been properly bundled.
      $<TARGET_GENEX_EVAL:pear,$<LINK_LIBRARY:WHOLE_ARCHIVE,$<TARGET_FILE:pear>>>
    PRIVATE
      $<TARGET_GENEX_EVAL:fx,$<TARGET_PROPERTY:fx,INTERFACE_LINK_LIBRARIES>>
    PRIVATE
      js
  )

  add_test(
    NAME ${test}
    COMMAND ${test}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )

  set_tests_properties(
    ${test}
    PROPERTIES
    DISABLED TRUE # Don't actually run tests
  )
endforeach()


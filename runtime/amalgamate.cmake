function(zsr_amalgamate_sources ROOT TARGET)
  get_filename_component(TARGET_NAME ${TARGET} NAME)
  message("Amalgamating generated zserio sources to ${TARGET_NAME}...")

  file(WRITE "${TARGET}" "")

  file(GLOB_RECURSE SOURCES
    "${ROOT}/*.cpp")

  foreach(SRC ${SOURCES})
    get_filename_component(SRC_NAME ${SRC} NAME)
    message(STATUS "${SRC_NAME}")

    file(READ ${SRC} CONTENTS)
    file(APPEND ${TARGET} "${CONTENTS}")
  endforeach()
endfunction()

# Why 5?
#   cmake -P <script> <3> <4>
#   0     1  2        3   4
if (${CMAKE_ARGC} EQUAL 5)
  zsr_amalgamate_sources("${CMAKE_ARGV3}" "${CMAKE_ARGV4}")
else()
  message(FATAL_ERROR "Missing arguments.")
endif()

cmake_minimum_required(VERSION 3.10.2)
project(zserio-cpp-introspection-runtime)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll")

set(SRC  "${CMAKE_CURRENT_SOURCE_DIR}")
set(TEST "${SRC}/test")

if (NOT MSVC)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON) # This does not set -fPIC :(
  set(CMAKE_CXX_FLAGS -fPIC)
endif()

## ZSERIO C++ runtime (optional)
####

if (NOT TARGET ZserioCppRuntime)
  option(ZSERIO_RT_DIR "Directory of zserio C++ runtime CMakeLists.txt (e.g. extensions/cpp/runtime/src)")
  if (ZSERIO_RT_DIR)
    add_subdirectory("${ZSERIO_RT_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/zserio-cpp-rt")
  else ()
    message(FATAL_ERROR "Option ZSERIO_RT_DIR is empty.")
  endif()
endif()

## ZSR Runtime
####

add_library(zsr SHARED
  "${SRC}/src/types.cpp"
  "${SRC}/src/error.cpp"
  "${SRC}/src/introspectable.cpp")

target_compile_features(zsr
  PUBLIC cxx_std_17)

target_include_directories(zsr
  PUBLIC
    "${SRC}")

target_link_libraries(zsr
  PUBLIC
    ZserioCppRuntime)

set_target_properties(zsr
  PROPERTIES
    src_dir "${CMAKE_CURRENT_SOURCE_DIR}")

add_custom_target(zsr-amalgamate
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/amalgamate.cmake")

set_target_properties(zsr-amalgamate
  PROPERTIES
    src "${CMAKE_CURRENT_SOURCE_DIR}/amalgamate.cmake")

function(add_zserio_module NAME ROOT ENTRY)
  set(GEN_SRC_ROOT "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.zserio-gen/")
  file(MAKE_DIRECTORY "${GEN_SRC_ROOT}")

  find_program(JAVA java)
  if (NOT JAVA)
    message(FATAL_ERROR "Could not find java!")
  endif()

  set(ZSERIO ${JAVA})

  get_target_property(JAR_PATH zserio jar)
  if (${JAR_PATH} EQUAL "NOTFOUND")
    message(FATAL_ERROR "Could not read property 'jar' from target 'zserio'.")
  endif()

  list(APPEND ZSERIO "-jar")
  list(APPEND ZSERIO "${JAR_PATH}")

  add_custom_command(
    DEPENDS zserio
    OUTPUT "${GEN_SRC_ROOT}/reflection-defs.cpp"
    COMMAND ${ZSERIO} -setTopLevelPackage ${ZSERIO_TLP}
                      -cpp ${GEN_SRC_ROOT}
                      -cpp_reflect ${GEN_SRC_ROOT}
                      -src ${ROOT}
                      ${ENTRY})

  # Generated source amalgamation
  get_target_property(AMALGAMATE zsr-amalgamate src)

  add_custom_command(
    DEPENDS "${GEN_SRC_ROOT}/reflection-defs.cpp"
    OUTPUT "${GEN_SRC_ROOT}/amalgamation.cpp"
    COMMAND ${CMAKE_COMMAND} -P "${AMALGAMATE}"
                                "${GEN_SRC_ROOT}/${ZSERIO_TLP}"
                                "${GEN_SRC_ROOT}/amalgamation.cpp")

  add_library(${NAME} SHARED
    "${GEN_SRC_ROOT}/amalgamation.cpp")

  target_include_directories(${NAME}
    PUBLIC "${GEN_SRC_ROOT}")

  target_link_libraries(${NAME}
    PUBLIC ZserioCppRuntime)

  # Reflection runtime lib
  get_target_property(ZSR_SRC zsr src_dir)

  add_library(${NAME}-reflection SHARED
    "${ZSR_SRC}/zsr/stub.hpp"
    "${ZSR_SRC}/src/lib-prefix.cpp")

  target_compile_definitions(${NAME}-reflection
    PRIVATE
      REFLECTION_DEFS_INCLUDE="${GEN_SRC_ROOT}/reflection-defs.cpp")

  target_compile_options(${NAME}-reflection
    PRIVATE
      $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
      $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Werror>)

  target_include_directories(${NAME}-reflection
    PRIVATE
      "${ZSR_SRC}/src")

  target_link_libraries(${NAME}-reflection
    zsr
    ${NAME})
endfunction()

## Test
####

function(add_zsr_test TARGET FILE)
  add_executable(${TARGET}
    "${FILE}")

  target_include_directories(${TARGET}
    PRIVATE "${TEST}/zserio")

  target_link_libraries(${TARGET}
    PRIVATE zsr gtest gtest_main)

  target_compile_options(${TARGET}
    PRIVATE
      $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
      $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Werror>)

  target_compile_features(${TARGET}
    PUBLIC cxx_std_17)

  add_test(NAME "${TARGET}"
    COMMAND ${TARGET})
endfunction()

function(add_zsr_module_test NAME)
  add_zserio_module(zs-${NAME} "${TEST}/zserio/${NAME}" "${NAME}.zs")
  add_zsr_test(${NAME} "${TEST}/zserio/${NAME}/${NAME}.cpp")

  target_link_libraries(${NAME}
    PRIVATE zs-${NAME} zs-${NAME}-reflection)
endfunction()

# Enable tests by default
if (NOT DEFINED ZSR_ENABLE_TESTING)
  set(ZSR_ENABLE_TESTING On)
endif()

if (ZSR_ENABLE_TESTING)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/googletest/googletest")
  enable_testing()

  set(ZSERIO_TLP "zsr")

  add_zsr_test(variant-test "${TEST}/variant-test.cpp")

  add_zsr_module_test(bitmask_test)
  add_zsr_module_test(choice_test)
  add_zsr_module_test(union_test)
  add_zsr_module_test(enumeration_test)
  add_zsr_module_test(constant_test)
  add_zsr_module_test(structure_test)
endif()

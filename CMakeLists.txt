cmake_minimum_required(VERSION 3.16)
project(zsr)

set(TEST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/test")
set(SRC "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_POSITION_INDEPENDENT_CODE ON) # This does not set -fPIC :(
set(CMAKE_CXX_FLAGS "-fPIC")

## ZSERIO C++ runtime
####

option(ZSERIO_RT_DIR "Directory of zserio C++ runtime CMakeLists.txt (e.g. extensions/cpp/runtime/src)")
if (ZSERIO_RT_DIR)
  add_subdirectory("${ZSERIO_RT_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/zserio-cpp-rt")
else ()
  message(FATAL_ERROR "Option ZSERIO_RT_DIR is empty.")
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
    "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(zsr
  PUBLIC
    ZserioCppRuntime)

set(ZSERIO_TLP "nds") # Zserio top-level-package

function(add_zserio_module NAME ROOT ENTRY)
  set(GEN_SRC_ROOT "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.zserio-gen/")

  file(MAKE_DIRECTORY "${GEN_SRC_ROOT}")

  execute_process(
    COMMAND zserio -setTopLevelPackage ${ZSERIO_TLP}
                   -cpp ${GEN_SRC_ROOT}
                   -cpp_reflect ${GEN_SRC_ROOT}
                   -src ${ROOT}
                   ${ENTRY})

  file(GLOB_RECURSE GEN_SRC
    "${GEN_SRC_ROOT}/${ZSERIO_TLP}/*.cpp")

  if (GEN_SRC)
    add_library(${NAME} SHARED
      ${GEN_SRC})

    target_include_directories(${NAME}
      PUBLIC "${GEN_SRC_ROOT}")

    target_link_libraries(${NAME}
      PUBLIC ZserioCppRuntime)
  else()
    add_library(${NAME} INTERFACE)

    target_include_directories(${NAME}
      INTERFACE "${GEN_SRC_ROOT}")

    target_link_libraries(${NAME}
      INTERFACE ZserioCppRuntime)
  endif()

  add_library(${NAME}-reflection SHARED
    "${SRC}/src/lib-prefix.cpp")

  target_compile_definitions(${NAME}-reflection
    PRIVATE REFLECTION_DEFS_INCLUDE="${GEN_SRC_ROOT}/reflection-defs.cpp")

  target_compile_options(${NAME}-reflection
    PRIVATE
      "-Werror"
      "-Wall")

  target_include_directories(${NAME}-reflection
    PRIVATE
      "${SRC}/src")

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
    PRIVATE "${SRC}" "${TEST_SRC}/zserio")

  target_link_libraries(${TARGET}
    PRIVATE zsr gtest gtest_main)

  target_compile_options(${TARGET}
    PRIVATE
      "-Wall"
      "-Werror")

  target_compile_features(${TARGET}
    PUBLIC cxx_std_17)

  add_test(NAME "${TARGET}"
    COMMAND ${TARGET})
endfunction()

function(add_zsr_module_test NAME)
  add_zserio_module(zs-${NAME} "${TEST_SRC}/zserio/${NAME}" "${NAME}.zs")
  add_zsr_test(${NAME} "${TEST_SRC}/zserio/${NAME}/${NAME}.cpp")

  target_link_libraries(${NAME}
    PRIVATE zs-${NAME} zs-${NAME}-reflection)

  target_link_options(${NAME}
    PRIVATE -z now)
endfunction()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/googletest/googletest")
enable_testing()

set(ZSERIO_TLP "zsr")

add_zsr_test(variant-test "${TEST_SRC}/variant-test.cpp")

add_zsr_module_test(bitmask_test)
add_zsr_module_test(choice_test)
add_zsr_module_test(union_test)
add_zsr_module_test(enumeration_test)
add_zsr_module_test(constant_test)
add_zsr_module_test(structure_test)

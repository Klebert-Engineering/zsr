cmake_minimum_required(VERSION 3.10.2)
project(zserio-cpp-introspection-runtime)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll")

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /EHsc")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

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

target_compile_definitions(zsr
  PRIVATE
    ZSR_BUILD=1)

target_compile_options(zsr
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -fvisibility=hidden>)

add_custom_target(zsr-amalgamate
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/amalgamate.cmake")

set_target_properties(zsr-amalgamate
  PROPERTIES
    src "${CMAKE_CURRENT_SOURCE_DIR}/amalgamate.cmake")

function(add_zserio_module ZSR_MODULE_NAME)
  cmake_parse_arguments(PARSE_ARGV 0
    ZSR_MODULE "" "ROOT;ENTRY;TOP_LEVEL_PKG" "")

  if (NOT ZSR_MODULE_ROOT)
    message(FATAL_ERROR "Missing zserio-module argument ROOT!")
  elseif (NOT ZSR_MODULE_ENTRY)
    message(FATAL_ERROR "Missing zserio-module argument ENTRY!")
  endif()

  set(GEN_SRC_ROOT "${CMAKE_CURRENT_BINARY_DIR}/${ZSR_MODULE_NAME}.zserio-gen/")
  file(MAKE_DIRECTORY "${GEN_SRC_ROOT}")

  set(GEN_REFLECT_SRC_ROOT "${CMAKE_CURRENT_BINARY_DIR}/${ZSR_MODULE_NAME}.zserio-gen/reflection/")
  file(MAKE_DIRECTORY "${GEN_REFLECT_SRC_ROOT}")

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

  set(zserio_top_level_arg "")
  if (ZSR_MODULE_TOP_LEVEL_PKG)
    set(zserio_top_level_arg "-setTopLevelPackage")
  endif()

  execute_process(
    COMMAND ${ZSERIO} ${zserio_top_level_arg} ${ZSR_MODULE_TOP_LEVEL_PKG}
            -cpp ${GEN_SRC_ROOT}
            -cpp_reflect ${GEN_REFLECT_SRC_ROOT}
            -src ${ZSR_MODULE_ROOT}
            ${ZSR_MODULE_ENTRY}
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

  # Generated source amalgamation
  get_target_property(AMALGAMATE zsr-amalgamate src)

  execute_process(
    COMMAND ${CMAKE_COMMAND} -P "${AMALGAMATE}"
                                "${GEN_SRC_ROOT}/${ZSR_MODULE_TOP_LEVEL_PKG}"
                                "${GEN_SRC_ROOT}/amalgamation.cpp")

  add_library(${ZSR_MODULE_NAME} SHARED
    "${GEN_SRC_ROOT}/amalgamation.cpp")

  target_include_directories(${ZSR_MODULE_NAME}
    PUBLIC "${GEN_SRC_ROOT}")

  target_link_libraries(${ZSR_MODULE_NAME}
    PUBLIC ZserioCppRuntime)

  target_compile_options(${ZSR_MODULE_NAME}
    PUBLIC
      "$<$<CXX_COMPILER_ID:MSVC>:/bigobj;/EHsc;/wd4100>"
      "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fPIC>")

  #set_target_properties(${ZSR_MODULE_NAME}
    #PROPERTIES
      #WINDOWS_EXPORT_ALL_SYMBOLS ON)

  # Reflection runtime lib
  get_target_property(ZSR_SRC zsr src_dir)

  file(GLOB_RECURSE package_sources "${GEN_REFLECT_SRC_ROOT}/*.cpp")

  add_library(${ZSR_MODULE_NAME}-reflection SHARED
    "${package_sources}"
    "${GEN_REFLECT_SRC_ROOT}/reflection-main.cpp")

  target_compile_definitions(${ZSR_MODULE_NAME}-reflection
    PRIVATE
      ZSR_BUILD=1)

  target_compile_options(${ZSR_MODULE_NAME}-reflection
    PUBLIC
      "$<$<CXX_COMPILER_ID:MSVC>:/W4;/WX;/bigobj;/EHsc>"
      "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fPIC>"
    PRIVATE
      "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall;-fvisibility=hidden>")

  target_include_directories(${ZSR_MODULE_NAME}-reflection
    PRIVATE
      "${ZSR_SRC}/src")

  target_link_libraries(${ZSR_MODULE_NAME}-reflection
    zsr
    ${ZSR_MODULE_NAME})
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
      $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall>)

  target_compile_features(${TARGET}
    PUBLIC cxx_std_17)

  add_test(NAME "${TARGET}"
    COMMAND ${TARGET})
endfunction()

function(add_zsr_module_test NAME)
  cmake_parse_arguments(PARSE_ARGV 1
    ZSR_TEST "" "TOP_LEVEL_PKG" "")

  add_zserio_module(zs-${NAME}
    ROOT "${TEST}/zserio/${NAME}" ENTRY "${NAME}.zs" TOP_LEVEL_PKG "${ZSR_TEST_TOP_LEVEL_PKG}")
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

  add_zsr_test(variant-test "${TEST}/variant-test.cpp")

  add_zsr_module_test(bitmask_test TOP_LEVEL_PKG zsr)
  add_zsr_module_test(choice_test TOP_LEVEL_PKG zsr)
  add_zsr_module_test(union_test TOP_LEVEL_PKG zsr)
  add_zsr_module_test(enumeration_test TOP_LEVEL_PKG zsr)
  add_zsr_module_test(constant_test TOP_LEVEL_PKG zsr)
  add_zsr_module_test(structure_test TOP_LEVEL_PKG zsr)
  add_zsr_module_test(service_test TOP_LEVEL_PKG zsr)
endif()

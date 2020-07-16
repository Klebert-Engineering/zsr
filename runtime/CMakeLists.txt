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

target_compile_definitions(zsr
  PRIVATE
    ZSR_BUILD=1)

target_compile_options(zsr
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -fvisibility=hidden>)

function (add_submodule_lib TARGET ROOT_DIR SUBMODULE_DIR RECURSIVE)
  if (RECURSIVE)
    file(GLOB_RECURSE subdir_sources "${SUBMODULE_DIR}/*.cpp")
  else()
    file(GLOB subdir_sources "${SUBMODULE_DIR}/*.cpp")
  endif()

  if (subdir_sources)
    if (RECURSIVE)
      get_filename_component(dirname "${SUBMODULE_DIR}" NAME)
      message(STATUS "Adding target for ${TARGET} submodule ${dirname}")

      set(dirname "-${dirname}")
    else()
      message(STATUS "Adding target for ${TARGET} root")

      set(dirname "root")
    endif()

    set(subtarget ${TARGET}-${dirname})
    add_library(${subtarget} SHARED
      ${subdir_sources})

    target_include_directories(${subtarget}
      PUBLIC "${ROOT_DIR}")

    target_link_libraries(${subtarget}
      PUBLIC ZserioCppRuntime)

    target_link_libraries(${TARGET}
      INTERFACE ${subtarget})

    set_target_properties(${subtarget}
      PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON)

    if (MSVC)
      target_compile_options(${subtarget}
        PUBLIC
          /bigobj
          /EHsc
          /wd4100)
    else()
      target_compile_options(${subtarget}
        PUBLIC
          -fPIC)
    endif()
  endif()
endfunction()

function (add_zserio_module ZSR_MODULE_NAME)
  cmake_parse_arguments(PARSE_ARGV 0
    ZSR_MODULE "EXCLUDE_FROM_ALL" "ROOT;ENTRY;TOP_LEVEL_PKG" "")

  if (NOT ZSR_MODULE_ROOT)
    message(FATAL_ERROR "Missing zserio-module argument ROOT!")
  elseif (NOT ZSR_MODULE_ENTRY)
    message(FATAL_ERROR "Missing zserio-module argument ENTRY!")
  endif()

  set(GEN_SRC_ROOT "${CMAKE_CURRENT_BINARY_DIR}/${ZSR_MODULE_NAME}.zserio-gen/")
  file(REMOVE_RECURSE "${GEN_SRC_ROOT}")
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
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    RESULT_VARIABLE ZSERIO_RESULT)
  if (NOT ZSERIO_RESULT EQUAL "0")
      message(FATAL_ERROR "/////////// zserio FAILED to generate ${ZSR_MODULE_NAME} module! ///////////")
  endif()

  # Glob generated sources per sub-directory
  #
  # E.g. /<root>/<sub>/*
  set(zserio_gen_root "${GEN_SRC_ROOT}/${ZSR_MODULE_TOP_LEVEL_PKG}/")

  # Create single cmake interface library to link all sub-libs
  add_library(${ZSR_MODULE_NAME} INTERFACE)

  # Create root source library
  add_submodule_lib(${ZSR_MODULE_NAME} ${GEN_SRC_ROOT} ${GEN_SRC_ROOT} FALSE)

  # Create sub-libs per directory
  file(GLOB zserio_gen_dirs LIST_DIRECTORIES TRUE "${zserio_gen_root}/*")
  foreach (zserio_gen_subdir IN LISTS zserio_gen_dirs)
    if (IS_DIRECTORY "${zserio_gen_subdir}")
      add_submodule_lib(${ZSR_MODULE_NAME} ${GEN_SRC_ROOT} ${zserio_gen_subdir} TRUE)
    endif()
  endforeach()

  target_include_directories(${ZSR_MODULE_NAME}
    INTERFACE "${GEN_SRC_ROOT}")

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
    PRIVATE
      $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
      $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -fvisibility=hidden>)

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

cmake_minimum_required(VERSION 3.0.2)
project(zserio-cpp-introspection-extension)

find_program(ANT_PATH NAMES ant)
if (NOT ANT_PATH)
  message(FATAL_ERROR "Could not find ant binary!")
endif()

set(JAR_PATH "${CMAKE_CURRENT_BINARY_DIR}/zserio.jar")

execute_process(
  COMMAND ${ANT_PATH} install "-Dzserio_cpp_reflect.install_dir=${CMAKE_CURRENT_BINARY_DIR}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_custom_target(zserio
  DEPENDS "${JAR_PATH}"
  COMMENT "Download and compile zserio C++ introspection extension.")

set_target_properties(zserio
  PROPERTIES
    jar "${JAR_PATH}")

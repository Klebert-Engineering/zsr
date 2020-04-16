cmake_minimum_required(VERSION 3.0.2)

find_program(ANT_PATH NAMES ant)
if (NOT ANT_PATH)
  message(FATAL_ERROR "Could not find ant binary!")
endif()

execute_process(
  COMMAND ${ANT_PATH} install "-Dzserio_cpp_reflect.install_dir=${CMAKE_CURRENT_BINARY_DIR}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

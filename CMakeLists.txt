cmake_minimum_required(VERSION 3.0.2)

if (NOT DEFINED PROJECT_NAME)
  project(zsr)

  set(NOT_SUBPROJECT ON)
  set(ZSR_BUILD_TESTS ON)
  enable_testing()
endif()

add_subdirectory(extension)
add_subdirectory(runtime)

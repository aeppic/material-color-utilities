cmake_minimum_required(VERSION 3.20)

project(material_color_utilities LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(absl CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

file(GLOB_RECURSE MATERIAL_COLOR_SOURCES CONFIGURE_DEPENDS "cpp/*.cc")
list(FILTER MATERIAL_COLOR_SOURCES EXCLUDE REGEX "_test\\.cc$")

add_library(material_color_utilities STATIC ${MATERIAL_COLOR_SOURCES})
target_include_directories(material_color_utilities PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(material_color_utilities PUBLIC absl::flat_hash_map)

include(CTest)
include(GoogleTest)

file(GLOB_RECURSE MATERIAL_COLOR_TEST_SOURCES CONFIGURE_DEPENDS
     "cpp/*_test.cc")

foreach(TEST_SOURCE IN LISTS MATERIAL_COLOR_TEST_SOURCES)
  get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_SOURCE})
  target_link_libraries(${TEST_NAME} PRIVATE material_color_utilities
                                             GTest::gtest_main GTest::gmock)
  gtest_discover_tests(${TEST_NAME})
endforeach()

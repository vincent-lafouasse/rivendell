cmake_minimum_required(VERSION 3.16)

project(
  rivendell
  VERSION 1.0.1
  LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(RIVENDELL_IS_TOP_LEVEL ON)
else()
  set(RIVENDELL_IS_TOP_LEVEL OFF)
endif()

option(RIVENDELL_BUILD_TESTS "Build and run tests using CTest"
       ${RIVENDELL_IS_TOP_LEVEL})

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE src/elf.c)
add_library(rivendell::rivendell ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic
                                               -fdiagnostics-color=always)

# if(RIVENDELL_BUILD_TESTS)
#   enable_language(CXX)
#   set(CMAKE_CXX_STANDARD 20)
#   set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 
#   include(FetchContent)
#   include(cmake/fetch_gtest.cmake)
# 
#   include(CTest)
#   enable_testing()
# 
#   add_subdirectory(test)
# endif()

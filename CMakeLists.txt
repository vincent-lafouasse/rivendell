cmake_minimum_required(VERSION 3.16)

project(
  rivendell
  VERSION 1.0.1
  LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT UNIX)
    message(FATAL_ERROR "raven requires a POSIX system")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(RIVENDELL_IS_TOP_LEVEL ON)
else()
  set(RIVENDELL_IS_TOP_LEVEL OFF)
endif()

option(RIVENDELL_BUILD_TESTS "Build and run tests using CTest"
       ${RIVENDELL_IS_TOP_LEVEL})

add_library(rivendell_flags INTERFACE)
target_compile_options(
  rivendell_flags
  INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wshadow
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Wold-style-definition
    -Wredundant-decls
    -Wformat=2
    -Wcast-qual
    -Wpointer-arith
    -Wwrite-strings
    -Wundef
    -Wvla
    -Wnull-dereference
    -Wdouble-promotion
    "$<$<C_COMPILER_ID:GNU>:-Wduplicated-cond;-Wduplicated-branches;-Wlogical-op;-Wjump-misses-init;-Wcast-align=strict>"
    "$<$<C_COMPILER_ID:Clang,AppleClang>:-Wcast-align>"
    "$<$<C_COMPILER_ID:GNU>:-fdiagnostics-color=always>"
    "$<$<C_COMPILER_ID:Clang,AppleClang>:-fcolor-diagnostics>"
    "$<$<CONFIG:Debug>:-g;-O0;-fno-omit-frame-pointer;-fsanitize=address,undefined;-fno-sanitize-recover=all>"
)
target_link_options(rivendell_flags INTERFACE
                    "$<$<CONFIG:Debug>:-fsanitize=address,undefined>")

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

cmake_minimum_required(VERSION 3.16)

project(
  rivendell
  VERSION 0.0.1
  LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT UNIX)
  message(FATAL_ERROR "rivendell requires a POSIX system")
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
option(RIVENDELL_SANITIZE "Build with ASan and UBSan in Debug"
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
    "$<$<CONFIG:Debug>:-g;-O0;-fno-omit-frame-pointer>")
if(RIVENDELL_SANITIZE)
  target_compile_options(
    rivendell_flags
    INTERFACE
      "$<$<CONFIG:Debug>:-fsanitize=address,undefined;-fno-sanitize-recover=all>"
  )
  target_link_options(rivendell_flags INTERFACE
                      "$<$<CONFIG:Debug>:-fsanitize=address,undefined>")
endif()

include(FetchContent)
FetchContent_Declare(
  bitcast
  GIT_REPOSITORY https://github.com/vincent-lafouasse/libbitcast.git
  GIT_TAG 1.0.1)
FetchContent_MakeAvailable(bitcast)

add_library(rivendell_core OBJECT)
target_sources(rivendell_core PRIVATE src/read.c src/validate.c src/error_str.c)
target_include_directories(rivendell_core PUBLIC include src)
target_link_libraries(rivendell_core PRIVATE rivendell_flags)
target_link_libraries(rivendell_core PRIVATE bitcast::bitcast)

add_library(rivendell)
target_sources(rivendell PRIVATE src/load.c)
add_library(rivendell::rivendell ALIAS rivendell)

target_include_directories(rivendell PUBLIC include)

target_link_libraries(rivendell PUBLIC rivendell_flags)
target_link_libraries(rivendell PRIVATE rivendell_core)

if(RIVENDELL_BUILD_TESTS)
  add_executable(dummy)
  target_sources(dummy PRIVATE test/main.c)
  target_link_libraries(dummy PRIVATE rivendell_flags)
  target_link_libraries(dummy PRIVATE rivendell_core)
  include(CTest)
  enable_testing()
endif()

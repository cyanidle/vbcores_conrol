cmake_minimum_required(VERSION 3.16)

if (DEFINED TURTLEBRO2_SYSROOT)
    include(./toolchain/turtlebro2.cmake)
endif()

project(vbcores_control C CXX)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include(deps/get_cpm.cmake)

set(VERSION_HASH 0)
execute_process(COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE VERSION_HASH
    RESULT_VARIABLE git_res
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ECHO_ERROR_VARIABLE
)

configure_file(include/config.h.in ${CMAKE_BINARY_DIR}/config.h)
include_directories(${CMAKE_BINARY_DIR})

CPMAddPackage("gh:fmtlib/fmt#12.1.0")
CPMAddPackage("gh:p-ranav/argparse@3.1")
find_package(Curses REQUIRED)
find_package(Boost REQUIRED CONFIG)

add_subdirectory(deps/libcxxcanard)
target_include_directories(libcxxcanard INTERFACE
    deps/libcxxcanard
    deps/libcxxcanard/libs
)


add_executable(vb_control
    src/main.cpp
    src/asio.cpp
    src/teleop.cpp
    src/args.cpp
)

target_include_directories(vb_control PRIVATE include)

target_link_libraries(vb_control PRIVATE
    libcxxcanard
    fmt::fmt
    Boost::boost
    ncurses
    argparse::argparse
)
cmake_minimum_required(VERSION 3.16)

project(vbcores_control C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

include(deps/get_cpm.cmake)

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
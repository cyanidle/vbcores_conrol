cmake_minimum_required(VERSION 3.16)

project(vbcores_control C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include(deps/get_cpm.cmake)

CPMAddPackage("gh:fmtlib/fmt#12.1.0")

set(BOOST_INCLUDE_LIBRARIES asio)
CPMAddPackage(NAME Boost
    VERSION 1.89.0
    URL https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-cmake.7z
    URL_HASH SHA256=252149f583054515b10688855b24ca1c16072b5118e603b8fe017894d319c262
)

add_subdirectory(deps/libcxxcanard)
target_include_directories(libcxxcanard INTERFACE
    deps/libcxxcanard
    deps/libcxxcanard/libs
)

find_package(Curses REQUIRED)

add_executable(vb_control
    src/main.cpp
    src/asio.cpp
    src/teleop.cpp
)

target_include_directories(vb_control PRIVATE
    include
    deps/tapki
)

target_link_libraries(vb_control PRIVATE
    libcxxcanard
    fmt::fmt
    Boost::asio
    ncurses
)
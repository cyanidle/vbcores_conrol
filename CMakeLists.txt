cmake_minimum_required(VERSION 3.16)

project(vbcores_control C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS OFF)


include(deps/get_cpm.cmake)

CPMAddPackage("gh:fmtlib/fmt#12.1.0")

add_subdirectory(deps/libcxxcanard)
target_include_directories(libcxxcanard INTERFACE
    deps/libcxxcanard
    deps/libcxxcanard/libs
)


add_executable(vb_control
    src/main.cpp
    src/args.cpp
)

target_include_directories(vb_control PRIVATE
    include
    deps/tapki
)

target_link_libraries(vb_control PRIVATE
    libcxxcanard
    fmt::fmt
)
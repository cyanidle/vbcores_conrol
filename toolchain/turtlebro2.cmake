SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR aarch64)

# Specify the cross compilers
SET(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
SET(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# Specify the sysroot (root of your Raspberry Pi filesystem)
SET(CMAKE_SYSROOT ${TURTLEBRO2_SYSROOT})

# Set the search path for libraries and headers
SET(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})

# Only search in the specified root path
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
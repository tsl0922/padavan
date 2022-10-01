#
# CMake Toolchain file for crosscompiling on mipsel.
#
# This can be used when running cmake in the following way:
#  cd build/
#  cmake .. -DCMAKE_TOOLCHAIN_FILE=$(CONFIG_CMAKE_TOOLCHAIN_FILE)

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:$ENV{STAGEDIR}/lib/pkgconfig")

# Target operating system name.
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR "$ENV{ARCH}")

# Name of C compiler.
set(CMAKE_C_COMPILER "$ENV{CROSS_COMPILE}gcc")
set(CMAKE_CXX_COMPILER "$ENV{CROSS_COMPILE}g++")
set(CMAKE_AR "$ENV{CROSS_COMPILE}ar")
set(CMAKE_NM "$ENV{CROSS_COMPILE}nm")
set(CMAKE_RANLIB "$ENV{CROSS_COMPILE}ranlib")

# Set compiler flags.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} $ENV{CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} $ENV{CXXFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "$ENV{LDFLAGS}")

# Where to look for the target environment. (More paths can be added here)
set(CMAKE_FIND_ROOT_PATH "$ENV{CONFIG_CROSS_COMPILER_ROOT}" "$ENV{STAGEDIR}")

# Adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment only.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search headers and libraries in the target environment only.
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

add_definitions($ENV{CPUFLAGS})

#
# CMake Toolchain file for MIPS (mipsel) cross-compilation
#
# This can be used when running cmake in the following way:
#  cd build/
#  cmake .. -DCMAKE_TOOLCHAIN_FILE=$(CONFIG_CMAKE_TOOLCHAIN_FILE)

# Basic environment extraction
set(CROSS_COMPILE $ENV{CROSS_COMPILE})
set(STAGEDIR $ENV{STAGEDIR})
set(ARCH $ENV{ARCH})
set(CONFIG_CROSS_COMPILER_ROOT $ENV{CONFIG_CROSS_COMPILER_ROOT})
set(CPUFLAGS $ENV{CPUFLAGS})
set(CFLAGS $ENV{CFLAGS})
set(CXXFLAGS $ENV{CXXFLAGS})
set(LDFLAGS $ENV{LDFLAGS})

# Toolchain prefix
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR "${ARCH}")

# Compiler tools
set(CMAKE_C_COMPILER "${CROSS_COMPILE}gcc")
set(CMAKE_CXX_COMPILER "${CROSS_COMPILE}g++")
set(CMAKE_AR "${CROSS_COMPILE}ar")
set(CMAKE_NM "${CROSS_COMPILE}nm")
set(CMAKE_RANLIB "${CROSS_COMPILE}ranlib")

# Compilation flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${LDFLAGS}")

# Target environment paths
set(CMAKE_FIND_ROOT_PATH "${CONFIG_CROSS_COMPILER_ROOT}" "${STAGEDIR}")

# Find modes
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# pkg-config support
set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${STAGEDIR}/lib/pkgconfig")

# Architecture-specific flags
add_definitions(${CPUFLAGS})

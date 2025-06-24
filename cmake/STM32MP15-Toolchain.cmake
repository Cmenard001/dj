# STM32MP15 Toolchain file for Roboteseo cross-compilation environment
# This file configures CMake to use the Roboteseo cross-compilation toolchain
# Based on environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Set the cross-compilation environment from Roboteseo SDK
set(CROSS_COMPILE_PREFIX arm-poky-linux-gnueabi-)

# Set the sysroot from environment variable (set by Roboteseo environment script)
set(CMAKE_SYSROOT $ENV{SDKTARGETSYSROOT})
if(NOT CMAKE_SYSROOT)
    set(CMAKE_SYSROOT /opt/roboteseo/1.0/sysroots/cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi)
endif()

# Set the toolchain base path
set(TOOLCHAIN_BASE_PATH /opt/roboteseo/1.0/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi)

# Set the compiler paths - use full paths to avoid PATH issues
if(DEFINED ENV{CC})
    # Extract just the compiler name from the full CC environment variable
    string(REGEX MATCH "^[^ ]+" CMAKE_C_COMPILER_NAME $ENV{CC})
    set(CMAKE_C_COMPILER ${CMAKE_C_COMPILER_NAME})
else()
    set(CMAKE_C_COMPILER ${TOOLCHAIN_BASE_PATH}/${CROSS_COMPILE_PREFIX}gcc)
endif()

if(DEFINED ENV{CXX})
    # Extract just the compiler name from the full CXX environment variable
    string(REGEX MATCH "^[^ ]+" CMAKE_CXX_COMPILER_NAME $ENV{CXX})
    set(CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER_NAME})
else()
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_BASE_PATH}/${CROSS_COMPILE_PREFIX}g++)
endif()

set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

# Set additional tools from environment or defaults
set(CMAKE_AR $ENV{AR})
set(CMAKE_STRIP $ENV{STRIP})
set(CMAKE_NM $ENV{NM})
set(CMAKE_OBJCOPY $ENV{OBJCOPY})
set(CMAKE_OBJDUMP $ENV{OBJDUMP})
set(CMAKE_RANLIB $ENV{RANLIB})
set(CMAKE_READELF $ENV{READELF})

# Fallback to full paths if environment variables are not set
if(NOT CMAKE_AR)
    set(CMAKE_AR ${TOOLCHAIN_BASE_PATH}/${CROSS_COMPILE_PREFIX}ar)
endif()
if(NOT CMAKE_STRIP)
    set(CMAKE_STRIP ${TOOLCHAIN_BASE_PATH}/${CROSS_COMPILE_PREFIX}strip)
endif()
if(NOT CMAKE_NM)
    set(CMAKE_NM ${TOOLCHAIN_BASE_PATH}/${CROSS_COMPILE_PREFIX}nm)
endif()
if(NOT CMAKE_OBJCOPY)
    set(CMAKE_OBJCOPY ${TOOLCHAIN_BASE_PATH}/${CROSS_COMPILE_PREFIX}objcopy)
endif()
if(NOT CMAKE_OBJDUMP)
    set(CMAKE_OBJDUMP ${TOOLCHAIN_BASE_PATH}/${CROSS_COMPILE_PREFIX}objdump)
endif()
if(NOT CMAKE_RANLIB)
    set(CMAKE_RANLIB ${TOOLCHAIN_BASE_PATH}/${CROSS_COMPILE_PREFIX}ranlib)
endif()

# Set find root path
set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})

# Search for programs only in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers only in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Set PKG_CONFIG variables from environment
set(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_SYSROOT})
set(ENV{PKG_CONFIG_PATH} "${CMAKE_SYSROOT}/usr/lib/pkgconfig:${CMAKE_SYSROOT}/usr/share/pkgconfig")

# Set compiler flags based on Roboteseo environment and ARM toolchain pattern
# From OECORE_TUNE_CCARGS: -mthumb -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7
set(CORE_FLAGS "-mthumb -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7")

# Base compiler flags (similar to ARM toolchain but adapted for Linux target)
# compiler: language specific flags
set(CMAKE_C_FLAGS "${CORE_FLAGS} -fno-builtin -Wall -std=gnu99 -fdata-sections -ffunction-sections -g3 -gdwarf-2" CACHE INTERNAL "c compiler flags")
set(CMAKE_C_FLAGS_DEBUG "" CACHE INTERNAL "c compiler flags: Debug")
set(CMAKE_C_FLAGS_RELEASE "" CACHE INTERNAL "c compiler flags: Release")

set(CMAKE_CXX_FLAGS "${CORE_FLAGS} -fno-rtti -fno-exceptions -fno-builtin -Wall -std=gnu++11 -fdata-sections -ffunction-sections -g -ggdb3" CACHE INTERNAL "cxx compiler flags")
set(CMAKE_CXX_FLAGS_DEBUG "" CACHE INTERNAL "cxx compiler flags: Debug")
set(CMAKE_CXX_FLAGS_RELEASE "" CACHE INTERNAL "cxx compiler flags: Release")

set(CMAKE_ASM_FLAGS "${CORE_FLAGS} -g -ggdb3 -D__USES_CXX" CACHE INTERNAL "asm compiler flags")
set(CMAKE_ASM_FLAGS_DEBUG "" CACHE INTERNAL "asm compiler flags: Debug")
set(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "asm compiler flags: Release")

# Set linker flags from Roboteseo environment
# From LDFLAGS: -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed -Wl,-z,relro,-z,now  
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed -Wl,-z,relro,-z,now" CACHE INTERNAL "exe linker flags")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed -Wl,-z,relro,-z,now" CACHE INTERNAL "shared linker flags")

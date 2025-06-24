# the name of the target operating system
set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_VERSION   1)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# find full paths to toolchain executables
find_program(ARM_GCC_PATH arm-none-eabi-gcc)
find_program(ARM_GPP_PATH arm-none-eabi-g++)
find_program(ARM_ASM_PATH arm-none-eabi-gcc)
find_program(ARM_OBJCOPY_PATH arm-none-eabi-objcopy)
find_program(ARM_OBJDUMP_PATH arm-none-eabi-objdump)

# set compilers to the found paths
set(CMAKE_C_COMPILER ${ARM_GCC_PATH})
set(CMAKE_CXX_COMPILER ${ARM_GPP_PATH})
set(CMAKE_ASM_COMPILER ${ARM_ASM_PATH})
set(CMAKE_OBJCOPY ${ARM_OBJCOPY_PATH})
set(CMAKE_OBJDUMP ${ARM_OBJDUMP_PATH})

message(STATUS "C Compiler path: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler path: ${CMAKE_CXX_COMPILER}")
message(STATUS "ASM Compiler path: ${CMAKE_ASM_COMPILER}")
message(STATUS "Objcopy path: ${CMAKE_OBJCOPY}")
message(STATUS "Objdump path: ${CMAKE_OBJDUMP}")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# core flags
set(CORE_FLAGS "-mthumb -mcpu=cortex-m4 -mlittle-endian -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb-interwork --specs=nano.specs --specs=nosys.specs ${ADDITIONAL_CORE_FLAGS}")

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

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# find additional toolchain executables
find_program(ARM_SIZE_EXECUTABLE arm-none-eabi-size)
find_program(ARM_GDB_EXECUTABLE arm-none-eabi-gdb)
find_program(ARM_OBJCOPY_EXECUTABLE arm-none-eabi-objcopy)
find_program(ARM_OBJDUMP_EXECUTABLE arm-none-eabi-objdump)

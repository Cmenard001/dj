# set search path for CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

# include OpenOCD module, but make it not required
find_package(Openocd)

# STM32-specific settings
set(DEVICE_FAMILY STM32F40_41xxx)

# Speed of external resonator
set(HSE_VALUE 8000000)

# Used linker file (just for testing)
get_filename_component(LINKER_SCRIPT linker/stm32f407vgt.ld ABSOLUTE)

# Use LTO? WARNING: This might break the code
#set(ADDITIONAL_CORE_FLAGS -flto)

# OpenOCD config file
get_filename_component(OPENOCD_CONFIG tools/stm32f4discovery.cfg ABSOLUTE)

# add libraries
add_subdirectory(src/api/stm32f4)

add_sources(
    api/stm32f4/startup_stm32f40_41xxx.s
    api/stm32f4/system_stm32f4xx.c
    api/time/timeStm32f4.c
    api/stm32f4/syscalls.c
    api/printf/printfStm32f4.c
    )

# GCC optimization level: use -O0 in debug build, otherwise -O2
if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(opt_level -O0)
else()
  set(opt_level -Os)
endif()

# set some project constants
set(elf_file ${PROJECT_NAME}.elf)
set(bin_file ${PROJECT_NAME}.bin)
set(hex_file ${PROJECT_NAME}.hex)
set(map_file ${PROJECT_NAME}.map)
set(lss_file ${PROJECT_NAME}.lss)
set(PROJECT_NAME ${elf_file})

add_executable(${PROJECT_NAME} ${SRCS})

# link StdPeriph library to project
target_link_libraries(${PROJECT_NAME} PUBLIC stm32f4xx m)

# set additional for compiler and linker: optimization and generate map file
set(additional_compiler_flags ${opt_level})
set(additional_linker_flags -Wl,-Map=${map_file},--cref,--no-warn-mismatch)
target_compile_options(${PROJECT_NAME} PRIVATE ${additional_compiler_flags})
target_link_libraries(${PROJECT_NAME} PRIVATE ${additional_linker_flags})

# remove unused sections
target_link_libraries(${PROJECT_NAME} PUBLIC "-g -Wl,--gc-sections")

# link with linker file
target_link_libraries(${PROJECT_NAME} PUBLIC -T${LINKER_SCRIPT})

# show size of resulting firmware image
add_custom_target(${PROJECT_NAME}-size DEPENDS ${PROJECT_NAME} COMMAND ${ARM_SIZE_EXECUTABLE} -B ${PROJECT_NAME})

# generate extended listing
add_custom_target(${lss_file} DEPENDS ${PROJECT_NAME} COMMAND ${ARM_OBJDUMP_EXECUTABLE} -S ${PROJECT_NAME} > ${lss_file})

# create binary and hex files
add_custom_target(${hex_file} DEPENDS ${PROJECT_NAME} COMMAND ${ARM_OBJCOPY_EXECUTABLE} -Oihex ${PROJECT_NAME} ${hex_file})
add_custom_target(${bin_file} DEPENDS ${PROJECT_NAME} COMMAND ${ARM_OBJCOPY_EXECUTABLE} -Obinary ${PROJECT_NAME} ${bin_file})
add_custom_target(${application_name} ALL DEPENDS ${PROJECT_NAME}-size ${bin_file} ${hex_file} ${lss_file})

# OpenOCD targets
add_custom_target(flash DEPENDS ${PROJECT_NAME} COMMAND ${OPENOCD_EXECUTABLE}
  -f ${OPENOCD_CONFIG}
  --command "program ${PROJECT_NAME} reset exit")

# The port that OpenOCD will serve a gdb interface on
if(NOT GDB_DEBUG_PORT)
  set(GDB_DEBUG_PORT "4242")
endif()

# Connect OCD to the device and serve for an incoming GDB connection
add_custom_target(ocd_serve DEPENDS ${PROJECT_NAME} COMMAND ${OPENOCD_EXECUTABLE}
  -f ${OPENOCD_CONFIG}
  --command "gdb_port ${GDB_DEBUG_PORT}")

# Run GDB against the target on the device, via OpenOCD
add_custom_target(gdb_connect DEPENDS ${PROJECT_NAME} COMMAND ${ARM_GDB_EXECUTABLE}
  -ex "target remote localhost:${GDB_DEBUG_PORT}"
  -ex "monitor reset halt"
  ${PROJECT_NAME})

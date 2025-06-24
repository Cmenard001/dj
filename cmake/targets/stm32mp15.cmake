# STM32MP15 target configuration
# Uses the Roboteseo toolchain for STM32MP15 (Cortex-A7)

# Specific STM32MP15 source files  
add_sources(
    api/time/timeLinux.c
    api/printf/printfEmpty.c
    )

add_executable(${PROJECT_NAME} ${SRCS})

# Enable all warnings and optimization for embedded target
target_compile_options(${PROJECT_NAME} PRIVATE 
    -Wall
    -Wextra
    -ffunction-sections
    -fdata-sections
)

# Link libraries and set linker options
target_link_libraries(${PROJECT_NAME}
    m
    pthread
)

# Linker options for STM32MP15
target_link_options(${PROJECT_NAME} PRIVATE
    -Wl,--gc-sections
    -Wl,--print-memory-usage
)

# Set properties for cross-compilation
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

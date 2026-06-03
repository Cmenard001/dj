# STM32MP15 target configuration
# Uses the Roboteseo toolchain for STM32MP15 (Cortex-A7)

# STM32MP15 executable: main entry point + platform implementations, linked against dj_core
add_executable(${PROJECT_NAME}
    ${SRC_PREFIX}/main.c
    ${SRC_PREFIX}/api/time/timeLinux.c
    ${SRC_PREFIX}/api/printf/printfEmpty.c
)

# Enable all warnings and optimization for embedded target
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -ffunction-sections
    -fdata-sections
)

# Link libraries and set linker options
target_link_libraries(${PROJECT_NAME}
    dj_core
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

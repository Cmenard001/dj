# Specific windows source files
add_sources(
    api/time/timeWindows.c
    api/printf/printfEmpty.c
    )

add_executable(${PROJECT_NAME} ${SRCS})

# Enable all warnings
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    m
)

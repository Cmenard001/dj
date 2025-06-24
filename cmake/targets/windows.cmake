
# Specific windows source files
add_sources(
    api/time/timeWindows.c
    api/printf/printfEmpty.c
    )

add_executable(${PROJECT_NAME} ${SRCS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    m
)

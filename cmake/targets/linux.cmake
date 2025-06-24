
# Specific linux source files
add_sources(
    api/time/timeLinux.c
    api/printf/printfEmpty.c
    )

add_executable(${PROJECT_NAME} ${SRCS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    m
)

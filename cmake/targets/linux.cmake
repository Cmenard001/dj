# Linux executable: main entry point + platform implementations, linked against dj_core
add_executable(${PROJECT_NAME}
    ${SRC_PREFIX}/main.c
    ${SRC_PREFIX}/api/time/timeLinux.c
    ${SRC_PREFIX}/api/printf/printfEmpty.c
)

# Enable all warnings
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE dj_core m)

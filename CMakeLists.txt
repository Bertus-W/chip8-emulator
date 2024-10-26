cmake_minimum_required(VERSION 3.15)

# Set your project name and specify C++ as the language
project(Chip8-Emulator LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the raylib package
find_package(raylib REQUIRED)

# Create an executable from your source files
add_executable(${PROJECT_NAME} src/main.cpp)

# Link raylib to your executable
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Platform-specific settings
if(APPLE)
    # Link macOS specific frameworks required by Raylib
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
elseif(UNIX AND NOT APPLE)
    # Linux specific libraries
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RAYLIB REQUIRED raylib)
    pkg_check_modules(GL REQUIRED gl)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GL_LIBRARIES} m pthread)
elseif(WIN32)
    # Windows specific settings
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
endif()

# Include directories from Raylib
target_include_directories(${PROJECT_NAME} PRIVATE ${RAYLIB_INCLUDE_DIRS})

# Optionally, you can add some extra options for warnings or compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE -std=c++11 -Wall -Wextra -pedantic)

# Copy assets and scripts folders to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/scripts" "${CMAKE_BINARY_DIR}/scripts"
)

# Additional message to help with debugging paths
message(STATUS "Raylib include directory: ${RAYLIB_INCLUDE_DIRS}")
message(STATUS "Raylib libraries: ${RAYLIB_LIBRARIES}")


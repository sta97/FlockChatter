cmake_minimum_required(VERSION 3.10)

# Project name
project(FlockChatter)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable
add_executable(FlockChatter 
    src/main.cpp 
    src/networking/networking.cpp
    src/http/http.cpp
)

add_custom_command(TARGET FlockChatter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/assets
)
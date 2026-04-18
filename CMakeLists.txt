cmake_minimum_required(VERSION 3.10)

# Project name
project(FlockChatter)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SODIUM_ROOT "" CACHE PATH "Path to libsodium root directory")

add_library(sodium SHARED IMPORTED)
set_target_properties(sodium PROPERTIES
    IMPORTED_IMPLIB "${SODIUM_ROOT}/x64/Release/v143/dynamic/libsodium.lib"
    IMPORTED_LOCATION "${SODIUM_ROOT}/x64/Release/v143/dynamic/libsodium.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${SODIUM_ROOT}/include"
)

# Add executable
add_executable(FlockChatter 
    src/main.cpp 
    src/networking/networking.cpp
    src/http/http.cpp
    src/login/login.cpp
)

add_custom_command(TARGET FlockChatter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/assets
)

target_link_libraries(FlockChatter PRIVATE sodium)

add_custom_command(TARGET FlockChatter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SODIUM_ROOT}/x64/Release/v143/dynamic/libsodium.dll"
        ${CMAKE_BINARY_DIR}
)

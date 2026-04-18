cmake_minimum_required(VERSION 3.10)
project(FlockChatter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(unofficial-sodium CONFIG REQUIRED)

add_executable(FlockChatter
    src/main.cpp
    src/networking/networking.cpp
    src/http/http.cpp
    src/login/login.cpp
)

target_link_libraries(FlockChatter PRIVATE unofficial-sodium::sodium)

add_custom_command(TARGET FlockChatter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/assets
)
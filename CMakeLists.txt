cmake_minimum_required(VERSION 3.10)
project(FlockChatter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



add_executable(FlockChatter
    src/main.cpp
    src/networking/networking.cpp
    src/http/http.cpp
    src/login/login.cpp
)

if(WIN32)
    find_package(unofficial-sodium CONFIG QUIET)
    target_link_libraries(FlockChatter PRIVATE unofficial-sodium::sodium)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SODIUM REQUIRED libsodium)
    target_include_directories(FlockChatter PRIVATE ${SODIUM_INCLUDE_DIRS})
    target_link_libraries(FlockChatter PRIVATE ${SODIUM_LIBRARIES})
endif()

add_custom_command(TARGET FlockChatter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/assets
)
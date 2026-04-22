cmake_minimum_required(VERSION 3.10)
project(FlockChatter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(FlockChatterClient
    src/mainClient.cpp
    src/networking/networking.cpp
    src/login/login.cpp
)

add_executable(FlockChatterServer
    src/mainServer.cpp
    src/networking/networking.cpp
    src/login/login.cpp
    src/server/server.cpp
)

if(WIN32)
    find_package(unofficial-sodium CONFIG QUIET)
    target_link_libraries(FlockChatterClient PRIVATE unofficial-sodium::sodium)
    target_link_libraries(FlockChatterServer PRIVATE unofficial-sodium::sodium)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SODIUM REQUIRED libsodium)
    target_include_directories(FlockChatterClient PRIVATE ${SODIUM_INCLUDE_DIRS})
    target_link_libraries(FlockChatterClient PRIVATE ${SODIUM_LIBRARIES})
    target_include_directories(FlockChatterServer PRIVATE ${SODIUM_INCLUDE_DIRS})
    target_link_libraries(FlockChatterServer PRIVATE ${SODIUM_LIBRARIES})
endif()

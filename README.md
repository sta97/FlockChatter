# FlockChatter
C++ self-hosted Discord alternative, targeting Linux and Windows as server host environments.

# Planned features
- Text chat
- Voice Chat
- Video Chat
- Encrypted communication between client and server
- Owner/Admin/User permission levels
- Raspberry Pi compatible 
- Minimal dependencies
- Minimal setup required

# cmake setup

If on Windows install libsodium with vcpkg and run `cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/Users/<USER>/vcpkg/scripts/buildsystems/vcpkg.cmake`

If on Ubuntu Linux, install pkg-config and libsodium-dev with `sudo apt install pkg-config libsodium-dev cmake g++` then run `cmake -B build -S .` then `cmake --build build`

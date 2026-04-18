# FlockChatter
C++ self-hosted Discord alternative, targeting Linux and Windows as server host environments.

# Planned features
- Web UI
- Text chat
- Voice Chat
- Video Chat
- Https support
- Owner/Admin/User permission levels
- Minimal dependencies
- Raspberry Pi compatible 
- Minimal setup required

# cmake setup

If on Windows install libsodium with vcpkg and run `cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/Users/<USER>/vcpkg/scripts/buildsystems/vcpkg.cmake`

If on Ubuntu Linux, install pkg-config and libsodium-dev with `sudo apt install pkg-config libsodium-dev` then run `cmake -B build -S .`
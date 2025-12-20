# cmake/toolchains/clang-libc++.cmake
#
# Usage:
#   cmake -B build -S . \
#         -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/clang-libc++.cmake \
#         -DCMAKE_BUILD_TYPE=Release
#
# This config:
#   - Uses clang / clang++
#   - Links against libc++ / libc++abi
#   - Enables modern warnings and tuning flags

set(CMAKE_SYSTEM_NAME Linux)

# Compiler selection
set(CMAKE_C_COMPILER   clang)
set(CMAKE_CXX_COMPILER clang++)

# Use libc++ by default
set(CMAKE_CXX_FLAGS_INIT
    "-stdlib=libc++ -fPIC"
)

# Linker flags for libc++
set(CMAKE_EXE_LINKER_FLAGS_INIT
    "-stdlib=libc++ -lc++abi"
)

# Prefer LLD if available (faster links, good for big projects)
set(CLANG_LLD_FLAGS "-fuse-ld=lld")

# Append flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_LLD_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CLANG_LLD_FLAGS}")

# Additional tuning flags (optional; can also be handled in main CMakeLists)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG -march=native -fno-exceptions -fno-rtti")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")

# Warnings (you can still add more in main CMakeLists if desired)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wpedantic")

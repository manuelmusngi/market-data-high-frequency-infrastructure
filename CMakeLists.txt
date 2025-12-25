cmake_minimum_required(VERSION 3.23)

# ------------------------------------------------------------------------------
# Project metadata
# ------------------------------------------------------------------------------
project(market-data-parser
    VERSION 0.1.0
    LANGUAGES CXX
)

# ------------------------------------------------------------------------------
# Global configuration
# ------------------------------------------------------------------------------

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(MDP_BUILD_TESTS         "Build unit tests"                         ON)
option(MDP_BUILD_BENCHMARKS    "Build benchmark executables"             ON)
option(MDP_ENABLE_LTO          "Enable link-time optimization (LTO)"     OFF)
option(MDP_ENABLE_SANITIZERS   "Enable sanitizers in Debug builds"       OFF)
option(MDP_ENABLE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# Default to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Using C++ standard: ${CMAKE_CXX_STANDARD}")

# ------------------------------------------------------------------------------
# Compiler flags
# ------------------------------------------------------------------------------

# Base flags (compiler-agnostic)
set(MDP_BASE_FLAGS
    -Wall
    -Wextra
    -Wshadow
    -Wconversion
    -Wpedantic
)

if(MDP_ENABLE_WARNINGS_AS_ERRORS)
    list(APPEND MDP_BASE_FLAGS -Werror)
endif()

# Optimization flags per configuration
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    # Common optimization flags
    set(MDP_OPT_FLAGS_RELEASE
        -O3
        -DNDEBUG
        -march=native
        -fno-exceptions
        -fno-rtti
    )

    set(MDP_OPT_FLAGS_DEBUG
        -O0
        -g
    )

    # Apply configuration-specific flags
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(${MDP_OPT_FLAGS_RELEASE})
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        add_compile_options(-O3 -g -DNDEBUG)
    else()
        add_compile_options(${MDP_OPT_FLAGS_DEBUG})
    endif()

    # Base warnings
    add_compile_options(${MDP_BASE_FLAGS})
endif()

# Sanitizers (Debug only, and not for MSVC)
if(MDP_ENABLE_SANITIZERS AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU"))
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Enabling Address + Undefined sanitizers")
        add_compile_options(-fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# LTO
if(MDP_ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_output)
    if(ipo_supported)
        message(STATUS "IPO / LTO enabled")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    else()
        message(WARNING "IPO / LTO not supported: ${ipo_output}")
    endif()
endif()

# ------------------------------------------------------------------------------
# Output directories
# ------------------------------------------------------------------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ------------------------------------------------------------------------------
# Include directories
# ------------------------------------------------------------------------------

# Public include directory
set(MDP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${MDP_INCLUDE_DIR})

# ------------------------------------------------------------------------------
# Source files
# ------------------------------------------------------------------------------

# Core library sources
set(MDP_SRC
    src/core.cpp
    src/net_sock.cpp
    src/net_af_xdp.cpp
    src/net_dpdk.cpp
    src/parse_itch.cpp
    src/parse_fema.cpp
    src/dispatch.cpp
    src/time.cpp
    src/metrics.cpp
)

set(MDP_HEADERS
    include/mdp/core.hpp
    include/mdp/net.hpp
    include/mdp/ring.hpp
    include/mdp/pool.hpp
    include/mdp/time.hpp
    include/mdp/parse.hpp
    include/mdp/models.hpp
    include/mdp/dispatch.hpp
    include/mdp/metrics.hpp
    include/mdp/pinning.hpp
)

# ------------------------------------------------------------------------------
# Main executable
# ------------------------------------------------------------------------------

add_executable(mdp
    src/main.cpp
    ${MDP_SRC}
    ${MDP_HEADERS}
)

target_include_directories(mdp
    PRIVATE
        ${MDP_INCLUDE_DIR}
)

# Platform-specific libs
if(UNIX AND NOT APPLE)
    target_link_libraries(mdp PRIVATE pthread rt)
elseif(WIN32)
    # Add Windows-specific libs here if needed
endif()

# ------------------------------------------------------------------------------
# Tools
# ------------------------------------------------------------------------------

add_executable(mdp_replayer
    tools/replayer.cpp
    ${MDP_SRC}
)

target_include_directories(mdp_replayer PRIVATE ${MDP_INCLUDE_DIR})
target_link_libraries(mdp_replayer PRIVATE pthread rt)

add_executable(mdp_pcap_to_ring
    tools/pcap_to_ring.cpp
    ${MDP_SRC}
)

target_include_directories(mdp_pcap_to_ring PRIVATE ${MDP_INCLUDE_DIR})
target_link_libraries(mdp_pcap_to_ring PRIVATE pthread rt)

add_executable(mdp_gen_synth_feed
    tools/gen_synth_feed.cpp
    ${MDP_SRC}
)

target_include_directories(mdp_gen_synth_feed PRIVATE ${MDP_INCLUDE_DIR})
target_link_libraries(mdp_gen_synth_feed PRIVATE pthread rt)

# ------------------------------------------------------------------------------
# Benchmarks
# ------------------------------------------------------------------------------

if(MDP_BUILD_BENCHMARKS)
    add_executable(mdp_bench_throughput
        bench/throughput.cpp
        ${MDP_SRC}
    )
    target_include_directories(mdp_bench_throughput PRIVATE ${MDP_INCLUDE_DIR})
    target_link_libraries(mdp_bench_throughput PRIVATE pthread rt)

    add_executable(mdp_bench_latency
        bench/latency.cpp
        ${MDP_SRC}
    )
    target_include_directories(mdp_bench_latency PRIVATE ${MDP_INCLUDE_DIR})
    target_link_libraries(mdp_bench_latency PRIVATE pthread rt)

    add_executable(mdp_bench_micro_parse
        bench/micro_parse.cpp
        ${MDP_SRC}
    )
    target_include_directories(mdp_bench_micro_parse PRIVATE ${MDP_INCLUDE_DIR})
    target_link_libraries(mdp_bench_micro_parse PRIVATE pthread rt)
endif()

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------

if(MDP_BUILD_TESTS)
    enable_testing()

    # You can wire in your preferred test framework here (e.g., Catch2, GoogleTest).
    # For now, we'll just build some simple test executables.

    add_executable(mdp_test_ring
        tests/unit_ring.cpp
        ${MDP_SRC}
    )
    target_include_directories(mdp_test_ring PRIVATE ${MDP_INCLUDE_DIR})
    target_link_libraries(mdp_test_ring PRIVATE pthread rt)
    add_test(NAME ring_tests COMMAND mdp_test_ring)

    add_executable(mdp_test_pool
        tests/unit_pool.cpp
        ${MDP_SRC}
    )
    target_include_directories(mdp_test_pool PRIVATE ${MDP_INCLUDE_DIR})
    target_link_libraries(mdp_test_pool PRIVATE pthread rt)
    add_test(NAME pool_tests COMMAND mdp_test_pool)

    add_executable(mdp_test_parse
        tests/unit_parse.cpp
        ${MDP_SRC}
    )
    target_include_directories(mdp_test_parse PRIVATE ${MDP_INCLUDE_DIR})
    target_link_libraries(mdp_test_parse PRIVATE pthread rt)
    add_test(NAME parse_tests COMMAND mdp_test_parse)

    add_executable(mdp_fuzz_parse
        tests/fuzz_parse.cpp
        ${MDP_SRC}
    )
    target_include_directories(mdp_fuzz_parse PRIVATE ${MDP_INCLUDE_DIR})
    target_link_libraries(mdp_fuzz_parse PRIVATE pthread rt)
endif()

# ------------------------------------------------------------------------------
# Installation (optional, for later)
# ------------------------------------------------------------------------------

# install(TARGETS mdp
#     RUNTIME DESTINATION bin
# )
# install(DIRECTORY include/ DESTINATION include)

# ------------------------------------------------------------------------------
# Summary
# ------------------------------------------------------------------------------

message(STATUS "MDP_BUILD_TESTS         = ${MDP_BUILD_TESTS}")
message(STATUS "MDP_BUILD_BENCHMARKS    = ${MDP_BUILD_BENCHMARKS}")
message(STATUS "MDP_ENABLE_LTO          = ${MDP_ENABLE_LTO}")
message(STATUS "MDP_ENABLE_SANITIZERS   = ${MDP_ENABLE_SANITIZERS}")
message(STATUS "MDP_ENABLE_WARNINGS_AS_ERRORS = ${MDP_ENABLE_WARNINGS_AS_ERRORS}")

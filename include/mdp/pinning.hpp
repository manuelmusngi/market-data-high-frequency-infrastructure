#pragma once

// ============================================================================
// CPU Pinning & NUMA Awareness Utilities
// ----------------------------------------------------------------------------
// This header provides simple, low-overhead helpers for:
//   - Pinning threads to specific CPU cores
//   - Querying available CPUs
//   - Optional NUMA node awareness (stubbed for portability)
//
// Why this matters:
//   In ultra-low-latency systems, keeping a thread on a fixed CPU core
//   eliminates scheduler jitter, improves cache locality, and stabilizes
//   nanosecond-level performance.
// ============================================================================

#include <thread>
#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>

#if defined(__linux__)
    #include <pthread.h>
    #include <sched.h>
    #include <unistd.h>
#endif

namespace mdp::pin {

// ----------------------------------------------------------------------------
// Convert a vector<int> to a readable string (for logging/debugging)
// ----------------------------------------------------------------------------
inline std::string to_string(const std::vector<int>& v) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        oss << v[i];
        if (i + 1 < v.size()) oss << ", ";
    }
    oss << "]";
    return oss.str();
}

// ----------------------------------------------------------------------------
// Pin the *current thread* to a specific CPU core
// ----------------------------------------------------------------------------
inline void pin_this(int cpu_id) {
#if defined(__linux__)
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);

    pthread_t thread = pthread_self();
    int rc = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);

    if (rc != 0) {
        throw std::runtime_error("Failed to pin thread to CPU " + std::to_string(cpu_id));
    }
#else
    (void)cpu_id; // unused
    // On non-Linux systems, CPU pinning is not implemented.
#endif
}

// ----------------------------------------------------------------------------
// Pin a std::thread object (before or after starting it)
// ----------------------------------------------------------------------------
inline void pin_thread(std::thread& t, int cpu_id) {
#if defined(__linux__)
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);

    int rc = pthread_setaffinity_np(t.native_handle(), sizeof(cpu_set_t), &cpuset);
    if (rc != 0) {
        throw std::runtime_error("Failed to pin std::thread to CPU " + std::to_string(cpu_id));
    }
#else
    (void)t;
    (void)cpu_id;
#endif
}

// ----------------------------------------------------------------------------
// Query number of available CPU cores
// ----------------------------------------------------------------------------
inline int cpu_count() {
#if defined(__linux__)
    long n = sysconf(_SC_NPROCESSORS_ONLN);
    return (n > 0) ? static_cast<int>(n) : 1;
#else
    return std::thread::hardware_concurrency();
#endif
}

// ----------------------------------------------------------------------------
// NUMA Awareness (stubbed for portability)
// ----------------------------------------------------------------------------
// In real HFT systems, you'd integrate libnuma or hwloc here.
// We provide stubs so the API is stable even without those libs.
// ----------------------------------------------------------------------------

inline int numa_node_for_cpu(int /*cpu_id*/) {
    // Stub: always return node 0
    return 0;
}

inline int numa_node_count() {
    // Stub: single NUMA node
    return 1;
}

// ----------------------------------------------------------------------------
// Pin thread to a NUMA node (round-robin across CPUs in that node)
// ----------------------------------------------------------------------------
inline void pin_to_numa_node(int node_id) {
    // Stub: treat node 0 as "all CPUs"
    if (node_id != 0) {
        throw std::runtime_error("NUMA node pinning not implemented on this platform");
    }
    // In a real implementation, you'd map node → CPU list and pick one.
}

} // namespace mdp::pin

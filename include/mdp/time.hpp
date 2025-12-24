#pragma once
#include <atomic>
#include <chrono>
#include <x86intrin.h>

namespace mdp::time {
inline uint64_t rdtsc() { return __rdtsc(); }

struct TscClock {
  std::atomic<double> ns_per_tick{0.0};
  void calibrate() {
    auto t0 = std::chrono::steady_clock::now();
    auto c0 = rdtsc();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto t1 = std::chrono::steady_clock::now();
    auto c1 = rdtsc();
    double ns = double(std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
    ns_per_tick.store(ns / double(c1 - c0), std::memory_order_release);
  }
  uint64_t tsc_ns() const {
    return static_cast<uint64_t>(rdtsc() * ns_per_tick.load(std::memory_order_acquire));
  }
};

inline TscClock& clock() { static TscClock c; return c; }
inline uint64_t tsc_ns() { return clock().tsc_ns(); }
} // namespace mdp::time

#include "mdp/metrics.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>

namespace mdp {

void Metrics::log_periodically(double interval_sec, std::atomic<bool>& running) {
    using namespace std::chrono;
    auto interval = duration<double>(interval_sec);
    while (running.load(std::memory_order_relaxed)) {
        auto rx  = rx_pkts.load(std::memory_order_relaxed);
        auto rd  = rx_drops.load(std::memory_order_relaxed);
        auto ok  = parse_ok.load(std::memory_order_relaxed);
        auto bad = parse_fail.load(std::memory_order_relaxed);

        std::cout << "[metrics] rx=" << rx
                  << " drops=" << rd
                  << " parse_ok=" << ok
                  << " parse_fail=" << bad
                  << "\n";

        std::this_thread::sleep_for(interval);
    }
}

} // namespace mdp

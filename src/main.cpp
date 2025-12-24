#include "mdp/core.hpp"
#include <iostream>
#include <csignal>
#include <atomic>

using namespace mdp;

namespace {
std::atomic<bool> g_running{true};

void handle_signal(int) {
    g_running.store(false, std::memory_order_relaxed);
}
} // namespace

int main() {
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    AppConfig cfg{
        .feed_type       = "ITCH",
        .net_mode        = "SOCK",  // change to "AF_XDP" / "DPDK" when implemented
        .rx_threads      = 2,
        .parse_threads   = 4,
        .consume_threads = 2,
        .cpu_affinity    = {0, 1, 2, 3, 4, 5, 6, 7}
    };

    try {
        App app(cfg);
        app.start();
        std::cout << "Market data parser running. Press Ctrl+C to exit.\n";

        while (g_running.load(std::memory_order_relaxed)) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::cout << "Shutting down...\n";
        app.stop();
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}

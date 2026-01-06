#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>

#include "mdp/ring.hpp"
#include "mdp/parse.hpp"
#include "mdp/time.hpp"

using namespace mdp;

int main() {
    constexpr size_t samples = 1'000'000;
    constexpr size_t ring_size = 1 << 16;

    Ring<PacketView> rx_ring(ring_size);
    Ring<uint64_t> latency_ring(ring_size);

    std::atomic<bool> running{true};

    // Producer
    std::thread producer([&] {
        std::vector<uint8_t> pkt(32);
        pkt[0] = 'A';
        while (running.load()) {
            uint64_t ts = time::tsc_ns();
            rx_ring.try_push({pkt.data(), static_cast<uint32_t>(pkt.size()), ts});
        }
    });

    // Parser + consumer
    std::thread consumer([&] {
        parse::ParseResult pr{};
        while (latency_ring.size() < samples) {
            auto pkt = rx_ring.try_pop();
            if (!pkt) continue;
            if (parse::parse_itch(std::span(pkt->data, pkt->len), pr)) {
                uint64_t now = time::tsc_ns();
                latency_ring.try_push(now - pkt->ts_ns);
            }
        }
        running.store(false);
    });

    producer.join();
    consumer.join();

    std::vector<uint64_t> latencies;
    while (auto l = latency_ring.try_pop()) {
        latencies.push_back(*l);
    }

    std::sort(latencies.begin(), latencies.end());

    auto p50 = latencies[latencies.size() * 50 / 100];
    auto p99 = latencies[latencies.size() * 99 / 100];
    auto max = latencies.back();

    std::cout << "Latency Benchmark (ns)\n";
    std::cout << "Samples: " << latencies.size() << "\n";
    std::cout << "P50: " << p50 << "\n";
    std::cout << "P99: " << p99 << "\n";
    std::cout << "Max: " << max << "\n";

    return 0;
}

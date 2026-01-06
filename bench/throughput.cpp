#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>

#include "mdp/ring.hpp"
#include "mdp/parse.hpp"
#include "mdp/time.hpp"

using namespace mdp;

int main() {
    constexpr size_t duration_sec = 5;
    constexpr size_t ring_size = 1 << 16;

    Ring<PacketView> rx_ring(ring_size);
    Ring<MessageView> msg_ring(ring_size);

    std::atomic<bool> running{true};
    std::atomic<uint64_t> parsed{0};

    // Producer: synthetic packets
    std::thread producer([&] {
        std::vector<uint8_t> pkt(32);
        pkt[0] = 'A';
        while (running.load()) {
            PacketView pv{pkt.data(), static_cast<uint32_t>(pkt.size()), time::tsc_ns()};
            rx_ring.try_push(pv);
        }
    });

    // Parser
    std::thread parser([&] {
        parse::ParseResult pr{};
        while (running.load()) {
            auto pkt = rx_ring.try_pop();
            if (!pkt) continue;
            if (parse::parse_itch(std::span(pkt->data, pkt->len), pr)) {
                msg_ring.try_push({pr.type, pr.msg_ptr});
                parsed.fetch_add(1, std::memory_order_relaxed);
            }
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(duration_sec));
    running.store(false);

    producer.join();
    parser.join();

    double mps = static_cast<double>(parsed.load()) / duration_sec;

    std::cout << "Throughput Benchmark\n";
    std::cout << "Duration (s): " << duration_sec << "\n";
    std::cout << "Messages parsed: " << parsed.load() << "\n";
    std::cout << "Messages/sec: " << mps << "\n";

    return 0;
}

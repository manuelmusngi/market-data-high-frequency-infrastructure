#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdint>
#include <cstring>

#include "mdp/time.hpp"

struct ReplayPacket {
    uint64_t ts_ns;
    uint32_t len;
    uint8_t  data[2048];
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: mdp_replayer <binary_capture_file>\n";
        return 1;
    }

    std::ifstream in(argv[1], std::ios::binary);
    if (!in) {
        std::cerr << "Failed to open capture file\n";
        return 1;
    }

    std::vector<ReplayPacket> packets;
    ReplayPacket pkt{};

    while (in.read(reinterpret_cast<char*>(&pkt), sizeof(pkt))) {
        packets.push_back(pkt);
    }

    std::cout << "Loaded " << packets.size() << " packets\n";

    if (packets.empty()) return 0;

    uint64_t start_ts = packets.front().ts_ns;
    uint64_t replay_start = mdp::time::tsc_ns();

    for (const auto& p : packets) {
        uint64_t target_ns = replay_start + (p.ts_ns - start_ts);
        while (mdp::time::tsc_ns() < target_ns) {
            _mm_pause();
        }

        // In a real system, this would inject into a ring buffer or socket
        // For now, we just simulate timing
    }

    std::cout << "Replay complete\n";
    return 0;
}

#include <iostream>
#include <fstream>
#include <random>
#include <cstdint>
#include <cstring>

struct SynthPacket {
    uint64_t ts_ns;
    uint32_t len;
    uint8_t  data[64];
};

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: mdp_gen_synth_feed <output.bin> <packet_count>\n";
        return 1;
    }

    const char* out_file = argv[1];
    size_t packet_count = std::stoull(argv[2]);

    std::ofstream out(out_file, std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open output file\n";
        return 1;
    }

    std::mt19937_64 rng(42);
    SynthPacket pkt{};

    uint64_t ts = 0;

    for (size_t i = 0; i < packet_count; ++i) {
        pkt.ts_ns = ts;
        pkt.len   = 32;

        // Simple ITCH-like synthetic message
        pkt.data[0] = 'A'; // AddOrder
        std::memcpy(pkt.data + 1, &i, sizeof(uint64_t));
        uint32_t price = static_cast<uint32_t>(rng() % 100000);
        uint32_t qty   = static_cast<uint32_t>(rng() % 1000);

        std::memcpy(pkt.data + 9,  &price, sizeof(price));
        std::memcpy(pkt.data + 13, &qty,   sizeof(qty));
        pkt.data[17] = 'B';

        out.write(reinterpret_cast<const char*>(&pkt), sizeof(pkt));
        ts += 1000; // 1 µs spacing
    }

    std::cout << "Generated " << packet_count << " synthetic packets\n";
    return 0;
}

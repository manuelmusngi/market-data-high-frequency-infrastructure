#include <iostream>
#include <random>
#include <vector>
#include <cassert>

#include "mdp/parse.hpp"

using namespace mdp;

int main() {
    std::cout << "[fuzz_parse] starting fuzz tests...\n";

    std::mt19937_64 rng(12345);
    std::uniform_int_distribution<int> len_dist(1, 64);
    std::uniform_int_distribution<int> byte_dist(0, 255);

    for (int iter = 0; iter < 500000; ++iter) {
        int len = len_dist(rng);
        std::vector<uint8_t> buf(len);

        for (int i = 0; i < len; ++i) {
            buf[i] = static_cast<uint8_t>(byte_dist(rng));
        }

        parse::ParseResult pr{};
        bool ok = parse::parse_itch(buf, pr);

        if (ok) {
            // If parser claims success, msg_ptr must be non-null
            assert(pr.msg_ptr != nullptr);
        }
    }

    std::cout << "[fuzz_parse] fuzzing complete — no crashes\n";
    return 0;
}

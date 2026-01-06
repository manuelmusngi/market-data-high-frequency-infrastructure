#include <iostream>
#include <vector>
#include <chrono>
#include <cstring>

#include "mdp/parse.hpp"
#include "mdp/time.hpp"

using namespace mdp;

int main() {
    constexpr size_t iterations = 10'000'000;

    // Synthetic ITCH AddOrder message
    std::vector<uint8_t> msg(32);
    msg[0] = 'A'; // AddOrder
    uint64_t order_id = 123456;
    uint32_t price = 10000;
    uint32_t qty = 100;

    std::memcpy(msg.data() + 1,  &order_id, sizeof(order_id));
    std::memcpy(msg.data() + 9,  &price,    sizeof(price));
    std::memcpy(msg.data() + 13, &qty,      sizeof(qty));
    msg[17] = 'B';

    parse::ParseResult result{};

    auto start = time::tsc_ns();
    for (size_t i = 0; i < iterations; ++i) {
        parse::parse_itch(msg, result);
    }
    auto end = time::tsc_ns();

    double total_ns = static_cast<double>(end - start);
    double ns_per_msg = total_ns / iterations;

    std::cout << "Micro Parse Benchmark\n";
    std::cout << "Iterations: " << iterations << "\n";
    std::cout << "Total time (ns): " << total_ns << "\n";
    std::cout << "Avg ns/message: " << ns_per_msg << "\n";

    return 0;
}

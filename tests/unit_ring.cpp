#include <iostream>
#include <cassert>
#include "mdp/ring.hpp"

using namespace mdp;

int main() {
    std::cout << "[unit_ring] starting tests...\n";

    Ring<int> r(8); // capacity 8 (must be power of 2)

    // Test empty pop
    auto v = r.try_pop();
    assert(!v.has_value());

    // Test push/pop
    assert(r.try_push(42));
    auto v2 = r.try_pop();
    assert(v2.has_value() && *v2 == 42);

    // Fill ring
    for (int i = 0; i < 7; ++i) {
        assert(r.try_push(i));
    }

    // Ring should be full now
    bool pushed = r.try_push(999);
    assert(!pushed);

    // Pop all
    for (int i = 0; i < 7; ++i) {
        auto x = r.try_pop();
        assert(x.has_value());
        assert(*x == i);
    }

    // Empty again
    assert(!r.try_pop().has_value());

    std::cout << "[unit_ring] all tests passed\n";
    return 0;
}

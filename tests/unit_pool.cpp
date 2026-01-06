#include <iostream>
#include <cassert>
#include "mdp/pool.hpp"

using namespace mdp;

struct TestObj {
    int x;
    int y;
};

int main() {
    std::cout << "[unit_pool] starting tests...\n";

    Pool<TestObj> pool(4);

    TestObj* a = pool.acquire();
    TestObj* b = pool.acquire();
    TestObj* c = pool.acquire();
    TestObj* d = pool.acquire();

    assert(a && b && c && d);

    // Pool should now be empty
    TestObj* e = pool.acquire();
    assert(e == nullptr);

    // Release one and reacquire
    pool.release(a);
    TestObj* f = pool.acquire();
    assert(f == a);

    std::cout << "[unit_pool] all tests passed\n";
    return 0;
}

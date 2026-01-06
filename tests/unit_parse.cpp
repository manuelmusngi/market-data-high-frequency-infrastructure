#include <iostream>
#include <cassert>
#include <cstring>

#include "mdp/parse.hpp"
#include "mdp/models.hpp"

using namespace mdp;

int main() {
    std::cout << "[unit_parse] starting tests...\n";

    // Build synthetic ITCH AddOrder message
    std::vector<uint8_t> msg(32);
    msg[0] = 'A';

    uint64_t order_id = 123456;
    uint32_t price    = 10000;
    uint32_t qty      = 250;

    std::memcpy(msg.data() + 1,  &order_id, sizeof(order_id));
    std::memcpy(msg.data() + 9,  &price,    sizeof(price));
    std::memcpy(msg.data() + 13, &qty,      sizeof(qty));
    msg[17] = 'B';

    parse::ParseResult pr{};
    bool ok = parse::parse_itch(msg, pr);
    assert(ok);

    auto* m = static_cast<model::AddOrder*>(pr.msg_ptr);
    assert(m->order_id == order_id);
    assert(m->price    == price);
    assert(m->qty      == qty);
    assert(m->side     == 'B');

    std::cout << "[unit_parse] all tests passed\n";
    return 0;
}

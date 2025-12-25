#include "mdp/parse.hpp"
#include "mdp/pool.hpp"
#include "mdp/time.hpp"
#include <cstring>

namespace mdp::parse {

using namespace mdp::model;

// For demonstration, reuse AddOrder as a generic message; in reality you'd
// define FEMA-specific structs and pools.
static thread_local Pool<AddOrder> fema_order_pool(1 << 15);

bool parse_fema(std::span<const uint8_t> s, ParseResult& out) {
    if (s.size() < 2) return false;

    // This is purely illustrative. Real FEMA/CME parsing is template-id based.
    AddOrder* m = fema_order_pool.acquire();
    if (!m) return false;

    std::size_t offset = 0;
    std::memcpy(&m->order_id, s.data() + offset, sizeof(m->order_id));
    offset += sizeof(m->order_id);

    std::memcpy(&m->price, s.data() + offset, sizeof(m->price));
    offset += sizeof(m->price);

    std::memcpy(&m->qty, s.data() + offset, sizeof(m->qty));
    offset += sizeof(m->qty);

    m->side = 'B';
    m->symbol_id = 0;
    m->ts_ns = time::tsc_ns();

    out.type    = MsgType::AddOrder;
    out.msg_ptr = m;
    return true;
}

} // namespace mdp::parse

#include "mdp/parse.hpp"
#include "mdp/pool.hpp"
#include "mdp/time.hpp"
#include <cstring>

namespace mdp::parse {
static thread_local Pool<model::AddOrder> add_pool(1<<15);

bool parse_itch(std::span<const uint8_t> s, ParseResult& out) {
  if (s.size() < 2) return false;
  auto type = static_cast<model::MsgType>(s[0]);
  switch (type) {
    case model::MsgType::AddOrder: {
      if (s.size() < 32) return false; // length check
      auto* m = add_pool.acquire();
      if (!m) return false;
      m->ts_ns = time::tsc_ns();
      std::memcpy(&m->order_id, s.data()+1, sizeof(m->order_id));
      std::memcpy(&m->price,     s.data()+9, sizeof(m->price));
      std::memcpy(&m->qty,       s.data()+13, sizeof(m->qty));
      m->side = static_cast<char>(s[17]);
      std::memcpy(&m->symbol_id, s.data()+18, sizeof(m->symbol_id));
      out.type = type; out.msg_ptr = m;
      return true;
    }
    default: return false;
  }
}
} // namespace mdp::parse

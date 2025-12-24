#include "mdp/dispatch.hpp"
#include "mdp/net.hpp"
#include "mdp/parse.hpp"
#include <immintrin.h>

namespace mdp {
void Dispatcher::run_parse(int tid) {
  // bind to per-RX ring or a shard
  auto& rx_ring = /* get from ingress */ /* ... */;
  auto& out_ring = msg_rings_[tid % msg_rings_.size()];
  for (;;) {
    if (auto pkt = rx_ring.try_pop()) {
      parse::ParseResult pr;
      if (parse::parse_itch(std::span(pkt->data, pkt->len), pr)) {
        MessageView mv{pr.type, pr.msg_ptr};
        if (!out_ring.try_push(mv)) { /* increment drop */ }
      }
    } else { _mm_pause(); }
  }
}

void Dispatcher::run_consumer(int tid) {
  auto& r = msg_rings_[tid];
  for (;;) {
    if (auto mv = r.try_pop()) {
      switch (mv->type) {
        case model::MsgType::AddOrder: {
          auto* m = static_cast<model::AddOrder*>(mv->ptr);
          // strategy hook or logging
          // release to pool if ownership returns
          // add_pool.release(m); // if pool is globally accessible
        } break;
        default: break;
      }
    } else { _mm_pause(); }
  }
}
} // namespace mdp

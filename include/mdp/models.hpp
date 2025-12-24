#pragma once
#include <cstdint>

namespace mdp::model {
struct AddOrder {
  uint64_t ts_ns;
  uint64_t order_id;
  uint32_t price; // price in ticks
  uint32_t qty;
  char side;      // 'B'/'S'
  uint32_t symbol_id;
};
// ... other messages
enum class MsgType : uint8_t { AddOrder='A', Exec='E', Cancel='X', Unknown=0 };
} // namespace mdp::model

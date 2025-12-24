#pragma once
#include <span>
#include "mdp/models.hpp"

namespace mdp::parse {
struct ParseResult {
  model::MsgType type;
  void* msg_ptr; // points to pool-allocated model struct
};

bool parse_itch(std::span<const uint8_t> pkt, ParseResult& out);
} // namespace mdp::parse

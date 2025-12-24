#pragma once
#include "mdp/ring.hpp"
#include "mdp/models.hpp"
#include "mdp/metrics.hpp"

namespace mdp {
struct MessageView { model::MsgType type; void* ptr; };

class Dispatcher {
public:
  Dispatcher(const AppConfig&, Metrics&);
  void run_parse(int parse_tid);
  void run_consumer(int consume_tid);
  void shutdown();
private:
  std::vector<Ring<MessageView>> msg_rings_;
  Metrics& metrics_;
};
} // namespace mdp

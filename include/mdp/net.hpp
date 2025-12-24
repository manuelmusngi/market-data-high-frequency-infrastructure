#pragma once
#include <cstdint>
#include <span>
#include "mdp/ring.hpp"
#include "mdp/metrics.hpp"

namespace mdp {
struct PacketView { const uint8_t* data; uint32_t len; uint64_t ts_ns; };

class NetIngress {
public:
  virtual ~NetIngress() = default;
  static std::unique_ptr<NetIngress> create(const std::string& mode, const AppConfig&);
  virtual void run_rx(int thread_id) = 0;
  virtual void shutdown() = 0;
  virtual Ring<PacketView>& rx_ring(int thread_id) = 0;
};
} // namespace mdp

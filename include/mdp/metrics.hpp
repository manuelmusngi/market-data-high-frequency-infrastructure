#pragma once
#include <atomic>
#include <vector>

namespace mdp {
struct Metrics {
  std::atomic<uint64_t> rx_pkts{0}, rx_drops{0}, parse_ok{0}, parse_fail{0};
  // histograms omitted for brevity
};
} // namespace mdp

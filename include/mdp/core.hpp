#pragma once
#include <vector>
#include <thread>
#include "mdp/net.hpp"
#include "mdp/dispatch.hpp"
#include "mdp/pinning.hpp"
#include "mdp/metrics.hpp"

namespace mdp {
struct AppConfig {
  std::string feed_type;   // "ITCH","FEMA"
  std::string net_mode;    // "AF_XDP","DPDK","SOCK"
  int rx_threads;
  int parse_threads;
  int consume_threads;
  std::vector<int> cpu_affinity;
};

class App {
public:
  explicit App(AppConfig cfg);
  void start();
  void stop();
private:
  AppConfig cfg_;
  std::unique_ptr<NetIngress> ingress_;
  std::vector<std::thread> rx_;
  std::vector<std::thread> parse_;
  std::vector<std::thread> consume_;
  std::unique_ptr<Dispatcher> dispatcher_;
  Metrics metrics_;
};
} // namespace mdp

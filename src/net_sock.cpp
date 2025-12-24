#include "mdp/net.hpp"
#include "mdp/time.hpp"
#include <sys/socket.h>
#include <netinet/in.h>

namespace mdp {
class SockIngress : public NetIngress {
  std::vector<Ring<PacketView>> rx_rings_;
  int sock_{-1};
public:
  SockIngress(const AppConfig& cfg) : rx_rings_(cfg.rx_threads) { /* create socket, join multicast, set options */ }
  void run_rx(int tid) override {
    auto& ring = rx_rings_[tid];
    uint8_t buf[2048];
    for (;;) {
      ssize_t n = ::recv(sock_, buf, sizeof(buf), MSG_DONTWAIT);
      if (n>0) {
        uint64_t ts = time::tsc_ns();
        PacketView pv{buf, static_cast<uint32_t>(n), ts};
        ring.try_push(pv); // drop if full; count drops in metrics
      } else { _mm_pause(); }
    }
  }
  void shutdown() override { /* set flag */ }
  Ring<PacketView>& rx_ring(int tid) override { return rx_rings_[tid]; }
};
} // namespace mdp

#include "mdp/net.hpp"
#include <stdexcept>
#include <iostream>
#include <immintrin.h>

namespace mdp {

class AfXdpIngress : public NetIngress {
public:
    explicit AfXdpIngress(const AppConfig& cfg)
        : cfg_(cfg),
          rx_rings_(cfg.rx_threads, Ring<PacketView>(1 << 16)),
          running_(true) {
        // TODO: Initialize AF_XDP sockets, umem, attach XDP program, etc.
        std::cerr << "[AF_XDP] WARNING: AF_XDP backend is currently a stub.\n";
    }

    void run_rx(int thread_id) override {
        auto& ring = rx_rings_[thread_id];
        while (running_.load(std::memory_order_relaxed)) {
            // TODO: Poll AF_XDP fill/comp rings, retrieve packets
            // For now, just pause-spin
            (void)ring;
            _mm_pause();
        }
    }

    void shutdown() override {
        running_.store(false, std::memory_order_relaxed);
    }

    Ring<PacketView>& rx_ring(int thread_id) override {
        return rx_rings_.at(thread_id);
    }

private:
    AppConfig cfg_;
    std::vector<Ring<PacketView>> rx_rings_;
    std::atomic<bool> running_;
};

// If you want mode-based factory here, you could specialize NetIngress::create,
// but to keep things simple, we just provide the class and you can wire it
// into NetIngress::create later when AF_XDP is implemented.

} // namespace mdp

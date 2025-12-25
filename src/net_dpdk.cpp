#include "mdp/net.hpp"
#include <stdexcept>
#include <iostream>
#include <immintrin.h>

namespace mdp {

class DpdkIngress : public NetIngress {
public:
    explicit DpdkIngress(const AppConfig& cfg)
        : cfg_(cfg),
          rx_rings_(cfg.rx_threads, Ring<PacketView>(1 << 16)),
          running_(true) {
        // TODO: Initialize DPDK EAL, ports, queues, mbuf pools, etc.
        std::cerr << "[DPDK] WARNING: DPDK backend is currently a stub.\n";
    }

    void run_rx(int thread_id) override {
        auto& ring = rx_rings_[thread_id];
        while (running_.load(std::memory_order_relaxed)) {
            // TODO: Use rte_eth_rx_burst to receive a batch, push into ring
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

} // namespace mdp


#include "mdp/core.hpp"
#include "mdp/parse.hpp"
#include "mdp/net.hpp"
#include "mdp/pinning.hpp"
#include <iostream>

namespace mdp {

App::App(AppConfig cfg)
    : cfg_(std::move(cfg)) {}

void App::start() {
    if (cfg_.cpu_affinity.empty()) {
        cfg_.cpu_affinity.resize(cfg_.rx_threads + cfg_.parse_threads + cfg_.consume_threads);
        for (int i = 0; i < static_cast<int>(cfg_.cpu_affinity.size()); ++i)
            cfg_.cpu_affinity[i] = i;
    }

    // Pin main thread just as a baseline (optional)
    pin::pin_this(cfg_.cpu_affinity.front());

    // Create network ingress
    ingress_ = NetIngress::create(cfg_.net_mode, cfg_);

    // Create dispatcher (constructed after ingress, so it can access rings)
    dispatcher_ = std::make_unique<Dispatcher>(cfg_, metrics_, *ingress_);

    // RX threads
    for (int i = 0; i < cfg_.rx_threads; ++i) {
        rx_.emplace_back([this, i] {
            pin::pin_this(cfg_.cpu_affinity[i]);
            ingress_->run_rx(i);
        });
    }

    // Parse threads
    for (int i = 0; i < cfg_.parse_threads; ++i) {
        int idx = cfg_.rx_threads + i;
        parse_.emplace_back([this, i, idx] {
            pin::pin_this(cfg_.cpu_affinity[idx]);
            dispatcher_->run_parse(i);
        });
    }

    // Consumer threads
    for (int i = 0; i < cfg_.consume_threads; ++i) {
        int idx = cfg_.rx_threads + cfg_.parse_threads + i;
        consume_.emplace_back([this, i, idx] {
            pin::pin_this(cfg_.cpu_affinity[idx]);
            dispatcher_->run_consumer(i);
        });
    }
}

void App::stop() {
    if (ingress_)   ingress_->shutdown();
    if (dispatcher_) dispatcher_->shutdown();

    for (auto& t : rx_)      if (t.joinable()) t.join();
    for (auto& t : parse_)   if (t.joinable()) t.join();
    for (auto& t : consume_) if (t.joinable()) t.join();
}

} // namespace mdp

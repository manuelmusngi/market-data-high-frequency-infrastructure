// src/core.cpp
#include "mdp/core.hpp"
#include "mdp/parse.hpp"
using namespace mdp;

void App::start() {
  pin::set_thread_affinity(cfg_.cpu_affinity); // seed main affinity
  ingress_ = NetIngress::create(cfg_.net_mode, cfg_); // factory; prealloc rings
  dispatcher_ = std::make_unique<Dispatcher>(cfg_, metrics_);

  // RX threads: NIC → packet rings
  for (int i=0;i<cfg_.rx_threads;++i)
    rx_.emplace_back([&, i]{ pin::pin_this(i); ingress_->run_rx(i); });

  // Parse threads: packet rings → message rings
  for (int i=0;i<cfg_.parse_threads;++i)
    parse_.emplace_back([&, i]{ pin::pin_this(cfg_.rx_threads+i); dispatcher_->run_parse(i); });

  // Consumers: handle parsed messages
  for (int i=0;i<cfg_.consume_threads;++i)
    consume_.emplace_back([&, i]{ pin::pin_this(cfg_.rx_threads+cfg_.parse_threads+i); dispatcher_->run_consumer(i); });
}

void App::stop() {
  ingress_->shutdown();
  dispatcher_->shutdown();
  for (auto &t: rx_) t.join();
  for (auto &t: parse_) t.join();
  for (auto &t: consume_) t.join();
}

#include "mdp/dispatch.hpp"
#include "mdp/net.hpp"
#include "mdp/parse.hpp"
#include "mdp/models.hpp"
#include <immintrin.h>
#include <iostream>

namespace mdp {

Dispatcher::Dispatcher(const AppConfig& cfg, Metrics& metrics, NetIngress& ingress)
    : metrics_(metrics),
      ingress_(ingress),
      running_(true) {
    int consumer_count = cfg.consume_threads;
    msg_rings_.reserve(consumer_count);
    for (int i = 0; i < consumer_count; ++i) {
        msg_rings_.emplace_back(Ring<MessageView>(1 << 16));
    }
}

void Dispatcher::run_parse(int parse_tid) {
    // Simple sharding: parse thread i reads RX ring i % rx_threads (assume equal)
    // For now, we assume ingress_.rx_ring(parse_tid) is valid
    auto& rx_ring = ingress_.rx_ring(parse_tid);
    auto& out_ring = msg_rings_[parse_tid % msg_rings_.size()];

    while (running_.load(std::memory_order_relaxed)) {
        auto pkt_opt = rx_ring.try_pop();
        if (!pkt_opt) {
            _mm_pause();
            continue;
        }

        auto& pkt = *pkt_opt;
        parse::ParseResult pr{};
        bool ok = parse::parse_itch(std::span(pkt.data, pkt.len), pr);
        if (!ok) {
            metrics_.parse_fail.fetch_add(1, std::memory_order_relaxed);
            continue;
        }

        MessageView mv{pr.type, pr.msg_ptr};
        if (!out_ring.try_push(mv)) {
            metrics_.rx_drops.fetch_add(1, std::memory_order_relaxed);
        } else {
            metrics_.parse_ok.fetch_add(1, std::memory_order_relaxed);
        }
    }
}

void Dispatcher::run_consumer(int consume_tid) {
    auto& r = msg_rings_[consume_tid];

    while (running_.load(std::memory_order_relaxed)) {
        auto mv_opt = r.try_pop();
        if (!mv_opt) {
            _mm_pause();
            continue;
        }

        auto mv = *mv_opt;
        switch (mv.type) {
        case model::MsgType::AddOrder: {
            auto* m = static_cast<model::AddOrder*>(mv.ptr);
            // Demo: just print once in a while (not for real low latency)
            // std::cout << "AddOrder id=" << m->order_id << "\n";
            // In production: strategy, logging, risk, etc.
            // TODO: return to pool when ownership model is finalized
            break;
        }
        default:
            // Unknown or unsupported message type
            break;
        }
    }
}

void Dispatcher::shutdown() {
    running_.store(false, std::memory_order_relaxed);
}

} // namespace mdp

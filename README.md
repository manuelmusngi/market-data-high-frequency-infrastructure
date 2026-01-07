#### System-level C++20 project for market data feeds

This project is a development in C++ for high-frequency trading infrastructure, backed by academic research and practical engineering insights.

The project is an exercise in development of a production-style, modular C++20 that demonstrates: 

- systems programming 
- algorithms/data structures
- multithreading 
- networked I/O 
- serialization 
- CPU-aware engineering 
- kernel-level considerations 
- compilers. 

It includes message parsers and feed handlers for direct venue market data (UDP multicast, TCP, ITCH/FIX-like parsing), with clean interfaces and testable components.

---
🚀 Highlights

- Ultra-low latency design: Capable of handling millions of messages per second with nanosecond-level latency.

- Modern C++20 features: Utilizes concepts, constexpr, atomics, and structured bindings for clean, efficient code.

- Optimized network I/O: Supports AF_XDP, DPDK, and portable socket-based ingestion for high-throughput feeds.

- Lock-free concurrency: Implements SPSC/MPSC ring buffers and per-thread memory pools to eliminate contention.

- Protocol parsing: Branch-light, SIMD-friendly parsers for market data protocols (e.g., NASDAQ ITCH).

- Scalable architecture: Modular design with ingress → parse → dispatch → consumer pipeline, NUMA-aware threading, and CPU pinning.

- Observability: Built-in metrics, histograms, and benchmarking tools to measure throughput, latency, and drops.

- Reproducibility: Clear project structure, unit tests, fuzzing, and synthetic feed replayers for validation.
 
---
🔑 Key Takeaways

- Serialization optimization matters: Zero-copy parsing and compile-time layouts drastically reduce overhead.

- Networked I/O is the bottleneck: Kernel-bypass (AF_XDP/DPDK) is essential for nanosecond-level performance.

- Concurrency must be lock-free: Rings and pools outperform traditional queues under extreme message rates.

- NUMA and CPU affinity are critical: Pinning threads and aligning memory allocations to NUMA nodes avoids hidden latency spikes.

- Benchmarking is non-negotiable: Micro-benchmarks and synthetic replayers validate performance claims and guide tuning.

- Documentation is part of engineering: Architecture diagrams, tuning guides, and reproducible configs make the system usable by collaborators.

---
<img width="20" height="20" alt="image" src="https://github.com/user-attachments/assets/5a0783d5-17b6-467c-9a30-a0c620773cac" /> Project Architecture

market-data-parser/\
├─ [CMakeLists.txt](https://github.com/manuelmusngi/qr-market-data-parser-high-frequency-infrastructure/blob/main/CMakeLists.txt)\
├─ cmake/\
│  └─ [toolchains/clang-libc++.cmake](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/cmake/toolchains/clang-libc%2B%2B.cmake)\
├─ config/\
│  ├─ [mdp.toml](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/config/mdp.toml)\
│  └─ feeds/\
│     ├─ [feed_itch.toml](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/config/feeds/feed_itch.toml)\
│     └─ [feed_fema.toml](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/config/feeds/feed_fema.toml)\
├─ include/\
│  ├─ mdp/\
│  │  ├─ [core.hpp](https://github.com/manuelmusngi/qr-market-data-parser-high-frequency-infrastructure/blob/main/include/mdp/core.hpp)\
│  │  ├─ [net.hpp](https://github.com/manuelmusngi/qr-market-data-parser-high-frequency-infrastructure/blob/main/include/mdp/net.hpp)\
│  │  ├─ [ring.hpp](https://github.com/manuelmusngi/qr-market-data-parser-high-frequency-infrastructure/blob/main/include/mdp/ring.hpp)\
│  │  ├─ [pool.hpp](https://github.com/manuelmusngi/qr-market-data-parser-high-frequency-infrastructure/blob/main/include/mdp/pool.hpp)\
│  │  ├─ [time.hpp](https://github.com/manuelmusngi/qr-market-data-parser-high-frequency-infrastructure/blob/main/include/mdp/time.hpp)\
│  │  ├─ [parse.hpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/include/mdp/parse.hpp)\
│  │  ├─ [models.hpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/include/mdp/models.hpp)\
│  │  ├─ [dispatch.hpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/include/mdp/dispatch.hpp)\
│  │  ├─ [metrics.hpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/include/mdp/metrics.hpp)\
│  │  └─ [pinning.hpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/include/mdp/pinning.hpp)\
├─ src/\
│  ├─ [main.cpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/src/main.cpp)\
│  ├─ [core.cpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/src/core.cpp)\
│  ├─ [net_af_xdp.cpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/src/net_af_xdp.cpp)\
│  ├─ [net_dpdk.cpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/src/net_dpdk.cpp)\
│  ├─ [net_sock.cpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/src/net_sock.cpp)\
│  ├─ [parse_itch.cpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/src/parse_itch.cpp)\
│  ├─ [parse_fema.cpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/src/parse_fema.cpp)\
│  ├─ [dispatch.cpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/src/dispatch.cpp)\
│  ├─ [metrics.cpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/src/metrics.cpp)\
│  └─ [time.cpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/src/time.cpp)\
├─ tools/\
│  ├─ [replayer.cpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/tools/replayer.cpp)\
│  ├─ [pcap_to_ring.cpp](https://github.com/manuelmusngi/market-data-parser-high-frequency-infrastructure/blob/main/tools/pcap_to_ring.cpp)\
│  └─ gen_synth_feed.cpp\
├─ bench/\
│  ├─ micro_parse.cpp\
│  ├─ throughput.cpp\
│  └─ latency.cpp\
├─ tests/\
│  ├─ unit_ring.cpp\
│  ├─ unit_pool.cpp\
│  ├─ unit_parse.cpp\
│  └─ fuzz_parse.cpp\
└─scripts/\
│  ├─ run_bench.sh\
│  ├─ cpu_pinning.sh\
│  └─ perf_record.sh

---
📚 References & Research Papers

[C++ Design Patterns for Low-latency Applications Including High-frequency Trading](https://arxiv.org/abs/2309.04259)

[High-Frequency Trading in C++26: Nanosecond Latency Optimization Practical techniques for pushing latency below microseconds using modern C++ features](https://markaicode.com/cpp26-high-frequency-trading-latency-optimization/)

[Network Design Considerations for Trading Systems SIGCOMM HotNets 2024 — Discusses specialized low-latency network architectures for algorithmic trading](https://conferences.sigcomm.org/hotnets/2024/papers/hotnets24-262.pdf)

[BQ: A Lock-Free Queue with Batching Explores batching techniques to improve throughput in concurrent queues](https://csaws.cs.technion.ac.il/~erez/Papers/bq-topc.pdf)

---
#### License
This project is licensed under the [MIT License](https://github.com/manuelmusngi/regime_switching_models/edit/main/LICENSE).






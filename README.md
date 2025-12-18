#### System-level C++20 project for market data feeds

This is a production-style, modular C++20 project that demonstrates systems programming, algorithms/data structures, multithreading, networked I/O, serialization, CPU-aware engineering, kernel-level considerations, and compilers. It includes message parsers and feed handlers for direct venue market data (UDP multicast, TCP, ITCH/FIX-like parsing), with clean interfaces and testable components.

This project is a development practice in C++20 system for high-frequency trading infrastructure, backed by academic research and practical engineering insights.

__

🚀 Highlights

Ultra-low latency design: Capable of handling millions of messages per second with nanosecond-level latency.

Modern C++20 features: Utilizes concepts, constexpr, atomics, and structured bindings for clean, efficient code.

Optimized network I/O: Supports AF_XDP, DPDK, and portable socket-based ingestion for high-throughput feeds.

Lock-free concurrency: Implements SPSC/MPSC ring buffers and per-thread memory pools to eliminate contention.

Protocol parsing: Branch-light, SIMD-friendly parsers for market data protocols (e.g., NASDAQ ITCH).

Scalable architecture: Modular design with ingress → parse → dispatch → consumer pipeline, NUMA-aware threading, and CPU pinning.

Observability: Built-in metrics, histograms, and benchmarking tools to measure throughput, latency, and drops.

Reproducibility: Clear project structure, unit tests, fuzzing, and synthetic feed replayers for validation.

--

🔑 Key Takeaways

Serialization optimization matters: Zero-copy parsing and compile-time layouts drastically reduce overhead.

Networked I/O is the bottleneck: Kernel-bypass (AF_XDP/DPDK) is essential for nanosecond-level performance.

Concurrency must be lock-free: Rings and pools outperform traditional queues under extreme message rates.

NUMA and CPU affinity are critical: Pinning threads and aligning memory allocations to NUMA nodes avoids hidden latency spikes.

Benchmarking is non-negotiable: Micro-benchmarks and synthetic replayers validate performance claims and guide tuning.

Documentation is part of engineering: Architecture diagrams, tuning guides, and reproducible configs make the system usable by collaborators.

--

Project Architecture
market-data-parser/
├─ CMakeLists.txt
├─ cmake/
│  └─ toolchains/clang-libc++.cmake
├─ config/
│  ├─ mdp.toml
│  └─ feeds/
│     ├─ feed_itch.toml
│     └─ feed_fema.toml
├─ include/
│  ├─ mdp/
│  │  ├─ core.hpp
│  │  ├─ net.hpp
│  │  ├─ ring.hpp
│  │  ├─ pool.hpp
│  │  ├─ time.hpp
│  │  ├─ parse.hpp
│  │  ├─ models.hpp
│  │  ├─ dispatch.hpp
│  │  ├─ metrics.hpp
│  │  └─ pinning.hpp
├─ src/
│  ├─ main.cpp
│  ├─ core.cpp
│  ├─ net_af_xdp.cpp
│  ├─ net_dpdk.cpp
│  ├─ net_sock.cpp
│  ├─ parse_itch.cpp
│  ├─ parse_fema.cpp
│  ├─ dispatch.cpp
│  ├─ metrics.cpp
│  └─ time.cpp
├─ tools/
│  ├─ replayer.cpp
│  ├─ pcap_to_ring.cpp
│  └─ gen_synth_feed.cpp
├─ bench/
│  ├─ micro_parse.cpp
│  ├─ throughput.cpp
│  └─ latency.cpp
├─ tests/
│  ├─ unit_ring.cpp
│  ├─ unit_pool.cpp
│  ├─ unit_parse.cpp
│  └─ fuzz_parse.cpp
├─ scripts/
│  ├─ run_bench.sh
│  ├─ cpu_pinning.sh
│  └─ perf_record.sh
└─ docs/
   ├─ architecture.md
   ├─ performance.md
   └─ ops_tuning.md

--

📚 References & Research Papers

C++ Design Patterns for Low-latency Applications Including High-frequency Trading Paul Bilokon, Burak Gunduz — arXiv:2309.04259 Read here

High-Frequency Trading in C++26: Nanosecond Latency Optimization Practical techniques for pushing latency below microseconds using modern C++ features. (https://arxiv.org/abs/2309.04259)

Network Design Considerations for Trading Systems SIGCOMM HotNets 2024 — Discusses specialized low-latency network architectures for algorithmic trading. (https://conferences.sigcomm.org/hotnets/2024/papers/hotnets24-262.pdf)

No Cords Attached: Coordination-Free Concurrent Lock-Free Queues Yusuf Motiwala — Introduces Cyclic Memory Protection (CMP) for scalable lock-free queues. (https://arxiv.org/pdf/2511.09410)

BQ: A Lock-Free Queue with Batching Explores batching techniques to improve throughput in concurrent queues. (https://csaws.cs.technion.ac.il/~erez/Papers/bq-topc.pdf)



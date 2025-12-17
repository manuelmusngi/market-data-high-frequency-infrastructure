#### System-level C++20 project for market data feeds

This is a production-style, modular C++20 project that demonstrates systems programming, algorithms/data structures, multithreading, networked I/O, serialization, CPU-aware engineering, kernel-level considerations, and compilers. It includes message parsers and feed handlers for direct venue market data (UDP multicast, TCP, ITCH/FIX-like parsing), with clean interfaces and testable components.

This project is a development practice in C++20 system for high-frequency trading infrastructure, backed by academic research and practical engineering insights.

🚀 Highlights

Ultra-low latency design: Capable of handling millions of messages per second with nanosecond-level latency.

Modern C++20 features: Utilizes concepts, constexpr, atomics, and structured bindings for clean, efficient code.

Optimized network I/O: Supports AF_XDP, DPDK, and portable socket-based ingestion for high-throughput feeds.

Lock-free concurrency: Implements SPSC/MPSC ring buffers and per-thread memory pools to eliminate contention.

Protocol parsing: Branch-light, SIMD-friendly parsers for market data protocols (e.g., NASDAQ ITCH).

Scalable architecture: Modular design with ingress → parse → dispatch → consumer pipeline, NUMA-aware threading, and CPU pinning.

Observability: Built-in metrics, histograms, and benchmarking tools to measure throughput, latency, and drops.

Reproducibility: Clear project structure, unit tests, fuzzing, and synthetic feed replayers for validation.

🔑 Key Takeaways

Serialization optimization matters: Zero-copy parsing and compile-time layouts drastically reduce overhead.

Networked I/O is the bottleneck: Kernel-bypass (AF_XDP/DPDK) is essential for nanosecond-level performance.

Concurrency must be lock-free: Rings and pools outperform traditional queues under extreme message rates.

NUMA and CPU affinity are critical: Pinning threads and aligning memory allocations to NUMA nodes avoids hidden latency spikes.

Benchmarking is non-negotiable: Micro-benchmarks and synthetic replayers validate performance claims and guide tuning.

Documentation is part of engineering: Architecture diagrams, tuning guides, and reproducible configs make the system usable by collaborators.



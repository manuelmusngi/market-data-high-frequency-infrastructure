#pragma once
#include <atomic>
#include <cstddef>
#include <vector>
#include <optional>
#include <new>

namespace mdp {
template<typename T>
class Ring {
public:
  explicit Ring(size_t capacity_pow2 = 1<<16)
    : cap_(capacity_pow2), mask_(capacity_pow2 - 1),
      buf_(operator new[](cap_*sizeof(T))), head_(0), tail_(0) {}

  ~Ring() { operator delete[](buf_); }

  bool try_push(const T& v) noexcept {
    auto head = head_.load(std::memory_order_relaxed);
    auto next = (head + 1) & mask_;
    if (next == tail_.load(std::memory_order_acquire)) return false;
    std::construct_at(&elem(head), v);
    head_.store(next, std::memory_order_release);
    return true;
  }
  std::optional<T> try_pop() noexcept {
    auto tail = tail_.load(std::memory_order_relaxed);
    if (tail == head_.load(std::memory_order_acquire)) return std::nullopt;
    T out = elem(tail);
    std::destroy_at(&elem(tail));
    tail_.store((tail + 1) & mask_, std::memory_order_release);
    return out;
  }
private:
  T& elem(size_t i) { return *std::launder(reinterpret_cast<T*>(buf_) + i); }
  void* buf_;
  const size_t cap_, mask_;
  alignas(64) std::atomic<size_t> head_, tail_;
};
} // namespace mdp

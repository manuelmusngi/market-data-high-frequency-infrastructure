#pragma once
#include <vector>
#include <atomic>

namespace mdp {
template<typename T>
class Pool {
public:
  explicit Pool(size_t n) : storage_(n), freelist_(n) {
    for (size_t i=0;i<n;++i) freelist_[i].store(i, std::memory_order_relaxed);
    head_.store(n, std::memory_order_relaxed);
  }
  T* acquire() noexcept {
    auto h = head_.load(std::memory_order_acquire);
    if (h == 0) return nullptr;
    auto idx = freelist_[h-1].load(std::memory_order_relaxed);
    head_.store(h-1, std::memory_order_release);
    return &storage_[idx];
  }
  void release(T* p) noexcept {
    auto idx = static_cast<size_t>(p - storage_.data());
    auto h = head_.load(std::memory_order_relaxed);
    freelist_[h].store(idx, std::memory_order_relaxed);
    head_.store(h+1, std::memory_order_release);
  }
private:
  std::vector<T> storage_;
  std::vector<std::atomic<size_t>> freelist_;
  std::atomic<size_t> head_;
};
} // namespace mdp

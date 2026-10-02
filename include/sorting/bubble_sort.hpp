#pragma once

#include "sort_interface.hpp"
#include "metrics.hpp"
#include <algorithm>
#include <vector>
#include <iterator>
#include <type_traits>

namespace sorting {

template <typename T = int>
class BubbleSort : public ISorter {
public:
    const char* name() const noexcept override { return "Bubble Sort"; }
    const char* complexity_best() const noexcept override { return "O(N)"; }
    const char* complexity_avg() const noexcept override { return "O(N^2)"; }
    const char* complexity_worst() const noexcept override { return "O(N^2)"; }
    const char* space_complexity() const noexcept override { return "O(1)"; }
    bool is_stable() const noexcept override { return true; }
    bool is_in_place() const noexcept override { return true; }

    SortMetrics get_metrics() const override {
        return {GlobalMetrics::comparisons, GlobalMetrics::moves, GlobalMetrics::allocations};
    }

    void reset_metrics() override {
        GlobalMetrics::reset();
    }

    void sort(std::vector<int>& data) override {
        sort_range(data.begin(), data.end());
    }

    void sort(std::vector<int>::iterator first, std::vector<int>::iterator last) override {
        sort_range(first, last);
    }

    template <typename RandomIt>
    void sort(RandomIt first, RandomIt last) {
        sort_range(first, last);
    }

    template <typename U = T>
    requires (!std::is_same_v<U, int>)
    void sort(std::vector<U>& data) {
        sort_range(data.begin(), data.end());
    }

private:
    template <typename RandomIt>
    void sort_range(RandomIt first, RandomIt last) {
        auto n = std::distance(first, last);
        if (n <= 1) return;

        for (decltype(n) i = 0; i < n - 1; ++i) {
            bool swapped = false;
            for (auto it = first; it != first + (n - 1 - i); ++it) {
                increment_comparisons();
                if (*(it + 1) < *it) {
                    std::iter_swap(it, it + 1);
                    increment_moves();
                    swapped = true;
                }
            }
            if (!swapped) break;
        }
    }
};

template <typename T = int>
class OptimizedBubbleSort : public ISorter {
public:
    const char* name() const noexcept override { return "Bubble Sort (Optimized)"; }
    const char* complexity_best() const noexcept override { return "O(N)"; }
    const char* complexity_avg() const noexcept override { return "O(N^2)"; }
    const char* complexity_worst() const noexcept override { return "O(N^2)"; }
    const char* space_complexity() const noexcept override { return "O(1)"; }
    bool is_stable() const noexcept override { return true; }
    bool is_in_place() const noexcept override { return true; }

    SortMetrics get_metrics() const override {
        return {GlobalMetrics::comparisons, GlobalMetrics::moves, GlobalMetrics::allocations};
    }

    void reset_metrics() override {
        GlobalMetrics::reset();
    }

    void sort(std::vector<int>& data) override {
        sort_range(data.begin(), data.end());
    }

    void sort(std::vector<int>::iterator first, std::vector<int>::iterator last) override {
        sort_range(first, last);
    }

    template <typename RandomIt>
    void sort(RandomIt first, RandomIt last) {
        sort_range(first, last);
    }

    template <typename U = T>
    requires (!std::is_same_v<U, int>)
    void sort(std::vector<U>& data) {
        sort_range(data.begin(), data.end());
    }

private:
    template <typename RandomIt>
    void sort_range(RandomIt first, RandomIt last) {
        auto n = std::distance(first, last);
        if (n <= 1) return;

        while (n > 1) {
            decltype(n) last_swap = 0;
            for (decltype(n) i = 1; i < n; ++i) {
                increment_comparisons();
                if (*(first + i) < *(first + i - 1)) {
                    std::iter_swap(first + i - 1, first + i);
                    increment_moves();
                    last_swap = i;
                }
            }
            n = last_swap;
        }
    }
};

} // namespace sorting
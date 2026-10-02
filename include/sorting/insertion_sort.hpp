#pragma once

#include "sort_interface.hpp"
#include "metrics.hpp"
#include <algorithm>
#include <vector>
#include <iterator>
#include <type_traits>

namespace sorting {

template <typename T = int>
class InsertionSort : public ISorter {
public:
    const char* name() const noexcept override { return "Insertion Sort"; }
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

        for (decltype(n) i = 1; i < n; ++i) {
            auto key = std::move(*(first + i));
            increment_moves();
            decltype(n) j = i;

            while (j > 0) {
                increment_comparisons();
                if (!(key < *(first + j - 1))) break;
                *(first + j) = std::move(*(first + j - 1));
                increment_moves();
                --j;
            }
            *(first + j) = std::move(key);
            increment_moves();
        }
    }
};

template <typename T = int>
class BinaryInsertionSort : public ISorter {
public:
    const char* name() const noexcept override { return "Binary Insertion Sort"; }
    const char* complexity_best() const noexcept override { return "O(N log N)"; }
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

        for (decltype(n) i = 1; i < n; ++i) {
            auto key = std::move(*(first + i));
            increment_moves();
            decltype(n) left = 0;
            decltype(n) right = i;

            while (left < right) {
                decltype(n) mid = left + (right - left) / 2;
                increment_comparisons();
                if (key < *(first + mid)) {
                    right = mid;
                } else {
                    left = mid + 1;
                }
            }

            for (decltype(n) j = i; j > left; --j) {
                *(first + j) = std::move(*(first + j - 1));
                increment_moves();
            }
            *(first + left) = std::move(key);
            increment_moves();
        }
    }
};

} // namespace sorting
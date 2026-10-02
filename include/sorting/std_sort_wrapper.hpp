#pragma once

#include "sort_interface.hpp"
#include "metrics.hpp"
#include <algorithm>
#include <vector>
#include <iterator>
#include <type_traits>

namespace sorting {

template <typename T = int>
class StdSortWrapper : public ISorter {
public:
    const char* name() const noexcept override { return "std::sort (Introsort)"; }
    const char* complexity_best() const noexcept override { return "O(N log N)"; }
    const char* complexity_avg() const noexcept override { return "O(N log N)"; }
    const char* complexity_worst() const noexcept override { return "O(N log N)"; }
    const char* space_complexity() const noexcept override { return "O(log N)"; }
    bool is_stable() const noexcept override { return false; }
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
        std::sort(first, last);
    }
};

template <typename T = int>
class StdStableSortWrapper : public ISorter {
public:
    const char* name() const noexcept override { return "std::stable_sort"; }
    const char* complexity_best() const noexcept override { return "O(N log N)"; }
    const char* complexity_avg() const noexcept override { return "O(N log N)"; }
    const char* complexity_worst() const noexcept override { return "O(N log^2 N)"; }
    const char* space_complexity() const noexcept override { return "O(N)"; }
    bool is_stable() const noexcept override { return true; }
    bool is_in_place() const noexcept override { return false; }

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
        std::stable_sort(first, last);
    }
};

} // namespace sorting
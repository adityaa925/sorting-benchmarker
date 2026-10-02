#pragma once

#include "sort_interface.hpp"
#include "metrics.hpp"
#include <algorithm>
#include <vector>
#include <iterator>
#include <type_traits>

namespace sorting {

template <typename T = int>
class QuickSort : public ISorter {
public:
    const char* name() const noexcept override { return "Quick Sort"; }
    const char* complexity_best() const noexcept override { return "O(N log N)"; }
    const char* complexity_avg() const noexcept override { return "O(N log N)"; }
    const char* complexity_worst() const noexcept override { return "O(N^2)"; }
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
        auto n = std::distance(first, last);
        if (n <= 1) return;
        quick_sort_recursive(first, last);
    }

    template <typename RandomIt>
    void quick_sort_recursive(RandomIt first, RandomIt last) {
        auto n = std::distance(first, last);
        if (n <= 1) return;

        if (n <= 16) {
            insertion_sort_range(first, last);
            return;
        }

        auto partition_point = hoare_partition(first, last);
        if (partition_point == first) {
            partition_point = first + 1;
        } else if (partition_point == last) {
            partition_point = last - 1;
        }

        quick_sort_recursive(first, partition_point);
        quick_sort_recursive(partition_point, last);
    }

    template <typename RandomIt>
    RandomIt hoare_partition(RandomIt first, RandomIt last) {
        auto n = std::distance(first, last);
        auto mid = first + n / 2;
        auto right_idx = last - 1;

        increment_comparisons();
        if (*mid < *first) { std::iter_swap(first, mid); increment_moves(); }
        increment_comparisons();
        if (*right_idx < *first) { std::iter_swap(first, right_idx); increment_moves(); }
        increment_comparisons();
        if (*right_idx < *mid) { std::iter_swap(mid, right_idx); increment_moves(); }

        auto pivot = *mid;
        auto i = first;
        auto j = last - 1;

        while (true) {
            while (true) {
                increment_comparisons();
                if (!(*i < pivot)) break;
                ++i;
            }
            while (true) {
                increment_comparisons();
                if (!(pivot < *j)) break;
                if (j == first) break;
                --j;
            }

            if (i >= j) return j + 1;

            std::iter_swap(i, j);
            increment_moves();
            ++i;
            if (i == j) break;
            --j;
        }
        return j + 1;
    }

    template <typename RandomIt>
    void insertion_sort_range(RandomIt first, RandomIt last) {
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
class QuickSort3Way : public ISorter {
public:
    const char* name() const noexcept override { return "Quick Sort (3-Way)"; }
    const char* complexity_best() const noexcept override { return "O(N log N)"; }
    const char* complexity_avg() const noexcept override { return "O(N log N)"; }
    const char* complexity_worst() const noexcept override { return "O(N^2)"; }
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
        auto n = std::distance(first, last);
        if (n <= 1) return;
        quick_sort_3way_recursive(first, last);
    }

    template <typename RandomIt>
    void quick_sort_3way_recursive(RandomIt first, RandomIt last) {
        auto n = std::distance(first, last);
        if (n <= 1) return;

        if (n <= 16) {
            insertion_sort_range(first, last);
            return;
        }

        auto mid = first + n / 2;
        auto right_idx = last - 1;

        increment_comparisons();
        if (*mid < *first) { std::iter_swap(first, mid); increment_moves(); }
        increment_comparisons();
        if (*right_idx < *first) { std::iter_swap(first, right_idx); increment_moves(); }
        increment_comparisons();
        if (*right_idx < *mid) { std::iter_swap(mid, right_idx); increment_moves(); }

        auto pivot = *mid;
        auto lt = first;
        auto gt = last - 1;
        auto it = first;

        while (it <= gt) {
            increment_comparisons();
            if (*it < pivot) {
                std::iter_swap(lt, it);
                increment_moves();
                ++lt;
                ++it;
            } else {
                increment_comparisons();
                if (pivot < *it) {
                    std::iter_swap(it, gt);
                    increment_moves();
                    if (gt == first) break;
                    --gt;
                } else {
                    ++it;
                }
            }
        }

        quick_sort_3way_recursive(first, lt);
        quick_sort_3way_recursive(gt + 1, last);
    }

    template <typename RandomIt>
    void insertion_sort_range(RandomIt first, RandomIt last) {
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

} // namespace sorting
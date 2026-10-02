#pragma once

#include "sort_interface.hpp"
#include "metrics.hpp"
#include <algorithm>
#include <vector>
#include <iterator>
#include <type_traits>

namespace sorting {

template <typename T = int>
class MergeSort : public ISorter {
public:
    const char* name() const noexcept override { return "Merge Sort"; }
    const char* complexity_best() const noexcept override { return "O(N log N)"; }
    const char* complexity_avg() const noexcept override { return "O(N log N)"; }
    const char* complexity_worst() const noexcept override { return "O(N log N)"; }
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
        auto n = static_cast<std::size_t>(std::distance(first, last));
        if (n <= 1) return;

        using ValueType = typename std::iterator_traits<RandomIt>::value_type;
        std::vector<ValueType> buffer;
        buffer.reserve(n);
        for (auto it = first; it != last; ++it) {
            buffer.push_back(*it);
        }
        increment_allocations();

        merge_sort_recursive(first, 0, n, buffer);
    }

    template <typename RandomIt, typename BufferType>
    void merge_sort_recursive(RandomIt first, std::size_t left, std::size_t right, BufferType& buffer) {
        if (right - left <= 1) return;

        std::size_t mid = left + (right - left) / 2;
        merge_sort_recursive(first, left, mid, buffer);
        merge_sort_recursive(first, mid, right, buffer);
        merge(first, left, mid, right, buffer);
    }

    template <typename RandomIt, typename BufferType>
    void merge(RandomIt first, std::size_t left, std::size_t mid, std::size_t right, BufferType& buffer) {
        std::size_t i = left;
        std::size_t j = mid;
        std::size_t k = left;

        while (i < mid && j < right) {
            increment_comparisons();
            if (*(first + j) < *(first + i)) {
                buffer[k++] = std::move(*(first + j++));
            } else {
                buffer[k++] = std::move(*(first + i++));
            }
            increment_moves();
        }

        while (i < mid) {
            buffer[k++] = std::move(*(first + i++));
            increment_moves();
        }

        while (j < right) {
            buffer[k++] = std::move(*(first + j++));
            increment_moves();
        }

        for (std::size_t idx = left; idx < right; ++idx) {
            *(first + idx) = std::move(buffer[idx]);
            increment_moves();
        }
    }
};

template <typename T = int>
class BottomUpMergeSort : public ISorter {
public:
    const char* name() const noexcept override { return "Bottom-Up Merge Sort"; }
    const char* complexity_best() const noexcept override { return "O(N log N)"; }
    const char* complexity_avg() const noexcept override { return "O(N log N)"; }
    const char* complexity_worst() const noexcept override { return "O(N log N)"; }
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
        auto n = static_cast<std::size_t>(std::distance(first, last));
        if (n <= 1) return;

        using ValueType = typename std::iterator_traits<RandomIt>::value_type;
        std::vector<ValueType> buffer;
        buffer.reserve(n);
        for (auto it = first; it != last; ++it) {
            buffer.push_back(*it);
        }
        increment_allocations();

        for (std::size_t width = 1; width < n; width *= 2) {
            for (std::size_t i = 0; i < n; i += 2 * width) {
                std::size_t left = i;
                std::size_t mid = std::min(i + width, n);
                std::size_t right = std::min(i + 2 * width, n);
                if (mid < right) {
                    merge(first, left, mid, right, buffer);
                }
            }
        }
    }

    template <typename RandomIt, typename BufferType>
    void merge(RandomIt first, std::size_t left, std::size_t mid, std::size_t right, BufferType& buffer) {
        std::size_t i = left;
        std::size_t j = mid;
        std::size_t k = left;

        while (i < mid && j < right) {
            increment_comparisons();
            if (*(first + j) < *(first + i)) {
                buffer[k++] = std::move(*(first + j++));
            } else {
                buffer[k++] = std::move(*(first + i++));
            }
            increment_moves();
        }

        while (i < mid) {
            buffer[k++] = std::move(*(first + i++));
            increment_moves();
        }

        while (j < right) {
            buffer[k++] = std::move(*(first + j++));
            increment_moves();
        }

        for (std::size_t idx = left; idx < right; ++idx) {
            *(first + idx) = std::move(buffer[idx]);
            increment_moves();
        }
    }
};

} // namespace sorting
#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

namespace sorting {

struct SortMetrics {
    std::size_t comparisons = 0;
    std::size_t moves = 0;
    std::size_t allocations = 0;

    void reset() {
        comparisons = 0;
        moves = 0;
        allocations = 0;
    }

    SortMetrics& operator+=(const SortMetrics& other) {
        comparisons += other.comparisons;
        moves += other.moves;
        allocations += other.allocations;
        return *this;
    }
};

class ISorter {
public:
    virtual ~ISorter() = default;
    virtual const char* name() const noexcept = 0;
    virtual const char* complexity_best() const noexcept = 0;
    virtual const char* complexity_avg() const noexcept = 0;
    virtual const char* complexity_worst() const noexcept = 0;
    virtual const char* space_complexity() const noexcept = 0;
    virtual bool is_stable() const noexcept = 0;
    virtual bool is_in_place() const noexcept = 0;

    virtual void sort(std::vector<int>& data) = 0;
    virtual void sort(std::vector<int>::iterator first, std::vector<int>::iterator last) = 0;

    virtual SortMetrics get_metrics() const = 0;
    virtual void reset_metrics() = 0;
};

} // namespace sorting
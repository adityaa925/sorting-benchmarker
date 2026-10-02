#pragma once

#include <cstddef>
#include <iostream>
#include <utility>

namespace sorting {

struct GlobalMetrics {
    inline static std::size_t comparisons = 0;
    inline static std::size_t moves = 0;
    inline static std::size_t allocations = 0;

    static void reset() {
        comparisons = 0;
        moves = 0;
        allocations = 0;
    }

    struct Snapshot {
        std::size_t comparisons;
        std::size_t moves;
        std::size_t allocations;
    };

    static Snapshot snapshot() {
        return {comparisons, moves, allocations};
    }
};

// For tracking comparisons and moves in algorithms
inline void increment_comparisons() { ++GlobalMetrics::comparisons; }
inline void increment_moves() { ++GlobalMetrics::moves; }
inline void increment_allocations() { ++GlobalMetrics::allocations; }

template <typename T>
struct TrackedElement {
    T value{};
    inline static std::size_t comparisons = 0;
    inline static std::size_t moves = 0;

    TrackedElement() : value{} {}
    TrackedElement(const T& val) : value(val) {}
    TrackedElement(T&& val) : value(std::move(val)) {}

    TrackedElement(const TrackedElement& other) : value(other.value) {
        ++moves;
        increment_moves();
    }

    TrackedElement(TrackedElement&& other) noexcept : value(std::move(other.value)) {
        ++moves;
        increment_moves();
    }

    TrackedElement& operator=(const TrackedElement& other) {
        if (this != &other) {
            value = other.value;
            ++moves;
            increment_moves();
        }
        return *this;
    }

    TrackedElement& operator=(TrackedElement&& other) noexcept {
        if (this != &other) {
            value = std::move(other.value);
            ++moves;
            increment_moves();
        }
        return *this;
    }

    TrackedElement& operator=(const T& val) {
        value = val;
        ++moves;
        increment_moves();
        return *this;
    }

    bool operator<(const TrackedElement& other) const {
        ++comparisons;
        increment_comparisons();
        return value < other.value;
    }

    bool operator>(const TrackedElement& other) const {
        ++comparisons;
        increment_comparisons();
        return value > other.value;
    }

    bool operator<=(const TrackedElement& other) const {
        ++comparisons;
        increment_comparisons();
        return value <= other.value;
    }

    bool operator>=(const TrackedElement& other) const {
        ++comparisons;
        increment_comparisons();
        return value >= other.value;
    }

    bool operator==(const TrackedElement& other) const {
        return value == other.value;
    }

    bool operator!=(const TrackedElement& other) const {
        return value != other.value;
    }

    static void reset() {
        comparisons = 0;
        moves = 0;
    }
};

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const TrackedElement<T>& elem) {
    return os << elem.value;
}

} // namespace sorting
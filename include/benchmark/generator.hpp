#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <random>
#include <vector>
#include <string>

namespace sorting::benchmark {

enum class DistributionType {
    Random,
    Sorted,
    ReverseSorted,
    NearlySorted,
    Duplicates
};

inline const char* distribution_name(DistributionType type) {
    switch (type) {
        case DistributionType::Random: return "Random";
        case DistributionType::Sorted: return "Sorted";
        case DistributionType::ReverseSorted: return "Reverse Sorted";
        case DistributionType::NearlySorted: return "Nearly Sorted";
        case DistributionType::Duplicates: return "High Duplicates";
    }
    return "Unknown";
}

class DataGenerator {
public:
    explicit DataGenerator(std::uint64_t seed = 42) : rng_(seed) {}

    std::vector<int> generate(DistributionType type, std::size_t n) {
        switch (type) {
            case DistributionType::Random:
                return generate_random(n);
            case DistributionType::Sorted:
                return generate_sorted(n);
            case DistributionType::ReverseSorted:
                return generate_reverse_sorted(n);
            case DistributionType::NearlySorted:
                return generate_nearly_sorted(n);
            case DistributionType::Duplicates:
                return generate_duplicates(n);
        }
        return {};
    }

    std::vector<int> generate_random(std::size_t n) {
        std::vector<int> data(n);
        std::uniform_int_distribution<int> dist(std::numeric_limits<int>::min(),
                                                   std::numeric_limits<int>::max());
        for (auto& v : data) v = dist(rng_);
        return data;
    }

    std::vector<int> generate_sorted(std::size_t n) {
        std::vector<int> data(n);
        for (std::size_t i = 0; i < n; ++i) {
            data[i] = static_cast<int>(i);
        }
        return data;
    }

    std::vector<int> generate_reverse_sorted(std::size_t n) {
        std::vector<int> data(n);
        for (std::size_t i = 0; i < n; ++i) {
            data[i] = static_cast<int>(n - 1 - i);
        }
        return data;
    }

    std::vector<int> generate_nearly_sorted(std::size_t n, double disorder_ratio = 0.05) {
        auto data = generate_sorted(n);
        const std::size_t swaps = static_cast<std::size_t>(n * disorder_ratio);
        std::uniform_int_distribution<std::size_t> dist(0, n - 1);

        for (std::size_t i = 0; i < swaps; ++i) {
            auto idx1 = dist(rng_);
            auto idx2 = dist(rng_);
            std::swap(data[idx1], data[idx2]);
        }
        return data;
    }

    std::vector<int> generate_duplicates(std::size_t n, std::size_t unique_values = 5) {
        std::vector<int> data(n);
        std::vector<int> values(unique_values);

        std::uniform_int_distribution<int> val_dist(0, 100);
        for (auto& v : values) v = val_dist(rng_);

        std::uniform_int_distribution<std::size_t> idx_dist(0, unique_values - 1);
        for (auto& v : data) v = values[idx_dist(rng_)];
        return data;
    }

    void reseed(std::uint64_t seed) { rng_.seed(seed); }

private:
    std::mt19937_64 rng_;
};

} // namespace sorting::benchmark
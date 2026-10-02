#pragma once

#include "generator.hpp"
#include "timer.hpp"
#include "../sorting/sort_interface.hpp"
#include "../sorting/metrics.hpp"
#include <vector>
#include <string>
#include <memory>
#include <chrono>

namespace sorting::benchmark {

struct BenchmarkResult {
    std::string algorithm_name;
    std::string distribution_name;
    std::size_t input_size;
    Statistics time_stats;
    std::size_t comparisons = 0;
    std::size_t moves = 0;
    std::size_t allocations = 0;
    bool skipped = false;
    std::string skip_reason;

    double mean_time_ns() const { return time_stats.mean; }
    double median_time_ns() const { return time_stats.median; }
    double stddev_time_ns() const { return time_stats.stddev; }
    double cv_percent() const { return time_stats.cv; }
};

struct BenchmarkConfig {
    std::vector<std::size_t> input_sizes = {
        100, 500, 1000, 5000, 10000, 25000, 50000, 100000, 250000, 1000000
    };
    std::vector<DistributionType> distributions = {
        DistributionType::Random,
        DistributionType::Sorted,
        DistributionType::ReverseSorted,
        DistributionType::NearlySorted,
        DistributionType::Duplicates
    };
    std::size_t warmup_runs = 3;
    std::size_t measurement_runs = 5;
    std::size_t max_n2_size = 50000;
    double max_time_per_run_seconds = 30.0;
    bool skip_n2_algorithms = true;
    std::uint64_t seed = 42;
};

class BenchmarkEngine {
public:
    explicit BenchmarkEngine(const BenchmarkConfig& config = {})
        : config_(config), timer_(config.warmup_runs, config.measurement_runs), generator_(config.seed) {}

    void add_sorter(std::unique_ptr<ISorter> sorter) {
        sorters_.push_back(std::move(sorter));
    }

    template <typename SorterType, typename... Args>
    void add_sorter(Args&&... args) {
        sorters_.push_back(std::make_unique<SorterType>(std::forward<Args>(args)...));
    }

    std::vector<BenchmarkResult> run() {
        results_.clear();

        for (auto& sorter : sorters_) {
            for (auto dist : config_.distributions) {
                for (auto size : config_.input_sizes) {
                    if (should_skip(sorter.get(), size)) {
                        results_.push_back({
                            std::string(sorter->name()),
                            std::string(distribution_name(dist)),
                            size,
                            {},
                            0, 0, 0,
                            true,
                            "Skipped: O(N^2) algorithm with N > " + std::to_string(config_.max_n2_size)
                        });
                        continue;
                    }

                    auto result = run_benchmark(sorter.get(), dist, size);
                    results_.push_back(result);
                }
            }
        }

        return results_;
    }

    const std::vector<BenchmarkResult>& get_results() const { return results_; }
    const BenchmarkConfig& get_config() const { return config_; }

private:
    bool should_skip(const ISorter* sorter, std::size_t size) const {
        if (!config_.skip_n2_algorithms) return false;

        std::string name = sorter->name();
        bool is_n2 = name.find("Bubble") != std::string::npos ||
                     name.find("Insertion") != std::string::npos;

        return is_n2 && size > config_.max_n2_size;
    }

    BenchmarkResult run_benchmark(ISorter* sorter, DistributionType dist, std::size_t size) {
        BenchmarkResult result;
        result.algorithm_name = sorter->name();
        result.distribution_name = distribution_name(dist);
        result.input_size = size;

        sorter->reset_metrics();
        GlobalMetrics::reset();

        auto data = generator_.generate(dist, size);

        auto stats = timer_.run([&]() {
            std::vector<int> copy = data;
            sorter->sort(copy);
        });

        auto metrics = sorter->get_metrics();
        result.time_stats = stats;
        result.comparisons = metrics.comparisons;
        result.moves = metrics.moves;
        result.allocations = metrics.allocations;

        return result;
    }

    BenchmarkConfig config_;
    BenchmarkTimer timer_;
    DataGenerator generator_;
    std::vector<std::unique_ptr<ISorter>> sorters_;
    std::vector<BenchmarkResult> results_;
};

} // namespace sorting::benchmark
#pragma once

#include <chrono>
#include <cmath>
#include <cstddef>
#include <vector>
#include <algorithm>
#include <numeric>

namespace sorting::benchmark {

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using Nanoseconds = std::chrono::nanoseconds;
using Microseconds = std::chrono::microseconds;
using Milliseconds = std::chrono::milliseconds;

class Timer {
public:
    Timer() = default;

    void start() { start_ = Clock::now(); }
    void stop() { end_ = Clock::now(); }

    Nanoseconds elapsed_ns() const {
        return std::chrono::duration_cast<Nanoseconds>(end_ - start_);
    }

    Microseconds elapsed_us() const {
        return std::chrono::duration_cast<Microseconds>(end_ - start_);
    }

    Milliseconds elapsed_ms() const {
        return std::chrono::duration_cast<Milliseconds>(end_ - start_);
    }

    double elapsed_seconds() const {
        return std::chrono::duration<double>(end_ - start_).count();
    }

private:
    TimePoint start_;
    TimePoint end_;
};

struct Statistics {
    double mean = 0.0;
    double median = 0.0;
    double stddev = 0.0;
    double min = 0.0;
    double max = 0.0;
    double cv = 0.0;

    static Statistics compute(const std::vector<double>& values) {
        Statistics stats;
        if (values.empty()) return stats;

        stats.min = *std::min_element(values.begin(), values.end());
        stats.max = *std::max_element(values.begin(), values.end());

        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        stats.mean = sum / values.size();

        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        std::size_t mid = sorted.size() / 2;
        stats.median = (sorted.size() % 2 == 0)
            ? (sorted[mid - 1] + sorted[mid]) / 2.0
            : sorted[mid];

        double sq_sum = 0.0;
        for (double v : values) {
            double diff = v - stats.mean;
            sq_sum += diff * diff;
        }
        stats.stddev = std::sqrt(sq_sum / values.size());

        if (stats.mean > 0) {
            stats.cv = (stats.stddev / stats.mean) * 100.0;
        }

        return stats;
    }
};

class BenchmarkTimer {
public:
    explicit BenchmarkTimer(std::size_t warmup_runs = 3, std::size_t measurement_runs = 5)
        : warmup_runs_(warmup_runs), measurement_runs_(measurement_runs) {}

    template <typename Func>
    Statistics run(Func&& func) {
        for (std::size_t i = 0; i < warmup_runs_; ++i) {
            func();
        }

        std::vector<double> times;
        times.reserve(measurement_runs_);

        for (std::size_t i = 0; i < measurement_runs_; ++i) {
            Timer timer;
            timer.start();
            func();
            timer.stop();
            times.push_back(timer.elapsed_seconds() * 1e9);
        }

        return Statistics::compute(times);
    }

    void set_warmup_runs(std::size_t runs) { warmup_runs_ = runs; }
    void set_measurement_runs(std::size_t runs) { measurement_runs_ = runs; }

private:
    std::size_t warmup_runs_;
    std::size_t measurement_runs_;
};

} // namespace sorting::benchmark
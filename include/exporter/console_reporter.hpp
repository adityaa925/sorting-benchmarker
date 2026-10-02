#pragma once

#include "../benchmark/engine.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

namespace sorting::exporter {

class ConsoleReporter {
public:
    explicit ConsoleReporter(bool use_colors = true) : use_colors_(use_colors) {}

    void print_header() const {
        std::cout << "\n";
        print_line('=', 120);
        std::cout << center("SORTING ALGORITHM BENCHMARK RESULTS", 120) << "\n";
        print_line('=', 120);
        std::cout << "\n";
    }

    void print_summary(const std::vector<sorting::benchmark::BenchmarkResult>& results) const {
        std::cout << std::left << std::setw(25) << "Algorithm"
                  << std::setw(18) << "Distribution"
                  << std::setw(10) << "Size"
                  << std::setw(15) << "Mean (ns)"
                  << std::setw(15) << "Median (ns)"
                  << std::setw(12) << "StdDev (ns)"
                  << std::setw(8) << "CV%"
                  << std::setw(15) << "Comparisons"
                  << std::setw(12) << "Moves"
                  << "Status" << "\n";
        print_line('-', 120);

        for (const auto& r : results) {
            print_row(r);
        }
        print_line('-', 120);
    }

    void print_algorithm_summary(const std::vector<sorting::benchmark::BenchmarkResult>& results) const {
        std::cout << "\n";
        print_line('=', 120);
        std::cout << center("ALGORITHM PERFORMANCE SUMMARY", 120) << "\n";
        print_line('=', 120);

        std::map<std::string, std::vector<const sorting::benchmark::BenchmarkResult*>> by_algo;
        for (const auto& r : results) {
            if (!r.skipped) {
                by_algo[r.algorithm_name].push_back(&r);
            }
        }

        for (const auto& pair : by_algo) {
            const std::string& algo = pair.first;
            const auto& vec = pair.second;

            std::cout << "\n" << algo << ":\n";
            std::cout << std::left << std::setw(18) << "Distribution"
                      << std::setw(10) << "Size"
                      << std::setw(15) << "Mean (ns)"
                      << std::setw(15) << "Median (ns)"
                      << std::setw(15) << "Comparisons"
                      << std::setw(12) << "Moves" << "\n";
            print_line('-', 85);

            for (const auto* r : vec) {
                std::cout << std::left << std::setw(18) << r->distribution_name
                          << std::setw(10) << r->input_size
                          << std::setw(15) << format_number(r->mean_time_ns())
                          << std::setw(15) << format_number(r->median_time_ns())
                          << std::setw(15) << format_number(static_cast<double>(r->comparisons))
                          << std::setw(12) << format_number(static_cast<double>(r->moves)) << "\n";
            }
        }
    }

    void print_comparison_table(const std::vector<sorting::benchmark::BenchmarkResult>& results,
                                 std::size_t size,
                                 sorting::benchmark::DistributionType dist) const {
        std::cout << "\n";
        print_line('=', 120);
        std::cout << center("COMPARISON AT N=" + std::to_string(size) +
                           " (" + std::string(sorting::benchmark::distribution_name(dist)) + ")", 120) << "\n";
        print_line('=', 120);

        std::cout << std::left << std::setw(30) << "Algorithm"
                  << std::setw(15) << "Mean (ns)"
                  << std::setw(15) << "Median (ns)"
                  << std::setw(12) << "CV%"
                  << std::setw(15) << "Comparisons"
                  << std::setw(12) << "Moves"
                  << "Speedup vs std::sort" << "\n";
        print_line('-', 120);

        double baseline_mean = 0.0;
        for (const auto& r : results) {
            if (r.input_size == size && r.distribution_name == sorting::benchmark::distribution_name(dist) &&
                r.algorithm_name == "std::sort (Introsort)") {
                baseline_mean = r.mean_time_ns();
                break;
            }
        }

        for (const auto& r : results) {
            if (r.input_size == size && r.distribution_name == sorting::benchmark::distribution_name(dist) && !r.skipped) {
                double speedup = baseline_mean > 0 ? baseline_mean / r.mean_time_ns() : 0.0;
                std::string speedup_str = (r.algorithm_name == "std::sort (Introsort)") ? "1.00x (baseline)"
                    : std::to_string(speedup).substr(0, 5) + "x";

                std::cout << std::left << std::setw(30) << r.algorithm_name
                          << std::setw(15) << format_number(r.mean_time_ns())
                          << std::setw(15) << format_number(r.median_time_ns())
                          << std::setw(12) << std::fixed << std::setprecision(2) << r.cv_percent()
                          << std::setw(15) << format_number(static_cast<double>(r.comparisons))
                          << std::setw(12) << format_number(static_cast<double>(r.moves))
                          << speedup_str << "\n";
            }
        }
        print_line('-', 120);
    }

private:
    [[maybe_unused]] bool use_colors_;

    void print_row(const sorting::benchmark::BenchmarkResult& r) const {
        std::string status = r.skipped ? "SKIPPED" : "OK";
        if (r.skipped) status += ": " + r.skip_reason;

        std::cout << std::left << std::setw(25) << truncate(r.algorithm_name, 24)
                  << std::setw(18) << truncate(r.distribution_name, 17)
                  << std::setw(10) << r.input_size;

        if (r.skipped) {
            std::cout << std::setw(50) << "" << status << "\n";
            return;
        }

        std::cout << std::setw(15) << format_number(r.mean_time_ns())
                  << std::setw(15) << format_number(r.median_time_ns())
                  << std::setw(12) << format_number(r.stddev_time_ns())
                  << std::setw(8) << std::fixed << std::setprecision(2) << r.cv_percent()
                  << std::setw(15) << format_number(static_cast<double>(r.comparisons))
                  << std::setw(12) << format_number(static_cast<double>(r.moves))
                  << status << "\n";
    }

    std::string format_number(double value) const {
        if (value >= 1e9) return std::to_string(value / 1e9).substr(0, 6) + "B";
        if (value >= 1e6) return std::to_string(value / 1e6).substr(0, 6) + "M";
        if (value >= 1e3) return std::to_string(value / 1e3).substr(0, 6) + "K";
        return std::to_string(value).substr(0, 8);
    }

    std::string truncate(const std::string& str, std::size_t max_len) const {
        return str.length() > max_len ? str.substr(0, max_len - 1) + "..." : str;
    }

    std::string center(const std::string& str, std::size_t width) const {
        if (str.length() >= width) return str;
        std::size_t pad = (width - str.length()) / 2;
        return std::string(pad, ' ') + str;
    }

    void print_line(char c, std::size_t len) const {
        std::cout << std::string(len, c) << "\n";
    }
};

} // namespace sorting::exporter
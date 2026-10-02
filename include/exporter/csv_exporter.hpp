#pragma once

#include "../benchmark/engine.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace sorting::exporter {

class CSVExporter {
public:
    explicit CSVExporter(const std::string& filename = "benchmark_results.csv")
        : filename_(filename) {}

    void export_results(const std::vector<sorting::benchmark::BenchmarkResult>& results) {
        std::ofstream file(filename_);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open CSV file: " + filename_);
        }

        write_header(file);
        write_data(file, results);
    }

    void export_summary(const std::vector<sorting::benchmark::BenchmarkResult>& results,
                        const std::string& summary_filename = "benchmark_summary.csv") {
        std::ofstream file(summary_filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open CSV file: " + summary_filename);
        }

        file << "Algorithm,Distribution,Size,Mean_ns,Median_ns,StdDev_ns,CV_percent,"
                "Comparisons,Moves,Allocations,Skipped,SkipReason\n";

        for (const auto& r : results) {
            file << escape(r.algorithm_name) << ","
                 << escape(r.distribution_name) << ","
                 << r.input_size << ","
                 << (r.skipped ? "" : std::to_string(r.mean_time_ns())) << ","
                 << (r.skipped ? "" : std::to_string(r.median_time_ns())) << ","
                 << (r.skipped ? "" : std::to_string(r.stddev_time_ns())) << ","
                 << (r.skipped ? "" : std::to_string(r.cv_percent())) << ","
                 << r.comparisons << ","
                 << r.moves << ","
                 << r.allocations << ","
                 << (r.skipped ? "true" : "false") << ","
                 << escape(r.skip_reason) << "\n";
        }
    }

private:
    std::string filename_;

    void write_header(std::ofstream& file) {
        file << "Algorithm,Distribution,InputSize,MeanTime_ns,MedianTime_ns,StdDev_ns,CV_percent,"
                "Comparisons,Moves,Allocations,Skipped,SkipReason\n";
    }

    void write_data(std::ofstream& file, const std::vector<sorting::benchmark::BenchmarkResult>& results) {
        for (const auto& r : results) {
            file << escape(r.algorithm_name) << ","
                 << escape(r.distribution_name) << ","
                 << r.input_size << ","
                 << (r.skipped ? "" : std::to_string(r.mean_time_ns())) << ","
                 << (r.skipped ? "" : std::to_string(r.median_time_ns())) << ","
                 << (r.skipped ? "" : std::to_string(r.stddev_time_ns())) << ","
                 << (r.skipped ? "" : std::to_string(r.cv_percent())) << ","
                 << r.comparisons << ","
                 << r.moves << ","
                 << r.allocations << ","
                 << (r.skipped ? "true" : "false") << ","
                 << escape(r.skip_reason) << "\n";
        }
    }

    std::string escape(const std::string& str) {
        if (str.find(',') != std::string::npos ||
            str.find('"') != std::string::npos ||
            str.find('\n') != std::string::npos) {
            std::string escaped = "\"";
            for (char c : str) {
                if (c == '"') escaped += "\"\"";
                else escaped += c;
            }
            escaped += "\"";
            return escaped;
        }
        return str;
    }
};

} // namespace sorting::exporter
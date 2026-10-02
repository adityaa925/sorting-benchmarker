#pragma once

#include "../benchmark/engine.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>

namespace sorting::exporter {

class JSONExporter {
public:
    explicit JSONExporter(const std::string& filename = "benchmark_results.json")
        : filename_(filename) {}

    void export_results(const std::vector<sorting::benchmark::BenchmarkResult>& results,
                        const sorting::benchmark::BenchmarkConfig& config) {
        std::ofstream file(filename_);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open JSON file: " + filename_);
        }

        file << "{\n";
        file << "  \"metadata\": {\n";
        file << "    \"timestamp\": \"" << current_timestamp() << "\",\n";
        file << "    \"version\": \"1.0.0\",\n";
        file << "    \"config\": {\n";
        file << "      \"warmup_runs\": " << config.warmup_runs << ",\n";
        file << "      \"measurement_runs\": " << config.measurement_runs << ",\n";
        file << "      \"max_n2_size\": " << config.max_n2_size << ",\n";
        file << "      \"max_time_per_run_seconds\": " << config.max_time_per_run_seconds << ",\n";
        file << "      \"input_sizes\": " << array_to_json(config.input_sizes) << ",\n";
        file << "      \"distributions\": " << distributions_to_json(config.distributions) << "\n";
        file << "    }\n";
        file << "  },\n";
        file << "  \"results\": [\n";

        for (std::size_t i = 0; i < results.size(); ++i) {
            const auto& r = results[i];
            file << "    {\n";
            file << "      \"algorithm\": \"" << escape_json(r.algorithm_name) << "\",\n";
            file << "      \"distribution\": \"" << escape_json(r.distribution_name) << "\",\n";
            file << "      \"input_size\": " << r.input_size << ",\n";
            file << "      \"skipped\": " << (r.skipped ? "true" : "false") << ",\n";

            if (!r.skipped) {
                file << "      \"time_ns\": {\n";
                file << "        \"mean\": " << r.mean_time_ns() << ",\n";
                file << "        \"median\": " << r.median_time_ns() << ",\n";
                file << "        \"stddev\": " << r.stddev_time_ns() << ",\n";
                file << "        \"min\": " << r.time_stats.min << ",\n";
                file << "        \"max\": " << r.time_stats.max << ",\n";
                file << "        \"cv_percent\": " << r.cv_percent() << "\n";
                file << "      },\n";
                file << "      \"metrics\": {\n";
                file << "        \"comparisons\": " << r.comparisons << ",\n";
                file << "        \"moves\": " << r.moves << ",\n";
                file << "        \"allocations\": " << r.allocations << "\n";
                file << "      }\n";
            } else {
                file << "      \"skip_reason\": \"" << escape_json(r.skip_reason) << "\"\n";
            }

            file << "    }" << (i + 1 < results.size() ? "," : "") << "\n";
        }

        file << "  ]\n";
        file << "}\n";
    }

private:
    std::string filename_;

    std::string current_timestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
        return ss.str();
    }

    std::string array_to_json(const std::vector<std::size_t>& arr) {
        std::stringstream ss;
        ss << "[";
        for (std::size_t i = 0; i < arr.size(); ++i) {
            ss << arr[i] << (i + 1 < arr.size() ? ", " : "");
        }
        ss << "]";
        return ss.str();
    }

    std::string distributions_to_json(const std::vector<sorting::benchmark::DistributionType>& dists) {
        std::stringstream ss;
        ss << "[";
        for (std::size_t i = 0; i < dists.size(); ++i) {
            ss << "\"" << sorting::benchmark::distribution_name(dists[i]) << "\"" << (i + 1 < dists.size() ? ", " : "");
        }
        ss << "]";
        return ss.str();
    }

    std::string escape_json(const std::string& str) {
        std::string escaped;
        escaped.reserve(str.size() + 10);
        for (char c : str) {
            switch (c) {
                case '"': escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\b': escaped += "\\b"; break;
                case '\f': escaped += "\\f"; break;
                case '\n': escaped += "\\n"; break;
                case '\r': escaped += "\\r"; break;
                case '\t': escaped += "\\t"; break;
                default: escaped += c;
            }
        }
        return escaped;
    }
};

} // namespace sorting::exporter
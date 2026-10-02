#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "sorting/sort_interface.hpp"
#include "sorting/bubble_sort.hpp"
#include "sorting/insertion_sort.hpp"
#include "sorting/merge_sort.hpp"
#include "sorting/quick_sort.hpp"
#include "sorting/std_sort_wrapper.hpp"
#include "benchmark/generator.hpp"
#include "benchmark/engine.hpp"
#include "exporter/console_reporter.hpp"
#include "exporter/csv_exporter.hpp"
#include "exporter/json_exporter.hpp"

using namespace sorting;
using namespace sorting::benchmark;
using namespace sorting::exporter;

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --sizes N1,N2,...       Input sizes to test (default: 100,500,1000,5000,10000,25000,50000,100000,250000,1000000)\n";
    std::cout << "  --distributions D1,D2.. Distributions to test: random,sorted,reverse,nearly,duplicates (default: all)\n";
    std::cout << "  --algorithms A1,A2,...  Algorithms to test: bubble,optbubble,insertion,bininsertion,merge,bottomupmerge,quick,quick3,std,stable (default: all)\n";
    std::cout << "  --warmup N              Warmup runs (default: 3)\n";
    std::cout << "  --runs N                Measurement runs (default: 5)\n";
    std::cout << "  --max-n2-size N         Max size for O(N^2) algorithms (default: 50000)\n";
    std::cout << "  --no-skip-n2            Don't skip O(N^2) algorithms for large N\n";
    std::cout << "  --csv FILE              Export CSV results (default: benchmark_results.csv)\n";
    std::cout << "  --json FILE             Export JSON results (default: benchmark_results.json)\n";
    std::cout << "  --html FILE             Export HTML visualization (default: report.html)\n";
    std::cout << "  --seed N                Random seed (default: 42)\n";
    std::cout << "  --no-color              Disable colored output\n";
    std::cout << "  --help, -h              Show this help message\n";
}

std::vector<std::size_t> parse_sizes(const std::string& str) {
    std::vector<std::size_t> sizes;
    std::string token;
    for (char c : str) {
        if (c == ',') {
            if (!token.empty()) {
                sizes.push_back(std::stoull(token));
                token.clear();
            }
        } else {
            token += c;
        }
    }
    if (!token.empty()) sizes.push_back(std::stoull(token));
    return sizes;
}

std::vector<DistributionType> parse_distributions(const std::string& str) {
    std::vector<DistributionType> dists;
    std::string token;
    auto add_dist = [&](const std::string& t) {
        if (t == "random") dists.push_back(DistributionType::Random);
        else if (t == "sorted") dists.push_back(DistributionType::Sorted);
        else if (t == "reverse") dists.push_back(DistributionType::ReverseSorted);
        else if (t == "nearly") dists.push_back(DistributionType::NearlySorted);
        else if (t == "duplicates") dists.push_back(DistributionType::Duplicates);
    };

    for (char c : str) {
        if (c == ',') {
            if (!token.empty()) {
                add_dist(token);
                token.clear();
            }
        } else {
            token += c;
        }
    }
    if (!token.empty()) add_dist(token);
    return dists;
}

std::vector<std::string> parse_algorithms(const std::string& str) {
    std::vector<std::string> algos;
    std::string token;
    for (char c : str) {
        if (c == ',') {
            if (!token.empty()) {
                algos.push_back(token);
                token.clear();
            }
        } else {
            token += c;
        }
    }
    if (!token.empty()) algos.push_back(token);
    return algos;
}

std::string current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%d %H:%M:%S UTC");
    return ss.str();
}

std::string build_json_string(const std::vector<BenchmarkResult>& results, const BenchmarkConfig& config) {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"metadata\": {\n";
    ss << "    \"timestamp\": \"" << current_timestamp() << "\",\n";
    ss << "    \"version\": \"1.0.0\",\n";
    ss << "    \"config\": {\n";
    ss << "      \"warmup_runs\": " << config.warmup_runs << ",\n";
    ss << "      \"measurement_runs\": " << config.measurement_runs << ",\n";
    ss << "      \"max_n2_size\": " << config.max_n2_size << ",\n";
    ss << "      \"max_time_per_run_seconds\": " << config.max_time_per_run_seconds << "\n";
    ss << "    }\n";
    ss << "  },\n";
    ss << "  \"results\": [\n";

    for (std::size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        ss << "    {\n";
        ss << "      \"algorithm\": \"" << r.algorithm_name << "\",\n";
        ss << "      \"distribution\": \"" << r.distribution_name << "\",\n";
        ss << "      \"input_size\": " << r.input_size << ",\n";
        ss << "      \"skipped\": " << (r.skipped ? "true" : "false") << ",\n";

        if (!r.skipped) {
            ss << "      \"time_ns\": {\n";
            ss << "        \"mean\": " << r.mean_time_ns() << ",\n";
            ss << "        \"median\": " << r.median_time_ns() << ",\n";
            ss << "        \"stddev\": " << r.stddev_time_ns() << ",\n";
            ss << "        \"min\": " << r.time_stats.min << ",\n";
            ss << "        \"max\": " << r.time_stats.max << ",\n";
            ss << "        \"cv_percent\": " << r.cv_percent() << "\n";
            ss << "      },\n";
            ss << "      \"metrics\": {\n";
            ss << "        \"comparisons\": " << r.comparisons << ",\n";
            ss << "        \"moves\": " << r.moves << ",\n";
            ss << "        \"allocations\": " << r.allocations << "\n";
            ss << "      }\n";
        } else {
            ss << "      \"skip_reason\": \"" << r.skip_reason << "\"\n";
        }

        ss << "    }" << (i + 1 < results.size() ? "," : "") << "\n";
    }
    ss << "  ]\n";
    ss << "}\n";
    return ss.str();
}

void generate_html_report(const std::vector<BenchmarkResult>& results,
                          const BenchmarkConfig& config,
                          const std::string& filename) {
    std::string json_data = build_json_string(results, config);

    std::vector<std::string> template_candidates = {
        "visualization/template/dashboard.html",
        "../visualization/template/dashboard.html",
        "../../visualization/template/dashboard.html"
    };

    std::string template_content;
    for (const auto& path : template_candidates) {
        std::ifstream t_file(path);
        if (t_file.is_open()) {
            std::stringstream buffer;
            buffer << t_file.rdbuf();
            template_content = buffer.str();
            break;
        }
    }

    std::ofstream out_file(filename);
    if (!out_file.is_open()) {
        std::cerr << "Warning: Could not open output file: " << filename << "\n";
        return;
    }

    if (!template_content.empty()) {
        std::string placeholder = "/*{{BENCHMARK_DATA}}*/ null";
        auto pos = template_content.find(placeholder);
        if (pos != std::string::npos) {
            template_content.replace(pos, placeholder.length(), json_data);
        } else {
            std::string alt_placeholder = "/*{{BENCHMARK_DATA}}*/";
            pos = template_content.find(alt_placeholder);
            if (pos != std::string::npos) {
                template_content.replace(pos, alt_placeholder.length(), json_data);
            }
        }
        out_file << template_content;
    } else {
        // Fallback self-contained HTML
        out_file << "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Benchmark Report</title>"
                 << "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script></head><body>"
                 << "<h1>Benchmark Results</h1><pre>" << json_data << "</pre></body></html>";
    }
}

int main(int argc, char* argv[]) {
    BenchmarkConfig config;
    std::string csv_file = "benchmark_results.csv";
    std::string json_file = "benchmark_results.json";
    std::string html_file = "report.html";
    std::uint64_t seed = 42;
    bool use_colors = true;
    bool run_correctness_tests = true;
    std::vector<std::string> selected_algos;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        } else if (arg == "--sizes" && i + 1 < argc) {
            config.input_sizes = parse_sizes(argv[++i]);
        } else if (arg == "--distributions" && i + 1 < argc) {
            config.distributions = parse_distributions(argv[++i]);
        } else if (arg == "--algorithms" && i + 1 < argc) {
            selected_algos = parse_algorithms(argv[++i]);
        } else if (arg == "--warmup" && i + 1 < argc) {
            config.warmup_runs = std::stoull(argv[++i]);
        } else if (arg == "--runs" && i + 1 < argc) {
            config.measurement_runs = std::stoull(argv[++i]);
        } else if (arg == "--max-n2-size" && i + 1 < argc) {
            config.max_n2_size = std::stoull(argv[++i]);
        } else if (arg == "--no-skip-n2") {
            config.skip_n2_algorithms = false;
        } else if (arg == "--csv" && i + 1 < argc) {
            csv_file = argv[++i];
        } else if (arg == "--json" && i + 1 < argc) {
            json_file = argv[++i];
        } else if (arg == "--html" && i + 1 < argc) {
            html_file = argv[++i];
        } else if (arg == "--seed" && i + 1 < argc) {
            seed = std::stoull(argv[++i]);
        } else if (arg == "--no-color") {
            use_colors = false;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            print_usage(argv[0]);
            return 1;
        }
    }

    if (config.distributions.empty()) {
        config.distributions = {
            DistributionType::Random,
            DistributionType::Sorted,
            DistributionType::ReverseSorted,
            DistributionType::NearlySorted,
            DistributionType::Duplicates
        };
    }

    config.seed = seed;

    // Run correctness tests first
    if (run_correctness_tests) {
        std::cout << "Running pre-benchmark verification tests...\n";
        std::vector<int> test_data = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
        std::vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        std::vector<std::unique_ptr<ISorter>> test_sorters;
        test_sorters.push_back(std::make_unique<BubbleSort<int>>());
        test_sorters.push_back(std::make_unique<InsertionSort<int>>());
        test_sorters.push_back(std::make_unique<MergeSort<int>>());
        test_sorters.push_back(std::make_unique<QuickSort<int>>());
        test_sorters.push_back(std::make_unique<QuickSort3Way<int>>());
        test_sorters.push_back(std::make_unique<StdSortWrapper<int>>());
        test_sorters.push_back(std::make_unique<StdStableSortWrapper<int>>());

        bool all_passed = true;
        for (auto& sorter : test_sorters) {
            auto data = test_data;
            sorter->sort(data.begin(), data.end());
            if (data != expected) {
                std::cerr << "FAIL: " << sorter->name() << " produced incorrect result\n";
                all_passed = false;
            } else {
                std::cout << "  PASS: " << sorter->name() << "\n";
            }
        }

        if (!all_passed) {
            std::cerr << "Correctness tests FAILED!\n";
            return 1;
        }
        std::cout << "All pre-benchmark verification tests PASSED!\n\n";
    }

    // Setup benchmark engine
    BenchmarkEngine engine(config);
    auto should_add = [&](const std::string& key) {
        if (selected_algos.empty()) return true;
        for (const auto& a : selected_algos) {
            if (a == key) return true;
        }
        return false;
    };

    if (should_add("bubble")) engine.add_sorter<BubbleSort<int>>();
    if (should_add("optbubble")) engine.add_sorter<OptimizedBubbleSort<int>>();
    if (should_add("insertion")) engine.add_sorter<InsertionSort<int>>();
    if (should_add("bininsertion")) engine.add_sorter<BinaryInsertionSort<int>>();
    if (should_add("merge")) engine.add_sorter<MergeSort<int>>();
    if (should_add("bottomupmerge")) engine.add_sorter<BottomUpMergeSort<int>>();
    if (should_add("quick")) engine.add_sorter<QuickSort<int>>();
    if (should_add("quick3")) engine.add_sorter<QuickSort3Way<int>>();
    if (should_add("std")) engine.add_sorter<StdSortWrapper<int>>();
    if (should_add("stable")) engine.add_sorter<StdStableSortWrapper<int>>();

    // Run benchmarks
    std::cout << "Starting benchmark with configuration:\n";
    std::cout << "  Sizes: ";
    for (auto s : config.input_sizes) std::cout << s << " ";
    std::cout << "\n  Distributions: ";
    for (auto d : config.distributions) std::cout << distribution_name(d) << " ";
    std::cout << "\n  Warmup runs: " << config.warmup_runs;
    std::cout << "\n  Measurement runs: " << config.measurement_runs;
    std::cout << "\n  Max N^2 size: " << config.max_n2_size;
    std::cout << "\n  Skip O(N^2): " << (config.skip_n2_algorithms ? "yes" : "no");
    std::cout << "\n\n";

    auto start = std::chrono::high_resolution_clock::now();
    auto results = engine.run();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

    std::cout << "Benchmark completed in " << duration << " seconds\n\n";

    // Report results
    ConsoleReporter reporter(use_colors);
    reporter.print_header();
    reporter.print_summary(results);
    reporter.print_algorithm_summary(results);

    // Print comparison tables for key sizes if present
    for (std::size_t size : {1000ULL, 10000ULL, 50000ULL}) {
        for (auto dist : config.distributions) {
            bool has_data = false;
            for (const auto& r : results) {
                if (r.input_size == size && r.distribution_name == distribution_name(dist) && !r.skipped) {
                    has_data = true;
                    break;
                }
            }
            if (has_data) {
                reporter.print_comparison_table(results, size, dist);
            }
        }
    }

    // Export results
    try {
        CSVExporter csv_exporter(csv_file);
        csv_exporter.export_results(results);
        std::cout << "\nResults exported to " << csv_file << "\n";

        JSONExporter json_exporter(json_file);
        json_exporter.export_results(results, config);
        std::cout << "Results exported to " << json_file << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Export error: " << e.what() << "\n";
        return 1;
    }

    // Generate HTML report
    std::cout << "Generating HTML report...\n";
    generate_html_report(results, config, html_file);
    std::cout << "HTML report generated: " << html_file << "\n";

    return 0;
}
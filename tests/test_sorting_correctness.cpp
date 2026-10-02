#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>

#include "sorting/bubble_sort.hpp"
#include "sorting/insertion_sort.hpp"
#include "sorting/merge_sort.hpp"
#include "sorting/quick_sort.hpp"
#include "sorting/std_sort_wrapper.hpp"
#include "sorting/metrics.hpp"

using namespace sorting;

template <typename Sorter>
void test_sorter(const std::string& name) {
    Sorter sorter;

    // Test empty
    {
        std::vector<int> data;
        sorter.sort(data.begin(), data.end());
        assert(data.empty());
    }

    // Test single element
    {
        std::vector<int> data = {42};
        sorter.sort(data.begin(), data.end());
        assert(data.size() == 1 && data[0] == 42);
    }

    // Test already sorted
    {
        std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        sorter.sort(data.begin(), data.end());
        assert(std::is_sorted(data.begin(), data.end()));
    }

    // Test reverse sorted
    {
        std::vector<int> data = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
        sorter.sort(data.begin(), data.end());
        assert(std::is_sorted(data.begin(), data.end()));
    }

    // Test random
    {
        std::vector<int> data(100);
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(-1000, 1000);
        for (auto& v : data) v = dist(rng);
        sorter.sort(data.begin(), data.end());
        assert(std::is_sorted(data.begin(), data.end()));
    }

    // Test duplicates
    {
        std::vector<int> data = {5, 3, 5, 1, 3, 5, 1, 3, 1, 5};
        sorter.sort(data.begin(), data.end());
        assert(std::is_sorted(data.begin(), data.end()));
        int count_1 = static_cast<int>(std::count(data.begin(), data.end(), 1));
        int count_3 = static_cast<int>(std::count(data.begin(), data.end(), 3));
        int count_5 = static_cast<int>(std::count(data.begin(), data.end(), 5));
        (void)count_1; (void)count_3; (void)count_5;
        assert(count_1 == 3 && count_3 == 3 && count_5 == 4);
    }

    // Test large random
    {
        std::vector<int> data(10000);
        std::mt19937 rng(123);
        std::uniform_int_distribution<int> dist(-100000, 100000);
        for (auto& v : data) v = dist(rng);
        sorter.sort(data.begin(), data.end());
        assert(std::is_sorted(data.begin(), data.end()));
    }

    // Test with TrackedElement
    {
        std::vector<TrackedElement<int>> data(100);
        std::mt19937 rng(456);
        std::uniform_int_distribution<int> dist(-1000, 1000);
        for (auto& v : data) v.value = dist(rng);
        sorter.sort(data.begin(), data.end());
        for (size_t i = 1; i < data.size(); ++i) {
            assert(data[i-1].value <= data[i].value);
        }
    }

    std::cout << "PASS: " << name << "\n";
}

void test_stability() {
    struct Item {
        int key;
        int value;
        bool operator<(const Item& other) const { return key < other.key; }
        bool operator==(const Item& other) const { return key == other.key && value == other.value; }
    };

    std::vector<Item> data = {{2, 1}, {1, 1}, {2, 2}, {1, 2}, {3, 1}};
    std::vector<Item> expected = {{1, 1}, {1, 2}, {2, 1}, {2, 2}, {3, 1}};

    MergeSort<Item> merge;
    merge.sort(data.begin(), data.end());
    assert(data == expected);

    StdStableSortWrapper<Item> stable;
    data = {{2, 1}, {1, 1}, {2, 2}, {1, 2}, {3, 1}};
    stable.sort(data.begin(), data.end());
    assert(data == expected);

    std::cout << "PASS: Stability test\n";
}

void test_metrics_tracking() {
    GlobalMetrics::reset();

    std::vector<TrackedElement<int>> data = {5, 2, 8, 1, 9, 3};
    BubbleSort<TrackedElement<int>> sorter;
    sorter.sort(data.begin(), data.end());

    auto metrics = sorter.get_metrics();
    assert(metrics.comparisons > 0);
    assert(metrics.moves > 0);

    std::cout << "PASS: Metrics tracking\n";
    std::cout << "  Comparisons: " << metrics.comparisons << "\n";
    std::cout << "  Moves: " << metrics.moves << "\n";
}

void test_algorithm_properties() {
    BubbleSort<int> bubble;
    InsertionSort<int> insertion;
    MergeSort<int> merge;
    QuickSort<int> quick;
    StdSortWrapper<int> std_sort;
    StdStableSortWrapper<int> std_stable;

    assert(bubble.is_stable() == true);
    assert(insertion.is_stable() == true);
    assert(merge.is_stable() == true);
    assert(quick.is_stable() == false);
    assert(std_sort.is_stable() == false);
    assert(std_stable.is_stable() == true);

    assert(bubble.is_in_place() == true);
    assert(insertion.is_in_place() == true);
    assert(merge.is_in_place() == false);
    assert(quick.is_in_place() == true);
    assert(std_sort.is_in_place() == true);
    assert(std_stable.is_in_place() == false);

    std::cout << "PASS: Algorithm properties\n";
}

int main() {
    std::cout << "Running sorting algorithm correctness tests...\n\n";

    test_sorter<BubbleSort<int>>("Bubble Sort");
    test_sorter<InsertionSort<int>>("Insertion Sort");
    test_sorter<MergeSort<int>>("Merge Sort");
    test_sorter<QuickSort<int>>("Quick Sort");
    test_sorter<QuickSort3Way<int>>("Quick Sort 3-Way");
    test_sorter<StdSortWrapper<int>>("std::sort");
    test_sorter<StdStableSortWrapper<int>>("std::stable_sort");

    test_stability();
    test_metrics_tracking();
    test_algorithm_properties();

    std::cout << "\nAll tests PASSED!\n";
    return 0;
}
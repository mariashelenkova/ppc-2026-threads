#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <limits>
#include <random>
#include <vector>

#include "performance/include/performance.hpp"
#include "shelenkova_m_shell_sort_simple_merge/common/include/common.hpp"
#include "shelenkova_m_shell_sort_simple_merge/omp/include/ops_omp.hpp"
#include "shelenkova_m_shell_sort_simple_merge/seq/include/ops_seq.hpp"
#include "shelenkova_m_shell_sort_simple_merge/stl/include/ops_stl.hpp"
#include "shelenkova_m_shell_sort_simple_merge/tbb/include/ops_tbb.hpp"
#include "util/include/perf_test_util.hpp"

namespace shelenkova_m_shell_sort_simple_merge {

namespace {

constexpr double kConversionFactor = 1e-9;
constexpr int kLowerBound = -1000000;
constexpr int kUpperBound = 1000000;
constexpr size_t kDatasetSize = 100000;

}  // namespace

class ShellSortPerformanceEvaluation : public ppc::util::BaseRunPerfTests<InType, OutType> {
  static constexpr size_t kArraySize = kDatasetSize;
  InType test_data_;

 protected:
  void ConfigurePerformanceMetrics(ppc::performance::PerfAttr& metrics) override {
    const auto start_time = std::chrono::steady_clock::now();
    metrics.current_timer = [start_time] {
      auto current_time = std::chrono::steady_clock::now();
      auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time - start_time).count();
      return static_cast<double>(elapsed_ns) * kConversionFactor;
    };
    metrics.num_running = 5;
  }

  void InitializeTestData() override {
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<int> distribution(kLowerBound, kUpperBound);

    test_data_.resize(kArraySize);
    for (size_t i = 0; i < kArraySize; ++i) {
      test_data_[i] = distribution(generator);
    }
  }

  bool VerifyResult(OutType& result) final {
    if (result.size() != test_data_.size()) {
      return false;
    }
    return std::is_sorted(result.begin(), result.end());
  }

  InType ObtainInputData() final {
    return test_data_;
  }
};

TEST_P(ShellSortPerformanceEvaluation, MeasureSortingPerformance) {
  ExecuteTest(GetParam());
}

namespace {

const auto kAllPerformanceTasks =
    ppc::util::MakeAllPerfTasks<InType, ShelenkovaMShellSortSimpleMergeSEQ, ShelenkovaMShellSortSimpleMergeOMP,
                                ShelenkovaMShellSortSimpleMergeTBB, ShelenkovaMShellSortSimpleMergeSTL>(
        PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerformanceTasks);

const auto kPerformanceTestLabel = ShellSortPerformanceEvaluation::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(ShellSortPerformanceTests, ShellSortPerformanceEvaluation, kGtestValues, kPerformanceTestLabel);

}  // namespace

}  // namespace shelenkova_m_shell_sort_simple_merge

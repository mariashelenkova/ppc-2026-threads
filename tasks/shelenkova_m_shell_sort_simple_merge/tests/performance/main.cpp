#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <limits>
#include <random>

#include "shelenkova_m_shell_sort_simple_merge/all/include/ops_all.hpp"
#include "shelenkova_m_shell_sort_simple_merge/common/include/common.hpp"
#include "shelenkova_m_shell_sort_simple_merge/omp/include/ops_omp.hpp"
#include "shelenkova_m_shell_sort_simple_merge/seq/include/ops_seq.hpp"
#include "shelenkova_m_shell_sort_simple_merge/stl/include/ops_stl.hpp"
#include "shelenkova_m_shell_sort_simple_merge/tbb/include/ops_tbb.hpp"
#include "util/include/perf_test_util.hpp"

namespace shelenkova_m_shell_sort_simple_merge {

class ShelenkovaMShellSortSimpleMergeRunPerfTest : public ppc::util::BaseRunPerfTests<InType, OutType> {
 protected:
  void SetUp() override {
    int total_elements = 2000000;

    InType &source_vector = input_data_;
    source_vector.clear();
    source_vector.reserve(total_elements);

    int generator_seed = 777;
    std::mt19937 random_generator(generator_seed);
    std::uniform_int_distribution<int> value_distribution(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

    for (int element_index = 0; element_index < total_elements; ++element_index) {
      source_vector.push_back(value_distribution(random_generator));
    }

    sorted_reference_ = source_vector;
    std::ranges::sort(sorted_reference_);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    if (output_data.size() != sorted_reference_.size()) {
      return false;
    }
    for (std::size_t idx = 0; idx < output_data.size(); ++idx) {
      if (output_data[idx] != sorted_reference_[idx]) {
        return false;
      }
    }
    return true;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  OutType sorted_reference_;
};

TEST_P(ShelenkovaMShellSortSimpleMergeRunPerfTest, PerformanceSortTest) {
  ExecuteTest(GetParam());
}

namespace {

const auto kAllPerformanceTasks =
    ppc::util::MakeAllPerfTasks<InType, ShelenkovaMShellSortSimpleMergeSEQ, ShelenkovaMShellSortSimpleMergeOMP, 
                                ShelenkovaMShellSortSimpleMergeTBB, ShelenkovaMShellSortSimpleMergeSTL,
                                ShelenkovaMShellSortSimpleMergeALL>(PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge);

const auto kGtestPerformanceValues = ppc::util::TupleToGTestValues(kAllPerformanceTasks);

const auto kPerformanceTestName = ShelenkovaMShellSortSimpleMergeRunPerfTest::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(shellSortSimpleMergePerfTests, ShelenkovaMShellSortSimpleMergeRunPerfTest, 
                         kGtestPerformanceValues, kPerformanceTestName);

}  // namespace

}  // namespace shelenkova_m_shell_sort_simple_merge

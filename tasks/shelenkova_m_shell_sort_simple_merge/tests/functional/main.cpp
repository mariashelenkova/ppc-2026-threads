#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <limits>
#include <random>
#include <string>
#include <tuple>
#include <vector>

#include "shelenkova_m_shell_sort_simple_merge/common/include/common.hpp"
#include "shelenkova_m_shell_sort_simple_merge/omp/include/ops_omp.hpp"
#include "shelenkova_m_shell_sort_simple_merge/seq/include/ops_seq.hpp"
#include "shelenkova_m_shell_sort_simple_merge/stl/include/ops_stl.hpp"
#include "shelenkova_m_shell_sort_simple_merge/tbb/include/ops_tbb.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace shelenkova_m_shell_sort_simple_merge {

class ShelenkovaMRunFuncTestsShellSort : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 protected:
  void SetUp() override {
    TestType param = std::get<static_cast<int>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    int sz = param;
    input_data_ = std::vector<int>(sz, 0);
    int seed = 1;
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    for (auto &i : input_data_) {
      i = dist(rng);
    }
    expected_data_ = input_data_;
    std::ranges::sort(expected_data_);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    if (output_data.size() != expected_data_.size()) {
      return false;
    }

    for (std::size_t i = 0; i < output_data.size(); i++) {
      if (output_data[i] != expected_data_[i]) {
        return false;
      }
    }

    return true;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 public:
  static std::string PrintTestParam(const TestType &param) {
    return "Size_" + std::to_string(param);
  }

 private:
  InType input_data_;
  OutType expected_data_;
};

namespace {

TEST_P(ShelenkovaMRunFuncTestsShellSort, shellMergeTest) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 4> kTestParam = {1, 10, 100, 1000};

const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<ShelenkovaMShellSortSimpleMergeSEQ, InType>(kTestParam, PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge),
    ppc::util::AddFuncTask<ShelenkovaMShellSortSimpleMergeOMP, InType>(kTestParam, PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge),
    ppc::util::AddFuncTask<ShelenkovaMShellSortSimpleMergeTBB, InType>(kTestParam, PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge),
    ppc::util::AddFuncTask<ShelenkovaMShellSortSimpleMergeSTL, InType>(kTestParam, PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kTestName = ShelenkovaMRunFuncTestsShellSort::PrintFuncTestName<ShelenkovaMRunFuncTestsShellSort>;

INSTANTIATE_TEST_SUITE_P(shellSortSimpleMergeFuncTests, ShelenkovaMRunFuncTestsShellSort, kGtestValues, kTestName);

}  // namespace

}  // namespace shelenkova_m_shell_sort_simple_merge

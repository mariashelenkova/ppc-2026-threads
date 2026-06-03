#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
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

namespace shelenkova_m_shell_sort_simple_merge {

struct SortingTestCase {
  std::vector<int> data;
  std::string description;
};

const std::array<SortingTestCase, 14> kTestScenarios = {
    SortingTestCase{.data = std::vector<int>{}, .description = "empty"},
    SortingTestCase{.data = std::vector<int>{1}, .description = "single_element"},
    SortingTestCase{.data = std::vector<int>{2, 1}, .description = "two_unsorted"},
    SortingTestCase{.data = std::vector<int>{1, 2, 3, 4, 5}, .description = "ascending_order"},
    SortingTestCase{.data = std::vector<int>{5, 4, 3, 2, 1}, .description = "descending_order"},
    SortingTestCase{.data = std::vector<int>{5, 3, 8, 2, 1, 7, 4, 6}, .description = "random_sequence"},
    SortingTestCase{.data = std::vector<int>{7, 7, 7, 7, 7}, .description = "identical_values"},
    SortingTestCase{.data = std::vector<int>{-5, -1, -10, -3, -2, -8, -4}, .description = "negative_numbers"},
    SortingTestCase{.data = std::vector<int>{-3, 0, 5, -1, 2, -2, 4, -4}, .description = "mixed_values"},
    SortingTestCase{.data = std::vector<int>{10, -10, 10, -10, 0, 5, -5}, .description = "alternating_signs"},
    SortingTestCase{.data = std::vector<int>{1000, 500, 250, 125, 62, 31, 15, 7, 3, 1}, .description = "geometric_progression"},
    SortingTestCase{.data = std::vector<int>{42, -42, 17, 0, -1, 999, -999, 5, -5}, .description = "extreme_range"},
    SortingTestCase{.data = std::vector<int>{1, 3, 2, 4, 6, 5, 8, 7, 9, 10}, .description = "almost_sorted"},
    SortingTestCase{.data = std::vector<int>{100, 90, 80, 70, 60, 50, 40, 30, 20, 10}, .description = "large_step_descending"},
};

class SortingFunctionalTest : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string GenerateTestLabel(const TestType& parameters) {
    return std::to_string(std::get<0>(parameters)) + "_" + std::get<1>(parameters);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    const size_t scenario_index = std::get<0>(params);
    original_data_ = kTestScenarios.at(scenario_index).data;

    expected_output_ = original_data_;
    std::sort(expected_output_.begin(), expected_output_.end());
  }

  bool ValidateOutput(OutType& result) final {
    return result == expected_output_;
  }

  InType FetchInputData() final {
    return original_data_;
  }

 private:
  InType original_data_;
  OutType expected_output_;
};

TEST_P(SortingFunctionalTest, ShellSortValidation) {
  ExecuteTest(GetParam());
}

namespace {

const std::array<TestType, 14> kTestParameters = []() {
  std::array<TestType, 14> params;
  for (size_t i = 0; i < params.size(); ++i) {
    params.at(i) = std::make_tuple(i, kTestScenarios.at(i).description);
  }
  return params;
}();

const auto kAllTestTasks = std::tuple_cat(
    ppc::util::AddFuncTask<ShelenkovaMShellSortSimpleMergeSEQ, InType>(kTestParameters,
                                                                        PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge),
    ppc::util::AddFuncTask<ShelenkovaMShellSortSimpleMergeOMP, InType>(kTestParameters,
                                                                        PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge),
    ppc::util::AddFuncTask<ShelenkovaMShellSortSimpleMergeTBB, InType>(kTestParameters,
                                                                        PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge),
    ppc::util::AddFuncTask<ShelenkovaMShellSortSimpleMergeSTL, InType>(kTestParameters,
                                                                        PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge));

const auto kGtestValues = ppc::util::ExpandToValues(kAllTestTasks);

const auto kTestSuiteName = SortingFunctionalTest::PrintFuncTestName<SortingFunctionalTest>;

INSTANTIATE_TEST_SUITE_P(ShellSortFunctionalTests, SortingFunctionalTest, kGtestValues, kTestSuiteName);

}  // namespace

}  // namespace shelenkova_m_shell_sort_simple_merge

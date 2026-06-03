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

#include "shelenkova_m_shell_sort_simple_merge/all/include/ops_all.hpp"
#include "shelenkova_m_shell_sort_simple_merge/common/include/common.hpp"
#include "shelenkova_m_shell_sort_simple_merge/omp/include/ops_omp.hpp"
#include "shelenkova_m_shell_sort_simple_merge/seq/include/ops_seq.hpp"
#include "shelenkova_m_shell_sort_simple_merge/stl/include/ops_stl.hpp"
#include "shelenkova_m_shell_sort_simple_merge/tbb/include/ops_tbb.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace shelenkova_m_shell_sort_simple_merge {

class ShelenkovaMShellSortSimpleMergeFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 protected:
  void SetUp() override {
    TestType param = std::get<static_cast<int>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    int vector_size = param;
    input_data_ = std::vector<int>(vector_size, 0);
    int random_seed = 42;
    std::mt19937 rng(random_seed);
    std::uniform_int_distribution<int> dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    for (auto &element : input_data_) {
      element = dist(rng);
    }
    expected_data_ = input_data_;
    std::ranges::sort(expected_data_);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    if (output_data.size() != expected_data_.size()) {
      return false;
    }

    for (std::size_t position = 0; position < output_data.size(); position++) {
      if (output_data[position] != expected_data_[position]) {
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
    return "VectorSize_" + std::to_string(param);
  }

 private:
  InType input_data_;
  OutType expected_data_;
};

namespace {

TEST_P(ShelenkovaMShellSortSimpleMergeFuncTests, shellSortMergeTest) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 4> kTestParameters = {1, 10, 100, 1000};

const auto kAllTestTasks = std::tuple_cat(
    ppc::util::AddFuncTask<ShelenkovaMShellSortSimpleMergeSEQ, InType>(kTestParameters, PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge),
    ppc::util::AddFuncTask<ShelenkovaMShellSortSimpleMergeOMP, InType>(kTestParameters, PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge),
    ppc::util::AddFuncTask<ShelenkovaMShellSortSimpleMergeTBB, InType>(kTestParameters, PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge),
    ppc::util::AddFuncTask<ShelenkovaMShellSortSimpleMergeSTL, InType>(kTestParameters, PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge),
    ppc::util::AddFuncTask<ShelenkovaMShellSortSimpleMergeALL, InType>(kTestParameters, PPC_SETTINGS_shelenkova_m_shell_sort_simple_merge));

const auto kGtestValuesList = ppc::util::ExpandToValues(kAllTestTasks);

const auto kTestCaseName = ShelenkovaMShellSortSimpleMergeFuncTests::PrintFuncTestName<ShelenkovaMShellSortSimpleMergeFuncTests>;

INSTANTIATE_TEST_SUITE_P(shellSortSimpleMergeFuncTests, ShelenkovaMShellSortSimpleMergeFuncTests, kGtestValuesList, kTestCaseName);

}  // namespace

}  // namespace shelenkova_m_shell_sort_simple_merge

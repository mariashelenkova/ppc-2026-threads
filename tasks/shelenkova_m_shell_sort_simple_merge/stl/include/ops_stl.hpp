#pragma once

#include <cstdio>
#include <vector>

#include "shelenkova_m_shell_sort_simple_merge/common/include/common.hpp"
#include "task/include/task.hpp"

namespace shelenkova_m_shell_sort_simple_merge {

class ShelenkovaMShellSortSimpleMergeSTL : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSTL;
  }
  explicit ShelenkovaMShellSortSimpleMergeSTL(const InType &in);

  static void GapBasedSort(std::vector<int>::iterator start_pos, std::vector<int>::iterator end_pos);
  static void SimpleMergeSort(std::vector<int> &arr);
  static std::vector<std::size_t> CalculateChunks(std::size_t total_len, std::size_t chunk_cnt);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace shelenkova_m_shell_sort_simple_merge

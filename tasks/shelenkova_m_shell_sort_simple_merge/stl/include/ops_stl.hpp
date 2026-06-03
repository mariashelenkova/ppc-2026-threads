#pragma once

#include <vector>

#include "shelenkova_m_shell_sort_simple_merge/common/include/common.hpp"
#include "task/include/task.hpp"

namespace shelenkova_m_shell_sort_simple_merge {

class ShelenkovaMShellSortSimpleMergeSTL : public ppc::task::Task {
 public:
  explicit ShelenkovaMShellSortSimpleMergeSTL(ppc::task::TaskDataPtr task_data);
  ~ShelenkovaMShellSortSimpleMergeSTL() override = default;

  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<int> input_;
  std::vector<int> output_;
};

}  // namespace shelenkova_m_shell_sort_simple_merge

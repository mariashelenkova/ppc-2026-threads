#pragma once

#include <vector>

#include "shelenkova_m_shell_sort_simple_merge/common/include/common.hpp"
#include "task/include/task.hpp"

namespace shelenkova_m_shell_sort_simple_merge {

class ShelenkovaMShellSortSimpleMergeSTL : public ppc::task::TaskSTL {
 public:
  explicit ShelenkovaMShellSortSimpleMergeSTL(std::shared_ptr<ppc::task::TaskData> task_data);
  ~ShelenkovaMShellSortSimpleMergeSTL() override = default;

  bool validation() override;
  bool pre_processing() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<int> input_;
  std::vector<int> output_;
};

}  // namespace shelenkova_m_shell_sort_simple_merge

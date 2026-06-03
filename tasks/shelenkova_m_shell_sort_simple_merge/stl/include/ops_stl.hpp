#pragma once

#include <cstddef>
#include <vector>

#include "shelenkova_m_shell_sort_simple_merge/common/include/common.hpp"
#include "task/include/task.hpp"

namespace shelenkova_m_shell_sort_simple_merge {

class ShelenkovaMShellSortSimpleMergeSTL : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() { return ppc::task::TypeOfTask::kSTL; }
  explicit ShelenkovaMShellSortSimpleMergeSTL(const InType& in);

  static void SortingProcedure(std::vector<int>::iterator start, std::vector<int>::iterator finish);
  static void FullSorting(std::vector<int>& container);
  static std::vector<std::size_t> PartitionIndices(std::size_t total, std::size_t segments);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace shelenkova_m_shell_sort_simple_merge

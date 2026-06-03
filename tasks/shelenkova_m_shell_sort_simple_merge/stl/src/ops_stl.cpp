#include "shelenkova_m_shell_sort_simple_merge/stl/include/ops_stl.hpp"

#include <algorithm>
#include <cstddef>
#include <thread>
#include <vector>

#include "shelenkova_m_shell_sort_simple_merge/common/include/common.hpp"
#include "util/include/util.hpp"

namespace shelenkova_m_shell_sort_simple_merge {

void ShelenkovaMShellSortSimpleMergeSTL::SortingProcedure(std::vector<int>::iterator start,
                                                           std::vector<int>::iterator finish) {
  for (std::ptrdiff_t step = (finish - start) / 2; step > 0; step /= 2) {
    for (auto current = start + step; current != finish; ++current) {
      for (auto position = current; position - start >= step && (*position < *(position - step));
           position -= step) {
        std::swap(*position, *(position - step));
      }
    }
  }
}

std::vector<std::size_t> ShelenkovaMShellSortSimpleMergeSTL::PartitionIndices(std::size_t total,
                                                                                std::size_t segments) {
  segments = std::max<std::size_t>(1, std::min(segments, total));

  std::vector<std::size_t> boundaries;
  boundaries.reserve(segments + 1);
  boundaries.push_back(0);

  const std::size_t base_size = total / segments;
  const std::size_t remainder = total % segments;

  for (std::size_t idx = 0; idx < segments; ++idx) {
    boundaries.push_back(boundaries.back() + base_size);
    if (idx < remainder) {
      boundaries[idx + 1]++;
    }
  }

  return boundaries;
}

ShelenkovaMShellSortSimpleMergeSTL::ShelenkovaMShellSortSimpleMergeSTL(const InType& in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>();
}

bool ShelenkovaMShellSortSimpleMergeSTL::ValidationImpl() {
  const InType& data = GetInput();
  return !data.empty();
}

bool ShelenkovaMShellSortSimpleMergeSTL::PreProcessingImpl() {
  GetOutput() = GetInput();
  return true;
}

bool ShelenkovaMShellSortSimpleMergeSTL::RunImpl() {
  std::vector<int>& elements = GetOutput();

  if (elements.size() <= 1) {
    return true;
  }

  const auto available_threads = static_cast<std::size_t>(ppc::util::GetNumThreads());
  const std::size_t segment_count = std::min<std::size_t>(available_threads, elements.size());
  const auto boundaries = PartitionIndices(elements.size(), segment_count);

  std::vector<std::thread> workers(segment_count);

  for (std::size_t idx = 0; idx < segment_count; ++idx) {
    const std::size_t left = boundaries[idx];
    const std::size_t right = boundaries[idx + 1];
    workers[idx] = std::thread([&elements, left, right]() {
      SortingProcedure(elements.begin() + static_cast<std::ptrdiff_t>(left),
                       elements.begin() + static_cast<std::ptrdiff_t>(right));
    });
  }

  for (auto& worker : workers) {
    worker.join();
  }

  for (std::size_t idx = 1; idx < segment_count; ++idx) {
    std::inplace_merge(elements.begin(), elements.begin() + static_cast<std::ptrdiff_t>(boundaries[idx]),
                       elements.begin() + static_cast<std::ptrdiff_t>(boundaries[idx + 1]));
  }

  return true;
}

bool ShelenkovaMShellSortSimpleMergeSTL::PostProcessingImpl() { return true; }

}  // namespace shelenkova_m_shell_sort_simple_merge

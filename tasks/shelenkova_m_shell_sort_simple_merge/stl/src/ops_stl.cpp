#include "shelenkova_m_shell_sort_simple_merge/stl/include/ops_stl.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <thread>
#include <vector>

#include "shelenkova_m_shell_sort_simple_merge/common/include/common.hpp"
#include "util/include/util.hpp"

namespace shelenkova_m_shell_sort_simple_merge {

void ShelenkovaMShellSortSimpleMergeSTL::GapBasedSort(std::vector<int>::iterator start_pos,
                                                       std::vector<int>::iterator end_pos) {
  for (std::ptrdiff_t gap = (end_pos - start_pos) / 2; gap > 0; gap /= 2) {
    for (auto cur = start_pos + gap; cur != end_pos; ++cur) {
      for (auto walk = cur; walk - start_pos >= gap && (*walk < *(walk - gap)); walk -= gap) {
        std::swap(*walk, *(walk - gap));
      }
    }
  }
}

std::vector<std::size_t> ShelenkovaMShellSortSimpleMergeSTL::CalculateChunks(std::size_t total_len,
                                                                               std::size_t chunk_cnt) {
  chunk_cnt = std::max<std::size_t>(1, std::min(chunk_cnt, total_len));

  std::vector<std::size_t> boundaries;
  boundaries.reserve(chunk_cnt + 1);
  boundaries.push_back(0);

  const std::size_t chunk_size_base = total_len / chunk_cnt;
  const std::size_t remainder = total_len % chunk_cnt;

  for (std::size_t idx = 0; idx < chunk_cnt; ++idx) {
    boundaries.push_back(boundaries.back() + chunk_size_base);
    if (idx < remainder) {
      boundaries[idx + 1]++;
    }
  }

  return boundaries;
}

ShelenkovaMShellSortSimpleMergeSTL::ShelenkovaMShellSortSimpleMergeSTL(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>();
}

bool ShelenkovaMShellSortSimpleMergeSTL::ValidationImpl() {
  const InType &source = GetInput();
  return !source.empty();
}

bool ShelenkovaMShellSortSimpleMergeSTL::PreProcessingImpl() {
  GetOutput() = GetInput();
  return true;
}

bool ShelenkovaMShellSortSimpleMergeSTL::RunImpl() {
  std::vector<int> &data = GetOutput();

  if (data.size() <= 1) {
    return true;
  }

  const auto available_workers = static_cast<std::size_t>(ppc::util::GetNumThreads());
  const std::size_t segment_count = std::min<std::size_t>(available_workers, data.size());
  const auto segment_limits = CalculateChunks(data.size(), segment_count);

  std::vector<std::thread> worker_pool(segment_count);

  for (std::size_t seg = 0; seg < segment_count; ++seg) {
    const std::size_t left_border = segment_limits[seg];
    const std::size_t right_border = segment_limits[seg + 1];
    worker_pool[seg] = std::thread([&data, left_border, right_border]() {
      GapBasedSort(data.begin() + static_cast<std::ptrdiff_t>(left_border),
                   data.begin() + static_cast<std::ptrdiff_t>(right_border));
    });
  }

  for (auto &thread_handle : worker_pool) {
    thread_handle.join();
  }

  for (std::size_t seg = 1; seg < segment_count; ++seg) {
    std::inplace_merge(data.begin(), data.begin() + static_cast<std::ptrdiff_t>(segment_limits[seg]),
                       data.begin() + static_cast<std::ptrdiff_t>(segment_limits[seg + 1]));
  }

  return true;
}

bool ShelenkovaMShellSortSimpleMergeSTL::PostProcessingImpl() {
  return true;
}

}  // namespace shelenkova_m_shell_sort_simple_merge

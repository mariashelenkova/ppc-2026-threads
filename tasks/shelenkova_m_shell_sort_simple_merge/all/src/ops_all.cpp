#include "shelenkova_m_shell_sort_simple_merge/all/include/ops_all.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <thread>
#include <vector>

#include "shelenkova_m_shell_sort_simple_merge/common/include/common.hpp"
#include "util/include/util.hpp"

namespace shelenkova_m_shell_sort_simple_merge {

void ShelenkovaMShellSortSimpleMergeALL::GapBasedSort(std::vector<int>::iterator start_pos,
                                                       std::vector<int>::iterator end_pos) {
  for (std::ptrdiff_t step = (end_pos - start_pos) / 2; step > 0; step /= 2) {
    for (auto position = start_pos + step; position != end_pos; ++position) {
      for (auto mover = position; mover - start_pos >= step && (*mover < *(mover - step)); mover -= step) {
        std::swap(*mover, *(mover - step));
      }
    }
  }
}

std::vector<std::size_t> ShelenkovaMShellSortSimpleMergeALL::CalculateChunks(std::size_t total_len,
                                                                               std::size_t chunk_cnt) {
  chunk_cnt = std::max<std::size_t>(1, std::min(chunk_cnt, total_len));

  std::vector<std::size_t> boundaries;
  boundaries.reserve(chunk_cnt + 1);
  boundaries.push_back(0);

  const std::size_t base_size = total_len / chunk_cnt;
  const std::size_t extra = total_len % chunk_cnt;

  for (std::size_t part_id = 0; part_id < chunk_cnt; ++part_id) {
    boundaries.push_back(boundaries.back() + base_size);
    if (part_id < extra) {
      boundaries[part_id + 1]++;
    }
  }

  return boundaries;
}

ShelenkovaMShellSortSimpleMergeALL::ShelenkovaMShellSortSimpleMergeALL(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>();
}

bool ShelenkovaMShellSortSimpleMergeALL::ValidationImpl() {
  const InType &input_vec = GetInput();
  return !input_vec.empty();
}

bool ShelenkovaMShellSortSimpleMergeALL::PreProcessingImpl() {
  GetOutput() = GetInput();
  return true;
}

bool ShelenkovaMShellSortSimpleMergeALL::RunImpl() {
  std::vector<int> &data_buffer = GetOutput();

  if (data_buffer.size() <= 1) {
    return true;
  }

  const auto thread_limit = static_cast<std::size_t>(ppc::util::GetNumThreads());
  const std::size_t partition_count = std::min<std::size_t>(thread_limit, data_buffer.size());
  const auto partition_points = CalculateChunks(data_buffer.size(), partition_count);

  std::vector<std::thread> workers(partition_count);

  for (std::size_t part_idx = 0; part_idx < partition_count; ++part_idx) {
    const std::size_t left_edge = partition_points[part_idx];
    const std::size_t right_edge = partition_points[part_idx + 1];
    workers[part_idx] = std::thread([&data_buffer, left_edge, right_edge]() {
      GapBasedSort(data_buffer.begin() + static_cast<std::ptrdiff_t>(left_edge),
                   data_buffer.begin() + static_cast<std::ptrdiff_t>(right_edge));
    });
  }

  for (auto &worker : workers) {
    worker.join();
  }

  for (std::size_t part_idx = 1; part_idx < partition_count; ++part_idx) {
    std::inplace_merge(data_buffer.begin(), data_buffer.begin() + static_cast<std::ptrdiff_t>(partition_points[part_idx]),
                       data_buffer.begin() + static_cast<std::ptrdiff_t>(partition_points[part_idx + 1]));
  }

  return true;
}

bool ShelenkovaMShellSortSimpleMergeALL::PostProcessingImpl() {
  return true;
}

}  // namespace shelenkova_m_shell_sort_simple_merge

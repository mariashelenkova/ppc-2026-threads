#include "shelenkova_m_shell_sort_simple_merge/stl/include/ops_stl.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <thread>
#include <vector>

#include "shelenkova_m_shell_sort_simple_merge/common/include/common.hpp"
#include "util/include/util.hpp"

namespace shelenkova_m_shell_sort_simple_merge {

void ShelenkovaMShellSortSimpleMergeSTL::BaseShellSort(std::vector<int>::iterator first,
                                                        std::vector<int>::iterator last) {
  for (std::ptrdiff_t dist = (last - first) / 2; dist > 0; dist /= 2) {
    for (auto i = first + dist; i != last; ++i) {
      for (auto j = i; j - first >= dist && (*j < *(j - dist)); j -= dist) {
        std::swap(*j, *(j - dist));
      }
    }
  }
}

std::vector<std::size_t> ShelenkovaMShellSortSimpleMergeSTL::GetBounds(std::size_t n, std::size_t parts) {
  parts = std::max<std::size_t>(1, std::min(parts, n));

  std::vector<std::size_t> bounds;
  bounds.reserve(parts + 1);
  bounds.push_back(0);

  const std::size_t base = n / parts;
  const std::size_t rem = n % parts;

  for (std::size_t i = 0; i < parts; ++i) {
    bounds.push_back(bounds.back() + base);
    if (i < rem) {
      bounds[i + 1]++;
    }
  }

  return bounds;
}

ShelenkovaMShellSortSimpleMergeSTL::ShelenkovaMShellSortSimpleMergeSTL(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>();
}

bool ShelenkovaMShellSortSimpleMergeSTL::ValidationImpl() {
  const InType &vec = GetInput();
  return !vec.empty();
}

bool ShelenkovaMShellSortSimpleMergeSTL::PreProcessingImpl() {
  GetOutput() = GetInput();
  return true;
}

bool ShelenkovaMShellSortSimpleMergeSTL::RunImpl() {
  std::vector<int> &vec = GetOutput();

  if (vec.size() <= 1) {
    return true;
  }

  const auto threads = static_cast<std::size_t>(ppc::util::GetNumThreads());
  const std::size_t parts_count = std::min<std::size_t>(threads, vec.size());
  const auto bounds = GetBounds(vec.size(), parts_count);

  std::vector<std::thread> thread_pool(parts_count);

  for (std::size_t i = 0; i < parts_count; ++i) {
    const std::size_t l = bounds[i];
    const std::size_t r = bounds[i + 1];
    thread_pool[i] = std::thread([&vec, l, r]() {
      BaseShellSort(vec.begin() + static_cast<std::ptrdiff_t>(l), vec.begin() + static_cast<std::ptrdiff_t>(r));
    });
  }

  for (auto &t : thread_pool) {
    t.join();
  }

  for (std::size_t i = 1; i < parts_count; ++i) {
    std::inplace_merge(vec.begin(), vec.begin() + static_cast<std::ptrdiff_t>(bounds[i]),
                       vec.begin() + static_cast<std::ptrdiff_t>(bounds[i + 1]));
  }

  return true;
}

bool ShelenkovaMShellSortSimpleMergeSTL::PostProcessingImpl() {
  return true;
}

}  // namespace shelenkova_m_shell_sort_simple_merge

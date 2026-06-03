#include "shelenkova_m_shell_sort_simple_merge/stl/include/ops_stl.hpp"

#include <algorithm>
#include <cstddef>
#include <thread>
#include <vector>

#include "shelenkova_m_shell_sort_simple_merge/common/include/common.hpp"
#include "util/include/util.hpp"

namespace shelenkova_m_shell_sort_simple_merge {

namespace {

void ShellSortSeq(std::vector<int>::iterator first, std::vector<int>::iterator last) {
  const size_t n = static_cast<size_t>(std::distance(first, last));
  if (n <= 1) {
    return;
  }

  for (size_t gap = n / 2; gap > 0; gap /= 2) {
    for (size_t i = gap; i < n; ++i) {
      int temp = *(first + static_cast<std::ptrdiff_t>(i));
      size_t j = i;
      while (j >= gap && *(first + static_cast<std::ptrdiff_t>(j - gap)) > temp) {
        *(first + static_cast<std::ptrdiff_t>(j)) = *(first + static_cast<std::ptrdiff_t>(j - gap));
        j -= gap;
      }
      *(first + static_cast<std::ptrdiff_t>(j)) = temp;
    }
  }
}

std::vector<size_t> CalcBounds(size_t n, size_t parts) {
  parts = std::max<size_t>(1, std::min(parts, n));

  std::vector<size_t> bounds;
  bounds.reserve(parts + 1);
  bounds.push_back(0);

  const size_t base = n / parts;
  const size_t rem = n % parts;

  for (size_t i = 0; i < parts; ++i) {
    bounds.push_back(bounds.back() + base);
    if (i < rem) {
      bounds[i + 1]++;
    }
  }

  return bounds;
}

}  // namespace

ShelenkovaMShellSortSimpleMergeSTL::ShelenkovaMShellSortSimpleMergeSTL(const InType& in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>();
}

bool ShelenkovaMShellSortSimpleMergeSTL::ValidationImpl() {
  const InType& vec = GetInput();
  return !vec.empty();
}

bool ShelenkovaMShellSortSimpleMergeSTL::PreProcessingImpl() {
  GetOutput() = GetInput();
  return true;
}

bool ShelenkovaMShellSortSimpleMergeSTL::RunImpl() {
  std::vector<int>& vec = GetOutput();

  if (vec.size() <= 1) {
    return true;
  }

  const auto threads = static_cast<size_t>(ppc::util::GetNumThreads());
  const size_t parts = std::min<size_t>(threads, vec.size());
  const auto bounds = CalcBounds(vec.size(), parts);

  std::vector<std::thread> thread_pool(parts);

  for (size_t i = 0; i < parts; ++i) {
    const size_t l = bounds[i];
    const size_t r = bounds[i + 1];
    thread_pool[i] = std::thread([&vec, l, r]() {
      ShellSortSeq(vec.begin() + static_cast<std::ptrdiff_t>(l),
                   vec.begin() + static_cast<std::ptrdiff_t>(r));
    });
  }

  for (auto& t : thread_pool) {
    t.join();
  }

  for (size_t i = 1; i < parts; ++i) {
    std::inplace_merge(vec.begin(), vec.begin() + static_cast<std::ptrdiff_t>(bounds[i]),
                       vec.begin() + static_cast<std::ptrdiff_t>(bounds[i + 1]));
  }

  return true;
}

bool ShelenkovaMShellSortSimpleMergeSTL::PostProcessingImpl() { return true; }

}  // namespace shelenkova_m_shell_sort_simple_merge

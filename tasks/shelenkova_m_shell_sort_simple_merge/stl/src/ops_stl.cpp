#include "shelenkova_m_shell_sort_simple_merge/stl/include/ops_stl.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <thread>
#include <vector>

#include "util/include/util.hpp"

namespace shelenkova_m_shell_sort_simple_merge {

namespace {

void ShellSort(std::vector<int>::iterator begin, std::vector<int>::iterator end) {
  const size_t n = static_cast<size_t>(std::distance(begin, end));
  if (n <= 1) {
    return;
  }

  for (size_t gap = n / 2; gap > 0; gap /= 2) {
    for (size_t i = gap; i < n; ++i) {
      int temp = *(begin + static_cast<std::ptrdiff_t>(i));
      size_t j = i;
      while (j >= gap && *(begin + static_cast<std::ptrdiff_t>(j - gap)) > temp) {
        *(begin + static_cast<std::ptrdiff_t>(j)) = *(begin + static_cast<std::ptrdiff_t>(j - gap));
        j -= gap;
      }
      *(begin + static_cast<std::ptrdiff_t>(j)) = temp;
    }
  }
}

}  // namespace

ShelenkovaMShellSortSimpleMergeSTL::ShelenkovaMShellSortSimpleMergeSTL(ppc::task::TaskDataPtr task_data)
    : Task(std::move(task_data)) {}

bool ShelenkovaMShellSortSimpleMergeSTL::pre_processing() {
  auto* input_ptr = reinterpret_cast<std::vector<int>*>(task_data->inputs[0]);
  input_ = *input_ptr;
  output_ = input_;
  return true;
}

bool ShelenkovaMShellSortSimpleMergeSTL::validation() {
  auto* input_ptr = reinterpret_cast<std::vector<int>*>(task_data->inputs[0]);
  return input_ptr != nullptr && !input_ptr->empty();
}

bool ShelenkovaMShellSortSimpleMergeSTL::run() {
  std::vector<int>& data = output_;
  const size_t n = data.size();
  if (n <= 1) {
    return true;
  }

  const int num_threads = std::max(1, ppc::util::GetNumThreads());
  const size_t thread_count = std::min<size_t>(static_cast<size_t>(num_threads), n);
  const size_t chunk = n / thread_count;
  const size_t rem = n % thread_count;

  std::vector<size_t> offsets(thread_count + 1);
  size_t curr = 0;
  for (size_t i = 0; i < thread_count; ++i) {
    offsets[i] = curr;
    curr += chunk + (i < rem ? 1 : 0);
  }
  offsets[thread_count] = n;

  // Parallel sort of chunks
  {
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for (size_t i = 0; i < thread_count; ++i) {
      threads.emplace_back([&data, &offsets, i]() {
        ShellSort(data.begin() + static_cast<std::ptrdiff_t>(offsets[i]),
                  data.begin() + static_cast<std::ptrdiff_t>(offsets[i + 1]));
      });
    }
    for (auto& t : threads) {
      t.join();
    }
  }

  // Sequential merge of chunks
  for (size_t size = 1; size < thread_count; size *= 2) {
    for (size_t left = 0; left < thread_count; left += 2 * size) {
      size_t mid = left + size;
      size_t right = std::min(left + 2 * size, thread_count);
      if (mid < right) {
        std::inplace_merge(data.begin() + static_cast<std::ptrdiff_t>(offsets[left]),
                          data.begin() + static_cast<std::ptrdiff_t>(offsets[mid]),
                          data.begin() + static_cast<std::ptrdiff_t>(offsets[right]));
      }
    }
  }

  return std::is_sorted(data.begin(), data.end());
}

bool ShelenkovaMShellSortSimpleMergeSTL::post_processing() {
  auto* output_ptr = reinterpret_cast<std::vector<int>*>(task_data->outputs[0]);
  *output_ptr = output_;
  return true;
}

}  // namespace shelenkova_m_shell_sort_simple_merge

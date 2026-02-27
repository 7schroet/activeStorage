/*
 * Copyright (c) 2025 - 2026 Niclas Schroeter
 * All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AS_RPC_KERNEL_MATH_HPP
#define AS_RPC_KERNEL_MATH_HPP

#include <H5Cpp.h>
#include <algorithm>
#include <cassert>
#include <functional>
#include <numeric>
#include <vector>

namespace as_rpc::kernel_impl
{

inline const auto max_wrap = [](auto& container)
{ return *std::ranges::max_element(container); };
inline const auto min_wrap = [](auto& container)
{ return *std::ranges::min_element(container); };

template <typename T>
std::pair<std::vector<double>, std::vector<hsize_t>>
mean_reduction(const std::vector<T>& data, const std::vector<hsize_t>& dims,
               const std::vector<char>& reduce_along_dim);

std::vector<double> running_mean(const std::vector<double>& mean,
                                 const std::vector<double>& running_mean,
                                 int num_entries);

template <typename T, auto Func>
std::pair<std::vector<T>, std::vector<hsize_t>>
compare_reduction(const std::vector<T>& data, const std::vector<hsize_t>& dims,
                  const std::vector<char>& reduce_along_dim);

// Template impl
template <typename T>
std::pair<std::vector<double>, std::vector<hsize_t>>
mean_reduction(const std::vector<T>& data, const std::vector<hsize_t>& dims,
               const std::vector<char>& reduce_along_dim)
{
  assert((data.size() != 0) && "Passed vector of size 0!");
  assert((dims.size() == reduce_along_dim.size()) &&
         "Dimensionalities do not match!");

  const auto num_reduced_dims =
      std::reduce(reduce_along_dim.begin(), reduce_along_dim.end(), 0u);
  const bool reduce_to_single_value = num_reduced_dims == dims.size();
  if (reduce_to_single_value)
  {
    const double sum = std::reduce(data.begin(), data.end(), 0.0);
    const double avg = sum / data.size();
    return {{avg}, {1}};
  }

  std::vector<hsize_t> new_dims{dims};
  std::vector<double> result(data.begin(), data.end());

  /**
   * Each iteration of this loop reduces one dimension down to 1, if requested
   * for the specific dimension. We start from the back to at least try to
   * take memory into account. Reduced dimensions are kept around as degenerate
   * dimensions of size 1 until the end. Though for the purposes of the
   * algorithm, these dimensions are nonexistent.
   */
  for (int current_dim = new_dims.size() - 1; current_dim >= 0; current_dim--)
  {
    if (!(reduce_along_dim[current_dim]))
      continue;

    const auto elements_post_reduction = result.size() / new_dims[current_dim];
    std::vector<double> tmp(elements_post_reduction);

    /**
     * Each reduction operates in 'blocks'. A block in this case is a contiguous
     * sequence in memory. The number of elements in a block is given by the
     * product of all dimensions, starting at the current one.
     */
    const auto elements_in_block =
        std::reduce(new_dims.begin() + current_dim, new_dims.end(), 1ul,
                    std::multiplies<>());

    const auto num_blocks = result.size() / elements_in_block;

    /**
     * The following two variables handle the stride for the reduction within
     * a block. There are two cases. Either the reduction happens along the
     * dimension that is consecutive in memory, in which case the
     * @stride_for_reduction_steps@ == 1. This happens if the reduction is
     * applied alongside the last dimension (excluding degenerates from
     * previous reductions).
     *
     * In the other case the reduction does not happen consecutive in memory,
     * which means that subsequent elements have a stride > 1. In that case the
     * other value, @stride_reduction_starts@, must be 1 though, as the
     * starting points for each reduction must be consecutive in memory.
     */
    const auto stride_for_reduction_steps =
        std::reduce(new_dims.begin() + current_dim + 1, new_dims.end(), 1ul,
                    std::multiplies<>());

    const auto stride_reduction_starts =
        stride_for_reduction_steps == 1 ? new_dims[current_dim] : 1ul;

    for (auto i = 0ul; i < num_blocks; i++)
    {
      auto block_offset = i * elements_in_block;

      for (auto j = 0ul; j < stride_for_reduction_steps; j++)
      {
        double sum = 0.0;
        auto offset = j * stride_reduction_starts + block_offset;
        for (auto element = 0ul; element < new_dims[current_dim]; element++)
        {
          sum += result[offset];
          offset += stride_for_reduction_steps;
        }

        tmp[j + i * stride_for_reduction_steps] = sum / new_dims[current_dim];
      }
    }

    new_dims[current_dim] = 1;
    result = std::move(tmp);
  }

  std::erase(new_dims, 1);
  return {std::move(result), std::move(new_dims)};
}

template <typename T, auto Func>
std::pair<std::vector<T>, std::vector<hsize_t>>
compare_reduction(const std::vector<T>& data, const std::vector<hsize_t>& dims,
                  const std::vector<char>& reduce_along_dim)
{
  assert((data.size() != 0) && "Passed vector of size 0!");
  assert((dims.size() == reduce_along_dim.size()) &&
         "Dimensionalities do not match!");

  const auto num_reduced_dims =
      std::reduce(reduce_along_dim.begin(), reduce_along_dim.end(), 0u);
  const bool reduce_to_single_value = num_reduced_dims == dims.size();
  if (reduce_to_single_value)
  {
    const T result = Func(data);
    return {{result}, {1}};
  }
  return {{}, {}};
}

} // namespace as_rpc::kernel_impl
#endif

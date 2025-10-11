/*
 * Copyright (c) 2025 Niclas Schroeter
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
#include <cassert>
#include <functional>
#include <numeric>
#include <vector>

namespace as_rpc::kernel_impl
{

template <typename T>
std::pair<std::vector<double>, std::vector<hsize_t>>
mean_reduction(const std::vector<T>& data, const std::vector<hsize_t>& dims,
               const std::vector<char>& reduce_along_dim);

std::vector<double> running_mean(const std::vector<double>& mean,
                                 const std::vector<double>& running_mean,
                                 int num_entries);

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

  for (int current_dim = new_dims.size() - 1; current_dim >= 0; current_dim--)
  {
    if (!(reduce_along_dim[current_dim]))
      continue;

    const auto elements_post_reduction = result.size() / new_dims[current_dim];
    std::vector<double> tmp(elements_post_reduction);

    const auto stride_for_reduction =
        std::reduce(new_dims.begin() + current_dim + 1, new_dims.end(), 1ul,
                    std::multiplies<>());

    const auto stride_between_starts =
        stride_for_reduction == 1 ? new_dims[current_dim] : 1ul;

    for (auto i = 0ul; i < elements_post_reduction; i++)
    {
      double sum = 0.0;
      auto offset = i * stride_between_starts;
      for (auto count = 0ul; count < new_dims[current_dim]; count++)
      {
        sum += result[offset];
        offset += stride_for_reduction;
      }
      tmp[i] = sum / new_dims[current_dim];
    }

    new_dims[current_dim] = 1;
    result = std::move(tmp);
  }

  std::erase(new_dims, 1);
  return {std::move(result), std::move(new_dims)};
}

} // namespace as_rpc::kernel_impl
#endif

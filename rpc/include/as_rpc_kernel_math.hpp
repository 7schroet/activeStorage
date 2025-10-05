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

namespace as_rpc
{
namespace kernel_impl
{

template <typename T>
std::vector<double> mean_reduction(const std::vector<T>& data,
                                   const std::vector<hsize_t>& dims,
                                   const std::vector<char>& reduce_along_dim)
{
  assert((data.size() != 0) && "Passed vector of size 0!");
  assert((dims.size() == reduce_along_dim.size()) &&
         "Dimensionalities do not match!");

  std::vector<hsize_t> new_dims{dims};
  const auto num_reduced_dims =
      std::reduce(reduce_along_dim.begin(), reduce_along_dim.end(), 0u);
  const bool reduce_to_single_value = num_reduced_dims == dims.size();

  if (dims.size() == 1 || reduce_to_single_value)
  {
    const double sum = std::reduce(data.begin(), data.end(), 0.0);
    const auto avg = sum / data.size();
    return {avg};
  }

  std::vector<double> result(data.begin(), data.end());

  for (int current_dim = new_dims.size() - 1; current_dim >= 0; current_dim--)
  {
    if (!(reduce_along_dim[current_dim]))
      continue;

    std::vector<double> tmp{};
    const auto stride_between_elements =
        std::reduce(new_dims.begin() + current_dim + 1, new_dims.end(), 1ul,
                    std::multiplies<>());

    const auto reduced_num_elements = result.size() / new_dims[current_dim];
    const auto stride_between_starts =
        stride_between_elements == 1 ? new_dims[current_dim] : 1ul;

    for (auto i = 0ul; i < reduced_num_elements; i++)
    {
      double sum = 0.0;
      auto offset = i * stride_between_starts;
      for (auto count = 0ul; count < new_dims[current_dim]; count++)
      {
        sum += result[offset];
        offset += stride_between_elements;
      }
      tmp.push_back(sum / new_dims[current_dim]);
    }

    new_dims[current_dim] = 1;
    result = std::move(tmp);
  }

  return result;
}

} // namespace kernel_impl
} // namespace as_rpc
#endif

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

  std::vector<hsize_t> new_dims{};
  for (decltype(dims.size()) i = 0; i < dims.size(); i++)
  {
    if (!(reduce_along_dim[i]))
      new_dims.push_back(dims[i]);
  }
  const bool reduce_to_single_value = new_dims.size() == 0;

  if (dims.size() == 1 || reduce_to_single_value)
  {
    auto sum = std::reduce(data.begin(), data.end(), 0.0);
    auto avg = sum / data.size();
    return {avg};
  }

  std::vector<double> result(data.begin(), data.end());

  for (int current_dim = dims.size() - 1; current_dim >= 0; current_dim--)
  {
    if (!(reduce_along_dim[current_dim]))
      continue;

    std::vector<double> tmp{};
    auto stride = std::reduce(dims.begin() + current_dim + 1, dims.end(), 1,
                              std::multiplies<>());
    auto reduced_num_elements = result.size() / dims[current_dim];

    for (decltype(reduced_num_elements) i = 0; i < reduced_num_elements; i++)
    {
      double sum = 0.0;
      unsigned offset = i;
      for (unsigned count = 0; count < dims[current_dim]; count++)
      {
        sum += result[offset];
        offset += stride;
      }
      tmp.push_back(sum / dims[current_dim]);
    }

    result = std::move(tmp);
  }

  return result;
}

} // namespace kernel_impl
} // namespace as_rpc
#endif

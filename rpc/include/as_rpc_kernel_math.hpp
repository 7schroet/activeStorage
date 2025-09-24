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
#include <numeric>
#include <vector>

namespace as_rpc
{
namespace kernel_impl
{

template <typename T>
std::vector<double> mean_reduction(const std::vector<T>& data,
                                   const std::vector<hsize_t>& dims,
                                   const std::vector<char>& keep_dim)
{
  assert((data.size() != 0) && "Passed vector of size 0!");
  auto kept_dims = std::reduce(keep_dim.begin(), keep_dim.end(), 0);
  bool reduce_to_single_value = kept_dims == 0;

  if (dims.size() == 1 || reduce_to_single_value)
  {
    auto sum = std::reduce(data.begin(), data.end(), 0.0);
    auto avg = sum / data.size();
    return {avg};
  }

  return {};
}

} // namespace kernel_impl
} // namespace as_rpc
#endif

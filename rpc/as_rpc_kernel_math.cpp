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

#include "as_rpc_kernel_math.hpp"
#include <cassert>

namespace as_rpc
{
namespace kernel_impl
{
std::vector<double> running_mean(const std::vector<double>& mean,
                                 const std::vector<double>& running_mean,
                                 int num_entries)
{
  assert((mean.size() == running_mean.size()) &&
         "Passed vectors must have the same size!");
  assert((running_mean.size() != 0) && "Running mean must contain elements!");

  const double reciprocal = 1.0 / (num_entries + 1);
  std::vector<double> result(mean.size());
  for (auto i = 0ul; i < mean.size(); i++)
  {
    result[i] = (mean[i] + num_entries * running_mean[i]) * reciprocal;
  }
  return result;
}
} // namespace kernel_impl
} // namespace as_rpc

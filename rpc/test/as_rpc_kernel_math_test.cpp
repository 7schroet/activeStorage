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

#include "../include/as_rpc_kernel_math.hpp"
#include <gtest/gtest.h>
#include <vector>

namespace
{
TEST(Math, MeanOneElement)
{
  const std::vector<double> expected{5.0};
  const std::vector<hsize_t> dims{1};

  auto actual = as_rpc::kernel_impl::mean_reduction(expected, dims, {});
  EXPECT_EQ(expected, actual);
}

TEST(Math, MeanDoubles1D)
{
  const std::vector<double> data{12.0, 13.0, 14.0, 15.0};
  const std::vector<hsize_t> dims{data.size()};
  const std::vector<double> expected{13.5};

  auto actual = as_rpc::kernel_impl::mean_reduction(data, dims, {});
  EXPECT_EQ(expected.size(), actual.size());
  EXPECT_DOUBLE_EQ(expected[0], actual[0]);
}

TEST(Math, MeanInts1D)
{
  const std::vector<int> data{12, 13, 14, 15, 16, 17};
  const std::vector<hsize_t> dims{data.size()};
  const std::vector<double> expected{14.5};

  auto actual = as_rpc::kernel_impl::mean_reduction(data, dims, {});
  EXPECT_EQ(expected.size(), actual.size());
  EXPECT_DOUBLE_EQ(expected[0], actual[0]);
}

TEST(Math, MeanZeroElement)
{
  EXPECT_DEBUG_DEATH(as_rpc::kernel_impl::mean_reduction<int>({}, {1}, {}),
                     "Passed vector of size 0!");
}
} // namespace

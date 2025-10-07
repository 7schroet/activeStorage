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
  const std::vector<double> expected_data{5.0};
  const std::vector<hsize_t> expected_dims{1};

  const auto [actual_data, actual_dims] =
      as_rpc::kernel_impl::mean_reduction(expected_data, expected_dims, {1});
  EXPECT_EQ(expected_data.size(), actual_data.size());
  EXPECT_DOUBLE_EQ(expected_data[0], actual_data[0]);
  EXPECT_EQ(expected_dims, actual_dims);
}

TEST(Math, MeanDoubles1D)
{
  const std::vector<double> data{12.0, 13.0, 14.0, 15.0};
  const std::vector<hsize_t> dims{data.size()};
  const std::vector<double> expected_data{13.5};
  const std::vector<hsize_t> expected_dims{1};

  const auto [actual_data, actual_dims] =
      as_rpc::kernel_impl::mean_reduction(data, dims, {1});
  EXPECT_EQ(expected_data.size(), actual_data.size());
  EXPECT_DOUBLE_EQ(expected_data[0], actual_data[0]);
  EXPECT_EQ(expected_dims, actual_dims);
}

TEST(Math, MeanInts1D)
{
  const std::vector<int> data{12, 13, 14, 15, 16, 17};
  const std::vector<hsize_t> dims{data.size()};
  const std::vector<double> expected_data{14.5};
  const std::vector<hsize_t> expected_dims{1};

  const auto [actual_data, actual_dims] =
      as_rpc::kernel_impl::mean_reduction(data, expected_dims, {1});
  EXPECT_EQ(expected_data.size(), actual_data.size());
  EXPECT_DOUBLE_EQ(expected_data[0], actual_data[0]);
  EXPECT_EQ(expected_dims, actual_dims);
}

TEST(Math, MeanZeroElementDeathTest)
{
  EXPECT_DEBUG_DEATH(as_rpc::kernel_impl::mean_reduction<int>({}, {1}, {1}),
                     "Passed vector of size 0!");
}

TEST(Math, MeanDimMismatchDeathTest)
{
  EXPECT_DEBUG_DEATH(as_rpc::kernel_impl::mean_reduction<int>({1}, {1}, {1, 0}),
                     "Dimensionalities do not match!");
}

TEST(Math, MeanReduceAll2D)
{
  std::vector<double> data{};
  const std::vector<hsize_t> dims{5, 10};
  const std::vector<char> reduce_along_dim{1, 1};
  for (decltype(dims)::value_type i = 0; i < dims[0] * dims[1]; i++)
    data.push_back(5.55 * i);

  auto sum = std::reduce(data.cbegin(), data.cend(), 0.0);
  std::vector<double> expected_data{sum / data.size()};
  const std::vector<hsize_t> expected_dims{1};

  const auto [actual_data, actual_dims] =
      as_rpc::kernel_impl::mean_reduction(data, dims, reduce_along_dim);
  EXPECT_EQ(expected_data.size(), actual_data.size());
  EXPECT_DOUBLE_EQ(expected_data[0], actual_data[0]);
  EXPECT_EQ(expected_dims.size(), actual_dims.size());
}

TEST(Math, MeanReduceFirst2D)
{
  std::vector<double> data{};
  std::vector<double> expected_data{};
  const std::vector<hsize_t> expected_dims{10};

  const std::vector<hsize_t> dims{5, 10};
  const std::vector<char> reduce_along_dim{1, 0};

  for (decltype(dims)::value_type i = 0; i < dims[0]; i++)
  {
    for (decltype(dims)::value_type j = 0; j < dims[1]; j++)
      data.push_back(10.0 * j);
  }

  for (decltype(dims)::value_type i = 0; i < dims[1]; i++)
    expected_data.push_back(10.0 * i);

  const auto [actual_data, actual_dims] =
      as_rpc::kernel_impl::mean_reduction(data, dims, reduce_along_dim);
  EXPECT_EQ(10, actual_data.size());
  EXPECT_EQ(expected_data, actual_data);
  EXPECT_EQ(expected_dims, actual_dims);
}

TEST(Math, MeanReduceSecond2D)
{
  std::vector<double> data{};
  std::vector<double> expected_data{};
  const std::vector<hsize_t> expected_dims{5};

  const std::vector<hsize_t> dims{5, 10};
  const std::vector<char> reduce_along_dim{0, 1};

  for (decltype(dims)::value_type i = 0; i < dims[0]; i++)
  {
    for (decltype(dims)::value_type j = 0; j < dims[1]; j++)
      data.push_back(10.0 * j + i);
  }

  for (decltype(dims)::value_type i = 0; i < dims[0]; i++)
    expected_data.push_back(45.0 + i);

  const auto [actual_data, actual_dims] =
      as_rpc::kernel_impl::mean_reduction(data, dims, reduce_along_dim);
  EXPECT_EQ(5, actual_data.size());
  EXPECT_EQ(expected_data, actual_data);
  EXPECT_EQ(expected_dims, actual_dims);
}

} // namespace

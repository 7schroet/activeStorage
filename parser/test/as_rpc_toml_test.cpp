/*
 * Copyright (c) 2026 Niclas Schroeter
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

#include "../include/as_rpc_config_parse.hpp"
#include <gtest/gtest.h>
#include <sstream>

namespace
{

void assert_operation_eq(const as_rpc_config::Operation& expected,
                         const as_rpc_config::Operation& actual)
{
  EXPECT_EQ(expected.dims, actual.dims);
  EXPECT_EQ(expected.dset, actual.dset);
  EXPECT_EQ(expected.infile, actual.infile);
  EXPECT_EQ(expected.outfile, actual.outfile);
  EXPECT_EQ(expected.kernel, actual.kernel);
}

TEST(TOML, EmptyDeath)
{
  std::string empty = "";
  std::stringstream input{empty};
  EXPECT_ANY_THROW(auto _ = as_rpc_config::parse_toml(input););
}

TEST(TOML, MissingOperationsTableDeath)
{
  std::string no_ops_table = R"(
[[op]]
type = "mean"
dims = [1,1,1]
infile = "a.h5"
dset = "dataset"
outfile = "outmean.h5"
      )";

  std::stringstream input{no_ops_table};
  EXPECT_ANY_THROW(auto _ = as_rpc_config::parse_toml(input););
}

TEST(TOML, OperationsNotTableDeath)
{
  std::string ops_not_table = R"(
[operations]
type = "hello"
dims = [1,1,1]
infile = "a.h5"
dset = "dataset"
outfile = "../outmean.h5"
  )";

  std::stringstream input{ops_not_table};
  EXPECT_EXIT(as_rpc_config::parse_toml(input),
              testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(TOML, MissingElementDeath)
{
  std::string missing_dims = R"(
[[operations]]
type = "mean"
infile = "a.h5"
dset = "dataset"
outfile = "outmean.h5"
      )";

  std::stringstream input{missing_dims};
  EXPECT_ANY_THROW(auto _ = as_rpc_config::parse_toml(input););
}

TEST(TOML, InvalidKernelDeath)
{
  std::string invalid_kernel = R"(
[[operations]]
type = "notDefined"
dims = [1,1,1]
infile = "a.h5"
dset = "dataset"
outfile = "../outmean.h5"
      )";

  std::stringstream input{invalid_kernel};
  EXPECT_ANY_THROW(auto _ = as_rpc_config::parse_toml(input););
}

TEST(TOML, SingleOperation)
{
  std::string single_op = R"(
[[operations]]
type = "hello"
dims = [1,1,1]
infile = "a.h5"
dset = "dataset"
outfile = "../outmean.h5"
      )";

  as_rpc_config::Operation expected{"a.h5", "../outmean.h5", "dataset",
                                    std::vector<char>{1, 1, 1},
                                    as_rpc::Kernel::hello};

  std::stringstream input{single_op};
  auto actual = as_rpc_config::parse_toml(input);
  ASSERT_EQ(1, actual.size());
  assert_operation_eq(expected, actual[0]);
}

TEST(TOML, SingleOperationExtraKeyInOperations)
{
  std::string extra_key_in_table = R"(
[[operations]]
type = "mean"
dims = [1,1,0]
infile = "a.h5"
dset = "dataset"
outfile = "../outmean.h5"
extraKey = "value"
      )";

  as_rpc_config::Operation expected{"a.h5", "../outmean.h5", "dataset",
                                    std::vector<char>{1, 1, 0},
                                    as_rpc::Kernel::mean};

  std::stringstream input{extra_key_in_table};
  auto actual = as_rpc_config::parse_toml(input);
  ASSERT_EQ(1, actual.size());
  assert_operation_eq(expected, actual[0]);
}

TEST(TOML, SingleOperationExtraKeyOutside)
{
  std::string extra_key_outside_table = R"(
extraKey = "value"

[[operations]]
type = "mean"
dims = [1,1,0]
infile = "a.h5"
dset = "dataset"
outfile = "../outmean.h5"
      )";

  as_rpc_config::Operation expected{"a.h5", "../outmean.h5", "dataset",
                                    std::vector<char>{1, 1, 0},
                                    as_rpc::Kernel::mean};

  std::stringstream input{extra_key_outside_table};
  auto actual = as_rpc_config::parse_toml(input);
  ASSERT_EQ(1, actual.size());
  assert_operation_eq(expected, actual[0]);
}

TEST(TOML, MultipleOperations)
{
  std::string multiple_ops = R"(
[[operations]]
type = "mean"
dims = [1,1,1]
infile = "a.h5"
dset = "dataset"
outfile = "/path/to/outmean.h5"

[[operations]]
type = "hello"
dset = "alsoDataset"
outfile = "outmean2.h5"
dims = [0,1,1]
infile = "b.h5"
      )";

  as_rpc_config::Operations expected{};
  expected.emplace_back("a.h5", "/path/to/outmean.h5", "dataset",
                        std::vector<char>{1, 1, 1}, as_rpc::Kernel::mean);

  expected.emplace_back("b.h5", "outmean2.h5", "alsoDataset",
                        std::vector<char>{0, 1, 1}, as_rpc::Kernel::hello);

  std::stringstream input{multiple_ops};
  auto actual = as_rpc_config::parse_toml(input);
  ASSERT_EQ(expected.size(), actual.size());
  for (decltype(expected.size()) i = 0; i < expected.size(); i++)
    assert_operation_eq(expected[i], actual[i]);
}

} // namespace

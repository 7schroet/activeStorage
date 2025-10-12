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

#include "../argparse.cpp"
#include "gtest/gtest.h"
#include <cstdlib>
#include <iostream>
#include <streambuf>

namespace
{

class ClientConfigTest : public testing::Test
{
protected:
  void SetUp() override
  {
    // required because of how getopt works
    optind = 1;
    // redirecting stderr to stdout because gtest suppresses stderr
    old_out = std::cerr.rdbuf();
    std::cerr.rdbuf(std::cout.rdbuf());
  }

  void TearDown() override
  {
    std::cerr.rdbuf(old_out);
    old_out = nullptr;
  }

  std::streambuf* old_out;
};

static void assert_config_eq(const ClientConfig& expected,
                             const ClientConfig& actual)
{
  EXPECT_EQ(expected.server_address_file, actual.server_address_file);
  EXPECT_EQ(expected.protocol, actual.protocol);
  EXPECT_EQ(expected.randomize_data, actual.randomize_data);
  EXPECT_EQ(expected.reduce_along_dim, actual.reduce_along_dim);
}

TEST_F(ClientConfigTest, Default)
{
  ClientConfig expected{};

  constexpr int argc = 1;
  const char* argv[argc] = {"as-client"};
  ClientConfig actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ClientConfigTest, PassProtocol)
{
  ClientConfig expected{};
  expected.protocol = as_rpc::Protocol::verbs;

  constexpr int argc = 3;
  const char* argv[argc] = {"as-client", "--protocol", "verbs"};
  ClientConfig actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ClientConfigTest, PassAddress)
{
  ClientConfig expected{};
  expected.server_address_file = "../filename";

  constexpr int argc = 3;
  const char* argv[argc] = {"as-client", "--addressfile", "../filename"};
  ClientConfig actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ClientConfigTest, PassRandom)
{
  ClientConfig expected{};
  expected.randomize_data = true;

  constexpr int argc = 2;
  const char* argv[argc] = {"as-client", "--random"};
  ClientConfig actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ClientConfigTest, PassMeanReduce)
{
  ClientConfig expected{};
  expected.reduce_along_dim = {1, 0, 1};

  constexpr int argc = 3;
  const char* argv[argc] = {"as-client", "--mean", "101"};
  ClientConfig actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ClientConfigTest, PassAll)
{
  ClientConfig expected{};
  expected.protocol = as_rpc::Protocol::tcp;
  expected.server_address_file = "file";
  expected.randomize_data = true;
  expected.reduce_along_dim = {0, 0, 1};

  constexpr int argc = 8;
  const char* argv[argc] = {"as-client",     "--protocol", "tcp",
                            "--addressfile", "file",       "--random",
                            "--mean",        "001"};
  ClientConfig actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ClientConfigTest, HelpDeathTest)
{
  constexpr int argc = 2;
  const char* argv[argc] = {"as-client", "--help"};
  EXPECT_EXIT(parse_args(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(EXIT_SUCCESS), "");
}

TEST_F(ClientConfigTest, MissingArgDeathTest)
{
  constexpr int argc = 4;
  const char* argv[argc] = {"as-client", "--protocol", "--addressfile",
                            "long/file/path"};
  EXPECT_EXIT(parse_args(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(ClientConfigTest, UnknownProtocolDeathTest)
{
  constexpr int argc = 3;
  const char* argv[argc] = {"as-client", "--protocol", "unknown"};
  EXPECT_EXIT(parse_args(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(ClientConfigTest, MeanStringLengthDeathTest)
{
  constexpr int argc = 3;
  const char* argv[argc] = {"as-client", "--mean", "11111"};
  EXPECT_EXIT(parse_args(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(ClientConfigTest, MeanStringSymbolDeathTest)
{
  constexpr int argc = 3;
  const char* argv[argc] = {"as-client", "--mean", "131"};
  EXPECT_EXIT(parse_args(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(ClientConfigTest, UnknownOptionDeathTest)
{
  constexpr int argc = 2;
  const char* argv[argc] = {"as-client", "--unknown-opt"};
  EXPECT_EXIT(parse_args(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(EXIT_FAILURE), "");
}
} // namespace

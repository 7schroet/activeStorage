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

class ConfigTest : public testing::Test
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

static void assert_config_eq(const Config& expected, const Config& actual)
{
  EXPECT_EQ(expected.address_file, actual.address_file);
  EXPECT_EQ(expected.port, actual.port);
  EXPECT_EQ(expected.protocol, actual.protocol);
}

TEST_F(ConfigTest, Default)
{
  Config expected = {
      .protocol = as_rpc::Protocol::tcp,
      .address_file = "servername",
      .port = "8080",
  };

  constexpr int argc = 1;
  const char* argv[argc] = {"as-server"};
  Config actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ConfigTest, PassPort)
{
  Config expected = {
      .protocol = as_rpc::Protocol::tcp,
      .address_file = "servername",
      .port = "12345",
  };

  constexpr int argc = 3;
  const char* argv[argc] = {"as-server", "--port", "12345"};
  Config actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ConfigTest, PassProtocol)
{
  Config expected = {
      .protocol = as_rpc::Protocol::verbs,
      .address_file = "servername",
      .port = "8080",
  };

  constexpr int argc = 3;
  const char* argv[argc] = {"as-server", "--protocol", "verbs"};
  Config actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ConfigTest, PassAddress)
{
  Config expected = {
      .protocol = as_rpc::Protocol::tcp,
      .address_file = "../filename",
      .port = "8080",
  };

  constexpr int argc = 3;
  const char* argv[argc] = {"as-server", "--addressfile", "../filename"};
  Config actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ConfigTest, PassAll)
{
  Config expected = {
      .protocol = as_rpc::Protocol::tcp,
      .address_file = "file",
      .port = "6666",
  };

  constexpr int argc = 7;
  const char* argv[argc] = {"as-server", "--port",        "6666", "--protocol",
                            "tcp",       "--addressfile", "file"};
  Config actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ConfigTest, HelpDeathTest)
{
  constexpr int argc = 2;
  const char* argv[argc] = {"as-server", "--help"};
  EXPECT_EXIT(parse_args(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(EXIT_SUCCESS), "");
}

TEST_F(ConfigTest, MissingArgDeathTest)
{
  constexpr int argc = 6;
  const char* argv[argc] = {"as-server",  "--port", "6666",
                            "--protocol", "tcp",    "--addressfile"};
  EXPECT_EXIT(parse_args(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(ConfigTest, UnknownProtocolDeathTest)
{
  constexpr int argc = 3;
  const char* argv[argc] = {"as-server", "--protocol", "unknown"};
  EXPECT_EXIT(parse_args(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(ConfigTest, UnknownOptionDeathTest)
{
  constexpr int argc = 2;
  const char* argv[argc] = {"as-server", "--unknown-opt"};
  EXPECT_EXIT(parse_args(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(EXIT_FAILURE), "");
}
} // namespace

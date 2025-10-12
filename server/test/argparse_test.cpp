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

class ServerConfigTest : public testing::Test
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

static void assert_config_eq(const ServerConfig& expected,
                             const ServerConfig& actual)
{
  EXPECT_EQ(expected.address_file, actual.address_file);
  EXPECT_EQ(expected.port, actual.port);
  EXPECT_EQ(expected.protocol, actual.protocol);
}

TEST_F(ServerConfigTest, Default)
{
  ServerConfig expected{};

  constexpr int argc = 1;
  const char* argv[argc] = {"as-server"};
  ServerConfig actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ServerConfigTest, PassPort)
{
  ServerConfig expected{};
  expected.port = "12345";

  constexpr int argc = 3;
  const char* argv[argc] = {"as-server", "--port", "12345"};
  ServerConfig actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ServerConfigTest, PassProtocol)
{
  ServerConfig expected{};
  expected.protocol = as_rpc::Protocol::verbs;

  constexpr int argc = 3;
  const char* argv[argc] = {"as-server", "--protocol", "verbs"};
  ServerConfig actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ServerConfigTest, PassAddress)
{
  ServerConfig expected{};
  expected.address_file = "../filename";

  constexpr int argc = 3;
  const char* argv[argc] = {"as-server", "--addressfile", "../filename"};
  ServerConfig actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ServerConfigTest, PassAll)
{
  ServerConfig expected{};
  expected.protocol = as_rpc::Protocol::tcp;
  expected.address_file = "file";
  expected.port = "6666";

  constexpr int argc = 7;
  const char* argv[argc] = {"as-server", "--port",        "6666", "--protocol",
                            "tcp",       "--addressfile", "file"};
  ServerConfig actual = parse_args(argc, const_cast<char**>(argv));
  assert_config_eq(expected, actual);
}

TEST_F(ServerConfigTest, HelpDeathTest)
{
  constexpr int argc = 2;
  const char* argv[argc] = {"as-server", "--help"};
  EXPECT_EXIT(parse_args(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(EXIT_SUCCESS), "");
}

TEST_F(ServerConfigTest, MissingArgDeathTest)
{
  constexpr int argc = 6;
  const char* argv[argc] = {"as-server",  "--port", "6666",
                            "--protocol", "tcp",    "--addressfile"};
  EXPECT_EXIT(parse_args(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(ServerConfigTest, UnknownProtocolDeathTest)
{
  constexpr int argc = 3;
  const char* argv[argc] = {"as-server", "--protocol", "unknown"};
  EXPECT_EXIT(parse_args(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(ServerConfigTest, UnknownOptionDeathTest)
{
  constexpr int argc = 2;
  const char* argv[argc] = {"as-server", "--unknown-opt"};
  EXPECT_EXIT(parse_args(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(EXIT_FAILURE), "");
}
} // namespace

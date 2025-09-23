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

#include "as_rpc.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iterator>
#include <string>

namespace
{

class ServerTest : public testing::Test
{
protected:
  void SetUp() override
  {
    my_port = get_next_port();
    std::string port = std::to_string(my_port);
    std::string address = as_rpc::construct_address(protocol, port);
    engine = as_rpc::init_engine(address, true);
  }

  void TearDown() override { as_rpc::stop_server(engine); }

  static unsigned get_next_port()
  {
    static unsigned port = 8081;
    return port++;
  }

  as_rpc::Engine engine;
  unsigned my_port;
  static const as_rpc::Protocol protocol = as_rpc::Protocol::tcp;
};

/**
 * Is here to ensure that the starting and stopping works as
 * expected, hence the empty test body.
 */
TEST_F(ServerTest, StartAndShutdown) {}

TEST_F(ServerTest, AddressFileCorrect)
{
  std::filesystem::path filename{std::tmpnam(nullptr)};
  as_rpc::write_address_to_file(engine, filename);

  std::ifstream address_in{filename};
  EXPECT_TRUE(address_in.good());
  std::string address{std::istreambuf_iterator<char>(address_in),
                      std::istreambuf_iterator<char>()};

  std::string port_str{std::to_string(my_port)};
  std::string protocol_str = as_rpc::protocol_to_string(protocol);

  // Refer to Mercury(https://mercury-hpc.github.io/user/na/) for their
  // structure on addresses.
  std::string regex = std::format("ofi\\+{}://.*:{}", protocol_str, port_str);
  EXPECT_THAT(address, testing::MatchesRegex(regex));

  std::filesystem::remove(filename);
}

TEST_F(ServerTest, ClientCanConnect)
{
  std::filesystem::path filename{std::tmpnam(nullptr)};
  as_rpc::write_address_to_file(engine, filename);

  std::string protocol_str = as_rpc::protocol_to_string(protocol);
  as_rpc::Engine client_engine = as_rpc::init_engine(protocol_str, false);
  const std::string server_address = as_rpc::get_address_from_file(filename);
  as_rpc::ServerEndpoint server =
      as_rpc::connect_to_server(engine, server_address);

  std::filesystem::remove(filename);
}

} // namespace

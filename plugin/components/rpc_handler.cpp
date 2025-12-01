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

#include "rpc_handler.hpp"
#include <print>

namespace
{
as_rpc::Engine engine;
as_rpc::ServerEndpoint server;
as_rpc::RemoteProcedures rpc_kernels;

as_rpc::ServerEndpoint find_server()
{
  auto address_file = std::getenv("AS_RPC_SERVER_ADDRESS");
  const std::string server_address =
      as_rpc::get_address_from_file(address_file);
  return as_rpc::connect_to_server(engine, server_address);
}
} // namespace

void register_rpc_client()
{
  const std::string client_address =
      as_rpc::protocol_to_string(as_rpc::Protocol::tcp);

  engine = as_rpc::init_engine(client_address, false);
  rpc_kernels = as_rpc::register_kernels_at_client(engine);
  server = find_server();

  auto search = rpc_kernels.find(as_rpc::Kernel::hello);
  if (search != rpc_kernels.end())
  {
    search->second.on(server)();
  }
}

void rpc_mean(const std::string& filename, const std::string& dset_name,
              unsigned timestep, const std::vector<char>& reduce_along_dim)
{
  auto search = rpc_kernels.find(as_rpc::Kernel::mean);
  if (search == rpc_kernels.end())
  {
    std::println(stderr, "Couldn't find mean RPC!");
    exit(1);
  }
  search->second.on(server)(filename, dset_name, timestep, reduce_along_dim);
}

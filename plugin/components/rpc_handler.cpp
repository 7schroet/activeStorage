/*
 * Copyright (c) 2025 - 2026 Niclas Schroeter
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
#include "as_rpc.hpp"
#include "as_rpc_config_parse.hpp"
#include <fstream>
#include <print>

namespace
{

class SingleOperation
{

public:
  explicit SingleOperation(const as_rpc_config::Operation& config_op,
                           unsigned timestep)
      : op_{config_op.kernel}, timestep_{timestep},
        reduce_along_dim_{config_op.dims}, filename_{config_op.infile},
        dset_name_{config_op.dset} {};

  [[nodiscard]] const std::string& filename() const { return filename_; };
  [[nodiscard]] const std::string& dset_name() const { return dset_name_; };

  void dispatch(const as_rpc::ServerEndpoint& server,
                const as_rpc::RemoteProcedures& rpc_kernels) const
  {
    std::println("---------------------");
    std::println("Dispatching:");
    std::println("File: {}", filename_);
    std::println("Dset: {}", dset_name_);
    std::println("Time: {}", timestep_);
    std::println("Op: {}", static_cast<int>(op_));
    std::print("Reduction: ");
    for (auto const& el : reduce_along_dim_)
      std::print("{}, ", static_cast<int>(el));
    std::println();
    std::println("---------------------");
    auto search = rpc_kernels.find(op_);
    if (search == rpc_kernels.end())
      return;

    switch (op_)
    {
    case as_rpc::Kernel::mean:
      search->second.on(server)(filename_, dset_name_, timestep_,
                                reduce_along_dim_);
      break;
    case as_rpc::Kernel::hello:
      search->second.on(server)();
      break;
    }
  }

private:
  as_rpc::Kernel op_;
  unsigned timestep_;
  std::vector<char> reduce_along_dim_;
  std::string filename_;
  std::string dset_name_;
};

as_rpc::Engine engine;
as_rpc::ServerEndpoint server;
as_rpc::RemoteProcedures rpc_kernels;
std::vector<SingleOperation> single_ops{};
as_rpc_config::Operations configured_ops{};

as_rpc::ServerEndpoint find_server()
{
  auto address_file = std::getenv("AS_RPC_SERVER_ADDRESS");
  const std::string server_address =
      as_rpc::get_address_from_file(address_file);
  return as_rpc::connect_to_server(engine, server_address);
}
} // namespace

void parse_as_rpc_config(const std::filesystem::path& path)
{
  std::ifstream in{path};
  configured_ops = as_rpc_config::parse_toml(in);
}

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

void register_single_operation(std::string_view filename,
                               std::string_view dset_name, unsigned timestep)
{
  for (const auto& op : configured_ops)
  {
    if (filename == op.infile && dset_name == op.dset)
      single_ops.emplace_back(op, timestep);
  }
}

void dispatch_operations(std::string_view filename, std::string_view dset_name)
{
  for (auto it = single_ops.begin(); it != single_ops.end();)
  {
    if (it->dset_name() == dset_name && it->filename() == filename)
    {
      it->dispatch(server, rpc_kernels);
      it = single_ops.erase(it);
    }
    else
    {
      it++;
    }
  }
}

void dispatch_operations(std::string_view filename)
{
  for (auto it = single_ops.begin(); it != single_ops.end();)
  {
    if (it->filename() == filename)
    {
      it->dispatch(server, rpc_kernels);
      it = single_ops.erase(it);
    }
    else
    {
      it++;
    }
  }
}

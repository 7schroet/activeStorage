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

class Operation
{

public:
  explicit Operation(as_rpc::Kernel op, unsigned timestep,
                     std::vector<char> reduce_along_dim, std::string filename,
                     std::string dset_name)
      : op_{op}, timestep_{timestep},
        reduce_along_dim_{std::move(reduce_along_dim)},
        filename_{std::move(filename)}, dset_name_{std::move(dset_name)} {};

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
std::vector<Operation> operations{};

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

void register_operation(const as_rpc::Kernel op,
                        const std::filesystem::path& filename,
                        const std::string& dset_name, unsigned timestep,
                        const std::vector<char>& reduce_along_dim)
{
  operations.emplace_back(op, timestep, reduce_along_dim, filename, dset_name);
}

void dispatch_operation(const std::filesystem::path& filename,
                        const std::string& dset_name)
{
  for (auto it = operations.begin(); it != operations.end();)
  {
    if (it->dset_name() == dset_name && it->filename() == std::string(filename))
    {
      it->dispatch(server, rpc_kernels);
      it = operations.erase(it);
    }
    else
    {
      it++;
    }
  }
}

void dispatch_operation(const std::filesystem::path& filename)
{
  for (auto it = operations.begin(); it != operations.end();)
  {
    if (it->filename() == std::string(filename))
    {
      it->dispatch(server, rpc_kernels);
      it = operations.erase(it);
    }
    else
    {
      it++;
    }
  }
}

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

#include "argparse.hpp"
#include "as_rpc.hpp"

int main(int argc, char** argv)
{
  Config config = parse_args(argc, argv);
  const std::string client_address =
      as_rpc::protocol_to_string(config.protocol);

  as_rpc::Engine engine = as_rpc::init_engine(client_address, false);
  as_rpc::RemoteProcedures rpc_kernels =
      as_rpc::register_kernels_at_client(engine);

  const std::string server_address =
      as_rpc::get_address_from_file(config.server_address_file);
  as_rpc::ServerEndpoint server =
      as_rpc::connect_to_server(engine, server_address);

  auto search = rpc_kernels.find(as_rpc::Kernel::hello);
  if (search != rpc_kernels.end())
  {
    search->second.on(server)();
  }
}

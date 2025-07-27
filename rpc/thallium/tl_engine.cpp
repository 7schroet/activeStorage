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

#include "as_rpc_engine.hpp"
#include <thallium.hpp>

namespace as_rpc
{

Engine init_engine(const std::string& address, bool is_server)
{
  auto is_server_switch =
      is_server ? THALLIUM_SERVER_MODE : THALLIUM_CLIENT_MODE;
  return thallium::engine{address, is_server_switch};
}

void run_server(Engine& engine) { engine.wait_for_finalize(); }

void stop_server(Engine& engine) { engine.finalize(); }

ServerEndpoint connect_to_server(Engine& engine,
                                 const std::string& server_address)
{
  return engine.lookup(server_address);
}

} // namespace as_rpc

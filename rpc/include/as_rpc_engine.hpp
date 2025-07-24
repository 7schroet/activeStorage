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

#ifndef AS_RPC_ENGINE_HPP
#define AS_RPC_ENGINE_HPP

#include "as_rpc_types.hpp"

namespace as_rpc
{

/**
 * The address for the RPC engine should be constructed
 * via as_rpc::construct_address(). If the engine is
 * supposed to receive RPCs, acting as a server,
 * call this function with is_server=true.
 */
Engine init_engine(const std::string& address, bool is_server);

/**
 * Once a server engine is done with the setup,
 * call this function to keep the server online.
 */
void run_server(Engine& engine);

/**
 * Use this function to stop a running server,
 * i.e. in a signal handler.
 */
void stop_server(Engine& engine);

} // namespace as_rpc

#endif

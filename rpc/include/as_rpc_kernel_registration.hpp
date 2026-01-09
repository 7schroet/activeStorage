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

#ifndef AS_RPC_KERNEL_REGISTRATION_HPP
#define AS_RPC_KERNEL_REGISTRATION_HPP

#include "as_rpc_types.hpp"

namespace as_rpc
{
/**
 * Servers need to call this at some point
 * before starting the main loop.
 */
void register_kernels_at_server(Engine& engine);

/**
 * Clients must call this function instead to
 * get access to the RPCs on the server.
 */
const RemoteProcedures register_kernels_at_client(Engine& engine);
} // namespace as_rpc

#endif

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

#include "as_rpc_kernels.hpp"
#include "as_rpc_types.hpp"
#include <thallium.hpp>

namespace
{
void hello(const thallium::request& req) { std::cout << "Received RPC\n"; }

/**
 * Registering the kernels with thallium requires both
 * client and server to provide the same kernel name
 * via a string.
 */
std::string kernel_to_string(as_rpc::Kernel kernel)
{
  static const std::string kernel_strings[] = {FOREACH_KERNEL(GENERATE_STRING)};
  return kernel_strings[std::to_underlying(kernel)];
}
} // namespace

namespace as_rpc
{
void register_kernels_at_server(Engine& engine)
{
  engine.define(kernel_to_string(Kernel::hello), hello).disable_response();
}

RemoteProcedures register_kernels_at_client(Engine& engine)
{
  RemoteProcedures res{};
  auto hello =
      engine.define(kernel_to_string(Kernel::hello)).disable_response();
  res.emplace(Kernel::hello, std::move(hello));
  return res;
}
} // namespace as_rpc

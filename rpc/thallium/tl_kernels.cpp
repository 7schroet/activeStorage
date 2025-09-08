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
#include "tl_kernel_impl.hpp"
#include <thallium.hpp>
#include <utility>

namespace
{
const char* kernel_strings[] = {ASRPC_FOREACH_KERNEL(ASRPC_GENERATE_STRING)};
}

namespace as_rpc
{

void register_kernels_at_server(Engine& engine)
{
#define X(INPUT)                                                               \
  engine                                                                       \
      .define(kernel_strings[std::to_underlying(Kernel::INPUT)],               \
              kernel_impl::INPUT)                                              \
      .disable_response();
  ASRPC_FOREACH_KERNEL(X)
#undef X
}

const RemoteProcedures register_kernels_at_client(Engine& engine)
{
  RemoteProcedures res{};

#define X(INPUT)                                                               \
  auto proc_INPUT =                                                            \
      engine.define(kernel_strings[std::to_underlying(Kernel::INPUT)])         \
          .disable_response();                                                 \
  res.emplace(Kernel::INPUT, std::move(proc_INPUT));
  ASRPC_FOREACH_KERNEL(X)
#undef X

  return res;
}
} // namespace as_rpc

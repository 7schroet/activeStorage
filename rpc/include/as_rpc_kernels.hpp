/*
 * Copyright (c) 2026 Niclas Schroeter
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

#ifndef AS_RPC_KERNELS_HPP
#define AS_RPC_KERNELS_HPP

#include "as_rpc_macros.hpp"

/**
 * This macro is supposed to be the single "point of truth"
 * for existing kernels. Whenever you want to add a kernel,
 * add the implementation somewhere and then add the name
 * of the new kernel to this macro.
 */
#define ASRPC_FOREACH_KERNEL(KERNEL)                                           \
  KERNEL(hello)                                                                \
  KERNEL(mean)

namespace as_rpc
{

enum class Kernel
{
  ASRPC_FOREACH_KERNEL(ASRPC_GENERATE_ENUM)
};

}
#endif

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
#include <cstdint>
#include <format>
#include <stdexcept>
#include <string_view>
#include <utility>

/**
 * This macro is supposed to be the single "point of truth"
 * for existing kernels. Whenever you want to add a kernel,
 * add the implementation somewhere and then add the name
 * of the new kernel to this macro. Also add it to the string
 * conversion function.
 */
#define ASRPC_FOREACH_KERNEL(KERNEL)                                           \
  KERNEL(hello)                                                                \
  KERNEL(mean)                                                                 \
  KERNEL(max)

namespace as_rpc
{

enum class Kernel : std::uint8_t
{
  ASRPC_FOREACH_KERNEL(ASRPC_GENERATE_ENUM)
};

inline constexpr std::string_view kernel_to_string(Kernel kernel)
{
  static const std::string kernel_strings[] = {
      ASRPC_FOREACH_KERNEL(ASRPC_GENERATE_STRING)};
  return kernel_strings[std::to_underlying(kernel)];
}

inline constexpr Kernel string_to_kernel(std::string_view kernel_string)
{
  if (kernel_string == "hello")
  {
    return Kernel::hello;
  }
  else if (kernel_string == "mean")
  {
    return Kernel::mean;
  }
  else if (kernel_string == "max")
  {
    return Kernel::max;
  }
  else
  {
    throw std::runtime_error(std::format(
        "Unknown kernel '{}'. Did you forget to add the string conversion?\n",
        kernel_string));
  }
}

} // namespace as_rpc
#endif

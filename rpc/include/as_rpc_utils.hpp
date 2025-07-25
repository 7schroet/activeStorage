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

#ifndef AS_RPC_UTILS_HPP
#define AS_RPC_UTILS_HPP

#include "as_rpc_protocols.hpp"
#include "as_rpc_types.hpp"
#include <filesystem>
#include <string>

namespace as_rpc
{

/**
 * Use this function to construct addresses for engine
 * initialization. Don't try to build them on your own.
 */
std::string construct_address(Protocol protocol, const std::string& port);

/**
 * This function writes the address of a server to a file, so that
 * clients can read the address to set up the communcation.
 */
void write_address_to_file(const Engine& engine,
                           const std::filesystem::path& filename);

} // namespace as_rpc

#endif

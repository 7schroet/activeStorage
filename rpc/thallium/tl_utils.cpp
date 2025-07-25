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

#include "as_rpc_protocols.hpp"
#include "as_rpc_types.hpp"
#include "as_rpc_utils.hpp"
#include <cstdlib>
#include <format>
#include <fstream>
#include <iostream>

namespace as_rpc
{

std::string construct_address(Protocol protocol, const std::string& port)
{
  std::string protocol_string = protocol_to_string(protocol);
  return std::format("{}://:{}", protocol_string, port);
}

void write_address_to_file(const Engine& engine,
                           const std::filesystem::path& filename)
{
  std::ofstream address_file{filename, std::ios_base::trunc};
  if (!address_file.good())
  {
    std::cerr << "Could not write address to file " << filename
              << ", aborting\n";
    exit(EXIT_FAILURE);
  }
  address_file << engine.self();
  address_file.close();
}
} // namespace as_rpc

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

#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include "as_rpc_protocols.hpp"
#include <filesystem>
#include <vector>
struct ClientConfig
{
  as_rpc::Protocol protocol{};
  std::filesystem::path server_address_file{"servername"};
  bool randomize_data{};
  std::vector<char> reduce_along_dim{1, 1, 1};
};

ClientConfig parse_args(int argc, char** argv);

#endif

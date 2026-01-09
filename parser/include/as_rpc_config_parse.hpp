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

#ifndef AS_RPC_CONFIG_PARSE_HPP
#define AS_RPC_CONFIG_PARSE_HPP

#include "as_rpc_kernels.hpp"
#include <istream>
#include <string>
#include <vector>

namespace as_rpc_config
{
struct Operation
{
  std::string infile;
  std::string outfile;
  std::string dset;
  std::vector<char> dims;
  as_rpc::Kernel kernel;
};

using Operations = std::vector<Operation>;

Operations parse_toml(std::istream& input);
} // namespace as_rpc_config

#endif

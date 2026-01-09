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

#include "as_rpc_config_parse.hpp"
#include <print>
#include <toml.hpp>

namespace as_rpc_config
{

Operations parse_toml(std::istream& input)
{
  constexpr char ARRAY_NAME[] = "operations";
  const toml::value toml_in = toml::parse(input);
  Operations operations{};

  auto toml_operations = toml_in.at(ARRAY_NAME);
  if (!toml_operations.is_array_of_tables())
  {
    std::println(stderr, "'{}' must be an array of tables!", ARRAY_NAME);
    std::println(stderr, "Declare as such:\n[[{}]]\ntype = ...", ARRAY_NAME);
    exit(EXIT_FAILURE);
  }

  for (const auto& table : toml_operations.as_array())
  {
    operations.emplace_back(
        toml::find<std::string>(table, "infile"),
        toml::find<std::string>(table, "outfile"),
        toml::find<std::string>(table, "dset"),
        toml::find<std::vector<char>>(table, "dims"),
        as_rpc::string_to_kernel(toml::find<std::string>(table, "type")));
  }

  return operations;
}
} // namespace as_rpc_config

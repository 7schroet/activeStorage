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

#include "as_rpc_kernels.hpp"
#include <cstdlib>
#include <fstream>
#include <istream>
#include <print>
#include <toml.hpp>
#include <vector>

namespace
{
struct as_rpc_operation
{
  std::string infile;
  std::string outfile;
  std::string dset;
  std::vector<char> dims;
  as_rpc::Kernel kernel;

  explicit as_rpc_operation(const toml::value& v)
      : infile{toml::find<std::string>(v, "infile")},
        outfile{toml::find<std::string>(v, "outfile")},
        dset{toml::find<std::string>(v, "dset")},
        dims{toml::find<std::vector<char>>(v, "dims")},
        kernel{as_rpc::string_to_kernel(toml::find<std::string>(v, "type"))}
  {
  }
};

using Operations = std::vector<as_rpc_operation>;

Operations parse_toml(std::istream& input)
{
  constexpr char ARRAY_NAME[] = "operations";
  const toml::value toml_in = toml::parse(input);
  Operations operations{};

  auto toml_operations = toml_in.at(ARRAY_NAME);
  if (!toml_operations.is_array_of_tables())
  {
    std::println(stderr, "'{}' must be an array of tables!", ARRAY_NAME);
    exit(EXIT_FAILURE);
  }

  for (const auto& table : toml_operations.as_array())
    operations.emplace_back(table);

  return operations;
}
} // namespace

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::println(stderr, "Path to toml file required!");
    return EXIT_FAILURE;
  }

  std::ifstream input{argv[1]};
  auto ops = parse_toml(input);
  for (const auto& op : ops)
  {
    std::println("{}", as_rpc::kernel_to_string(op.kernel));
    std::println("{}", op.infile);
    std::println("{}", op.outfile);
    std::println("{}", op.dset);
    for (const auto& el : op.dims)
      std::print("{}", static_cast<int>(el));
    std::println();
  }
  return EXIT_SUCCESS;
}

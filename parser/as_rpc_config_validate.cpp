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
#include <cstdlib>
#include <exception>
#include <fstream>
#include <print>

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::println(stderr, "Path to toml file required!");
    return EXIT_FAILURE;
  }

  std::ifstream input{argv[1]};
  try
  {
    auto ops = as_rpc_config::parse_toml(input);
    std::println("Input validated! Echoing the config:");
    for (const auto& op : ops)
    {
      std::println("[[operations]]");
      std::println("type = \"{}\"", as_rpc::kernel_to_string(op.kernel));
      std::println("infile = \"{}\"", op.infile);
      std::println("outfile = \"{}\"", op.outfile);
      std::println("dset = \"{}\"", op.dset);
      std::print("dims = [");
      for (const auto& el : op.dims)
        std::print("{},", static_cast<int>(el));
      std::println("]\n");
    }
  }
  catch (const std::exception& e)
  {
    std::println(stderr, "Input was not valid, error message was:");
    std::println(stderr, "{}", e.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

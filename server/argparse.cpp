/*
 * Copyright (c) 2025 - 2026 Niclas Schroeter
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

#include "argparse.hpp"
#include "as_rpc_protocols.hpp"
#include <cstdlib>
#include <exception>
#include <getopt.h>
#include <iostream>

namespace
{
[[noreturn]] void usage(int exit_code)
{
  std::cerr << "Start the Active Storage server.\n";
  std::cerr << "Usage:\n";
  std::cerr << "\t--help:\t\t\tPrint this message\n";
  std::cerr << "\t--protocol:\t\tPick a protocol (tcp[default],verbs)\n";
  std::cerr << "\t--port:\t\t\tPick a port (default: 8080)\n";
  std::cerr << "\t--addressfile:\t\tPath to the file that contains the "
               "server's address (default: ./servername)\n";
  std::exit(exit_code);
}
} // namespace

ServerConfig parse_args(int argc, char** argv)
{
  ServerConfig config{};

  const struct option options[] = {
      {"help", no_argument, nullptr, 'h'},
      {"protocol", required_argument, nullptr, 'p'},
      {"port", required_argument, nullptr, 'o'},
      {"addressfile", required_argument, nullptr, 'f'},
      {nullptr, 0, nullptr, 0},
  };

  int opt = 0;
  int index = 0;
  while ((opt = getopt_long(argc, argv, ":h", options, &index)) != -1)
  {
    switch (opt)
    {
    case 'h':
      usage(EXIT_SUCCESS);
    case 'p':
      try
      {
        config.protocol = as_rpc::string_to_protocol(optarg);
      }
      catch (const std::exception& e)
      {
        std::cerr << e.what();
        usage(EXIT_FAILURE);
      }
      break;
    case 'o':
      config.port = optarg;
      break;
    case 'f':
      config.address_file = optarg;
      break;
    case ':':
      std::cerr << "Missing argument for " << argv[optind - 1]
                << ", aborting\n";
      usage(EXIT_FAILURE);
    case '?':
      std::cerr << "Unknown option " << argv[optind - 1] << ", aborting\n";
      usage(EXIT_FAILURE);
    }
  }

  return config;
}

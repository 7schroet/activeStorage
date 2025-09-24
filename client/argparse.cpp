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

#include "argparse.hpp"
#include "as_rpc_protocols.hpp"
#include <cstdlib>
#include <exception>
#include <getopt.h>
#include <iostream>

[[noreturn]] static void usage(int exit_code)
{
  std::cerr << "Start the Active Storage client.\n";
  std::cerr << "Usage:\n";
  std::cerr << "\t--help:\t\t\tPrint this message\n";
  std::cerr << "\t--protocol:\t\tPick a protocol (tpc[default],verbs)\n";
  std::cerr << "\t--addressfile:\t\tPath to the file that contains the "
               "server's address (default: ./servername)\n";
  std::cerr << "\t--random: Add random numbers in [0,1) to the written data\n";
  std::exit(exit_code);
}

Config parse_args(int argc, char** argv)
{
  Config config{};

  const struct option options[] = {
      {"help", no_argument, NULL, 'h'},
      {"protocol", required_argument, NULL, 'p'},
      {"addressfile", required_argument, NULL, 'f'},
      {"random", no_argument, NULL, 'r'},
      {NULL, 0, NULL, 0},
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
    case 'f':
      config.server_address_file = optarg;
      break;
    case 'r':
      config.randomize_data = true;
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

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
#include <cstring>
#include <exception>
#include <getopt.h>
#include <iostream>

namespace
{
[[noreturn]] void usage(int exit_code)
{
  std::cerr << "Start the Active Storage client.\n";
  std::cerr << "Usage:\n";
  std::cerr << "\t--help:\t\t\tPrint this message\n";
  std::cerr << "\t--protocol:\t\tPick a protocol (tcp[default],verbs)\n";
  std::cerr << "\t--addressfile:\t\tPath to the file that contains the "
               "server's address (default: ./servername)\n";
  std::cerr << "\t--output:\t\tPath to the output file (default: ./out.h5)\n";
  std::cerr
      << "\t--random:\t\tAdd random numbers in [0,1) to the written data\n";
  std::cerr << "\t--mean:\t\t\tChoose which dimensions to calculate the mean "
               "over (default: \"111\")\n";
  std::cerr << "\t\t\t\tThe input is always 3 digits (0 to keep the dimension, "
               "1 to reduce it)\n";
  std::cerr << "\t--type:\t\t\tData type for written data (double[default], "
               "int, float)\n";
  std::exit(exit_code);
}
} // namespace

ClientConfig parse_args(int argc, char** argv)
{
  ClientConfig config{};

  const struct option options[] = {
      {.name = "help", .has_arg = no_argument, .flag = nullptr, .val = 'h'},
      {.name = "protocol",
       .has_arg = required_argument,
       .flag = nullptr,
       .val = 'p'},
      {.name = "addressfile",
       .has_arg = required_argument,
       .flag = nullptr,
       .val = 'f'},
      {.name = "output",
       .has_arg = required_argument,
       .flag = nullptr,
       .val = 'o'},
      {.name = "random", .has_arg = no_argument, .flag = nullptr, .val = 'r'},
      {.name = "mean",
       .has_arg = required_argument,
       .flag = nullptr,
       .val = 'm'},
      {.name = "type",
       .has_arg = required_argument,
       .flag = nullptr,
       .val = 't'},
      {.name = nullptr, .has_arg = 0, .flag = nullptr, .val = 0},
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
    case 'o':
      config.output_file = optarg;
      break;
    case 'r':
      config.randomize_data = true;
      break;
    case 'm':
      if (strlen(optarg) != 3)
      {
        std::cerr << "Argument for " << argv[optind - 2]
                  << " must be of size 3, aborting...\n";
        usage(EXIT_FAILURE);
      }
      for (auto i = 0; i < 3; i++)
      {
        if (optarg[i] == '1')
          config.reduce_along_dim[i] = 1;
        else if (optarg[i] == '0')
          config.reduce_along_dim[i] = 0;
        else
        {
          std::cerr << "Unknown state " << optarg[i] << " for "
                    << argv[optind - 2] << ", aborting...\n";
          usage(EXIT_FAILURE);
        }
      }
      break;
    case 't':
      if (strcmp(optarg, "double") == 0)
        config.value_type = Datatype::DOUBLE;
      else if (strcmp(optarg, "float") == 0)
        config.value_type = Datatype::FLOAT;
      else if (strcmp(optarg, "int") == 0)
        config.value_type = Datatype::INT;
      else
      {
        std::cerr << "Unknown type " << optarg << ", aborting\n";
        usage(EXIT_FAILURE);
      }
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

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
#include <cstdlib>
#include <getopt.h>
#include <iostream>

[[noreturn]] static void usage(int exit_code)
{
  std::cerr << "Start the Active Storage server.\n";
  std::cerr << "Usage:\n";
  std::cerr << "\t--help:\t\t\tPrint this message\n";
  std::cerr << "\t--protocol:\t\tPick a protocol (tpc[default],verbs)\n";
  std::cerr << "\t--port:\t\t\tPick a port (default: 8080)\n";
  std::cerr << "\t--addressfile:\t\tPath to the file that contains the "
               "server's address (default: ./servername)\n";
  std::exit(exit_code);
}

/*static enum protocol string_to_protocol(char* protocol_string)*/
/*{*/
/*  if (strcmp(protocol_string, "tcp") == 0)*/
/*  {*/
/*    return tcp;*/
/*  }*/
/*  else if (strcmp(protocol_string, "verbs") == 0)*/
/*  {*/
/*    return verbs;*/
/*  }*/
/*  else*/
/*  {*/
/*    fprintf(stderr, "Unknown protocol %s\n", protocol_string);*/
/*    usage(EXIT_FAILURE);*/
/*  }*/
/*}*/

Config parse_args(int argc, char** argv)
{
  Config config{
      /*.protocol = tcp, .address_file = "servername", .port = "8080"};*/
      .protocol = "tcp",
      .address_file = "servername",
      .port = "8080"};

  const struct option options[] = {
      {"help", no_argument, NULL, 'h'},
      {"protocol", required_argument, NULL, 'p'},
      {"port", required_argument, NULL, 'o'},
      {"addressfile", required_argument, NULL, 'f'},
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
      /*config.protocol = string_to_protocol(optarg);*/
      config.protocol = std::string(optarg);
      break;
    case 'o':
      config.port = std::string(optarg);
      break;
    case 'f':
      config.address_file = std::string(optarg);
      break;
    case ':':
      // TODO match optopt to .val and get the long option
      std::cerr << "Missing argument for -" << (char)optopt << ", aborting\n";
      usage(EXIT_FAILURE);
    case '?':
      fprintf(stderr, "Unknown option %s, aborting\n", argv[optind - 1]);
      usage(EXIT_FAILURE);
    }
  }

  return config;
}

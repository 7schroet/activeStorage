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

#include "argparse.h"
#include "rpc.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if __STDC_VERSION__ == 202311L && defined __has_attribute
#if __has_attribute(noreturn)
#define ATT_NORETURN [[noreturn]]
#else
#define ATT_NORETURN _Noreturn
#endif
#else
#define ATT_NORETURN _Noreturn
#endif

ATT_NORETURN static void usage(int exit_code)
{
  fprintf(stderr, "Start the Active Storage server.\n");
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "\t--help:\t\t\tPrint this message\n");
  fprintf(stderr, "\t--protocol:\t\tPick a protocol (tpc[default],verbs)\n");
  fprintf(stderr, "\t--port:\t\t\tPick a port (default: 8080)\n");
  fprintf(stderr, "\t--addressfile:\t\tPath to the file that contains the "
                  "server's address (default: ./servername)\n");
  exit(exit_code);
}

static enum protocol string_to_protocol(char* protocol_string)
{
  if (strcmp(protocol_string, "tcp") == 0)
  {
    return tcp;
  }
  else if (strcmp(protocol_string, "verbs") == 0)
  {
    return verbs;
  }
  else
  {
    fprintf(stderr, "Unknown protocol %s\n", protocol_string);
    usage(EXIT_FAILURE);
  }
}

config parse_args(int argc, char** argv)
{
  config config = {
      .protocol = tcp, .address_file = "servername", .port = "8080"};

  struct option options[] = {
      {"help", no_argument, NULL, 'h'},
      {"protocol", required_argument, NULL, 'p'},
      {"port", required_argument, NULL, 'o'},
      {"addressfile", required_argument, NULL, 'f'},
      {NULL, 0, NULL, 0},
  };

  int opt = 0;
  while ((opt = getopt_long(argc, argv, ":h", options, NULL)) != -1)
  {
    switch (opt)
    {
    case 'h':
      usage(EXIT_SUCCESS);
    case 'p':
      config.protocol = string_to_protocol(optarg);
      break;
    case 'o':
      config.port = optarg;
      break;
    case 'f':
      config.address_file = optarg;
      break;
    case ':':
      fprintf(stderr, "Missing argument for %c, aborting\n", optopt);
      usage(EXIT_FAILURE);
    case '?':
      fprintf(stderr, "Unknown option %s, aborting\n", argv[optind - 1]);
      usage(EXIT_FAILURE);
    }
  }

  return config;
}

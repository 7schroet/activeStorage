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
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#if __STDC_VERSION__ <= 201710L
#include <stdbool.h>
#endif
#include <stdlib.h>

#if __STDC_VERSION__ > 201710L && defined __has_attribute
#if __has_attribute(maybe_unused)
#define ATTR_UNUSED [[maybe_unused]]
#else
#define ATTR_UNUSED
#endif
#else
#define ATTR_UNUSED
#endif

volatile bool keep_running = true;

void interrupt_handle(ATTR_UNUSED int signal)
{
  keep_running = false;
  fprintf(stderr, "\nReceived SIGINT, shutting down server...\n");
}

int main(int argc, char** argv)
{
  config config = parse_args(argc, argv);

  rpc_handle* handle = NULL;
  char* address = rpc_initialize_address(config.protocol, config.port);

  rpc_init(&handle, address, true);
  signal(SIGINT, interrupt_handle);
  free(address);
  rpc_address_to_file(handle, config.address_file);

  rpc_register_kernels(handle);

  while (keep_running)
  {
    rpc_progress(handle);
  }

  rpc_finalize(handle);

  return EXIT_SUCCESS;
}

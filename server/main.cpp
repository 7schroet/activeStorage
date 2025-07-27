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
#include "as_rpc.hpp"
#include <csignal>

static as_rpc::Engine engine;
static struct sigaction sa;

void stop_handler(int signal)
{
  std::cout << "Received signal " << signal
            << ", shutting down the server...\n";
  as_rpc::stop_server(engine);
}

void setup_interrupt_handle()
{
  sa.sa_handler = stop_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
}

int main(int argc, char** argv)
{
  Config config = parse_args(argc, argv);

  std::string addr = as_rpc::construct_address(config.protocol, config.port);
  engine = as_rpc::init_engine(addr, true);
  as_rpc::write_address_to_file(engine, config.address_file);

  setup_interrupt_handle();
  as_rpc::register_kernels_at_server(engine);

  as_rpc::run_server(engine);
}

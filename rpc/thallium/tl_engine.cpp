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

#include "as_rpc_engine.hpp"
#include <charconv>
#include <cstdlib>
#include <cstring>
#include <print>
#include <system_error>
#include <thallium.hpp>

namespace as_rpc
{

Engine init_engine(const std::string& address, bool is_server)
{
  if (is_server)
  {
    char AS_SERVER_THREADS[] = "AS_SERVER_THREADS";
    int thread_count = 1;
    if (const char* const env_value = std::getenv(AS_SERVER_THREADS))
    {
      auto end = env_value + strlen(env_value);
      auto [ptr, ec] = std::from_chars(env_value, end, thread_count);
      if (!(ec == std::errc{} && ptr == end))
      {
        thread_count = 1;
        std::println(stderr, "Could not parse {}, using default value of {}",
                     AS_SERVER_THREADS, thread_count);
      }
    }
    return thallium::engine{address, THALLIUM_SERVER_MODE, false, thread_count};
  }
  else
  {
    return thallium::engine{address, THALLIUM_CLIENT_MODE};
  }
}

void run_server(Engine& engine) { engine.wait_for_finalize(); }

void stop_server(Engine& engine) { engine.finalize(); }

ServerEndpoint connect_to_server(Engine& engine,
                                 const std::string& server_address)
{
  return engine.lookup(server_address);
}

} // namespace as_rpc

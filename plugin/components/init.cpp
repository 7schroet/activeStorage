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

#include "init.hpp"
#include "log.hpp"
#include "rpc_handler.hpp"
#include <cstdlib>

herr_t H5VL_as_rpc_init([[maybe_unused]] hid_t vipl_id)
{
  log_msg("INIT");
  const std::filesystem::path operations_path{std::getenv("AS_RPC_OPERATIONS")};
  parse_as_rpc_config(operations_path);
  register_rpc_client();
  return 0;
}

herr_t H5VL_as_rpc_term()
{
  log_msg("TERM");

  return 0;
}

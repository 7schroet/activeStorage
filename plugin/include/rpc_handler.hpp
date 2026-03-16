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

#ifndef AS_RPC_HANDLER_HPP
#define AS_RPC_HANDLER_HPP

#include <filesystem>
#include <string_view>

void register_rpc_client();
void parse_as_rpc_config(const std::filesystem::path& path);
void register_single_operation(std::string_view filename,
                               std::string_view dset_name, unsigned timestep);
bool dset_is_in_ops(std::string_view filename, std::string_view dset_name);
void dispatch_operations(std::string_view filename, std::string_view dset_name);
void dispatch_operations(const std::string_view filename);

#endif

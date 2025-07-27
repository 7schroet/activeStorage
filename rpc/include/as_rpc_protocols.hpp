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

#ifndef AS_RPC_PROTOCOLS_HPP
#define AS_RPC_PROTOCOLS_HPP

#include <string>

namespace as_rpc
{
#define FOREACH_PROT(PROT)                                                     \
  PROT(tcp)                                                                    \
  PROT(verbs)

#define GENERATE_PROT_ENUM(ENUM) ENUM,

#define GENERATE_PROT_STRING(STRING) #STRING,

enum Protocol
{
  FOREACH_PROT(GENERATE_PROT_ENUM)
};

std::string protocol_to_string(Protocol protocol);

enum Protocol string_to_protocol(const std::string& protocol_string);

} // namespace as_rpc

#endif

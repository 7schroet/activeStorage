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

#ifndef RPC_H
#define RPC_H

typedef struct rpc_handle rpc_handle;

#define FOREACH_PROT(PROT)                                                     \
  PROT(tcp)                                                                    \
  PROT(verbs)

#define GENERATE_PROT_ENUM(ENUM) ENUM,

enum protocol
{
  FOREACH_PROT(GENERATE_PROT_ENUM)
};

/**
 * Requires an uninitialized rpc_handle. Must be destroyed with rpc_finalize
 * once finished. The address parameter can be constructed with
 * rpc_initialize_address.
 */
void rpc_init(rpc_handle**, char* address);

/**
 * The char array returned from this function must be freed by the caller.
 */
char* rpc_initialize_address(enum protocol protocol, char* port);

/**
 * Write the address to a file for communication among different RPC clients for
 * the setup.
 */
void rpc_address_to_file(rpc_handle*, char* filename);

void rpc_finalize(rpc_handle*);

#endif

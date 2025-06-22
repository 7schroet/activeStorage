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

#include "hg_common.h"
#include "rpc.h"
#include <stdio.h>
#include <stdlib.h>

void rpc_init(rpc_handle** handle)
{
  *handle = malloc(sizeof(rpc_handle));
  if (*handle == NULL)
  {
    fprintf(stderr, "Failed to allocate storage of size %ld, aborting!\n",
            sizeof(rpc_handle));
    exit(EXIT_FAILURE);
  }
  (*handle)->class = HG_Init("tcp://:12345", HG_TRUE);
  (*handle)->context = HG_Context_create((*handle)->class);
}

void rpc_address_to_file(rpc_handle* handle)
{
  hg_addr_t addr;
  HG_RETURN_CHECK(HG_Addr_self(handle->class, &addr));

  hg_size_t buf_size = 128;
  char buf[buf_size];
  HG_RETURN_CHECK(HG_Addr_to_string(handle->class, buf, &buf_size, addr));

  printf("Server address is: %s\n", buf);

  HG_RETURN_CHECK(HG_Addr_free(handle->class, addr));
}

void rpc_finalize(rpc_handle* handle)
{
  HG_RETURN_CHECK(HG_Context_destroy(handle->context));
  HG_RETURN_CHECK(HG_Finalize(handle->class));
  free(handle);
}

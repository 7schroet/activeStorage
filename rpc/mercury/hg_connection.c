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

#define _GNU_SOURCE

#include "file_writer.h"
#include "hg_common.h"
#include "hg_kernels.h"
#include "rpc.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if __STDC_VERSION__ <= 201710L
#include <stdbool.h>
#endif

#define GENERATE_PROT_STRING(STRING) #STRING,
static char* protocol_to_string(enum protocol protocol)
{
  static char* protocol_strings[] = {FOREACH_PROT(GENERATE_PROT_STRING)};
  return protocol_strings[protocol];
}

char* rpc_initialize_address(enum protocol protocol, char* port)
{
  assert(port);

  char* res = NULL;
  int ret = asprintf(&res, "%s://:%s", protocol_to_string(protocol), port);
  if (ret == -1)
  {
    fprintf(stderr, "asprintf failed, aborting!\n");
    exit(EXIT_FAILURE);
  }
  return res;
}

void rpc_init(rpc_handle** handle, char* address, bool accept_connection)
{
  assert(address);

  *handle = malloc(sizeof(rpc_handle));
  if (*handle == NULL)
  {
    fprintf(stderr, "Failed to allocate storage of size %ld, aborting!\n",
            sizeof(rpc_handle));
    exit(EXIT_FAILURE);
  }

  u_int8_t na_listen = accept_connection ? HG_TRUE : HG_FALSE;

  (*handle)->class = HG_Init(address, na_listen);
  (*handle)->context = HG_Context_create((*handle)->class);
}

void rpc_address_to_file(rpc_handle* handle, char* filename)
{
  assert(handle);
  assert(filename);

  hg_addr_t addr;
  HG_RETURN_CHECK(HG_Addr_self(handle->class, &addr));

  hg_size_t buf_size = 128;
  char buf[buf_size];
  HG_RETURN_CHECK(HG_Addr_to_string(handle->class, buf, &buf_size, addr));

  write_to_file(buf, filename);

  HG_RETURN_CHECK(HG_Addr_free(handle->class, addr));
}

void rpc_progress(rpc_handle* handle)
{
  unsigned int count;
  hg_return_t ret;

  do
  {
    ret = HG_Trigger(handle->context, 0, 1, &count);
  } while ((ret == HG_SUCCESS) && count);

  HG_Progress(handle->context, 100);
}

void rpc_finalize(rpc_handle* handle)
{
  assert(handle);

  HG_RETURN_CHECK(HG_Context_destroy(handle->context));
  HG_RETURN_CHECK(HG_Finalize(handle->class));
  free(handle);
}

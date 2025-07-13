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

#ifndef RPC_KERNELS_H
#define RPC_KERNELS_H

#include "hg_common.h"
#include <mercury.h>
#include <mercury_proc.h>
#include <mercury_proc_string.h>
#include <stdio.h>

typedef struct example_in
{
  hg_string_t str;
} example_in;

typedef struct example_out
{
  int32_t ret;
} example_out;

hg_return_t example_in_serialize(hg_proc_t proc, void* data)
{
  example_in* in = (example_in*)data;
  HG_RETURN_CHECK(hg_proc_hg_string_t(proc, &in->str));

  return HG_SUCCESS;
}

hg_return_t example_out_serialize(hg_proc_t proc, void* data)
{
  example_out* out = (example_out*)data;
  HG_RETURN_CHECK(hg_proc_int32_t(proc, &out->ret));

  return HG_SUCCESS;
}

void print_stuff(const char* const str)
{
  printf("RPC function was called with '%s'!\n", str);
}

hg_return_t callback(hg_handle_t handle)
{
  example_in in;
  HG_RETURN_CHECK(HG_Get_input(handle, &in));
  print_stuff(in.str);
  HG_RETURN_CHECK(HG_Free_input(handle, &in));
  HG_RETURN_CHECK(HG_Destroy(handle));

  return HG_SUCCESS;
}

// TODO return some sort of array that contains structs with name, id for
// lookups
unsigned long rpc_register_kernels(rpc_handle* handle)
{
  return HG_Register_name(handle->class, "printer", example_in_serialize,
                          example_out_serialize, callback);
}

#endif

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

#ifndef UTILS_HPP
#define UTILS_HPP

#include <hdf5.h>

// Used to wrap objects
struct H5VL_as_rpc_t
{
  hid_t under_vol_id; // ID for the underlying connector
  void* under_object; // Object of the underlying connector
};

// The wrap context
struct H5VL_as_rpc_ctx_t
{
  hid_t under_vol_id;   // VOL ID for under VOL
  void* under_wrap_ctx; // Object wrapping context for under VOL
};

#ifdef __cplusplus
extern "C"
{
#endif

  H5VL_as_rpc_t* H5VL_as_rpc_new_obj(void* under_obj, hid_t under_vol_id);
  herr_t H5VL_as_rpc_free_obj(H5VL_as_rpc_t* obj);

#ifdef __cplusplus
}
#endif

#endif

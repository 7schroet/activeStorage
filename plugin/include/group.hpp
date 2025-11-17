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

#ifndef GROUP_HPP
#define GROUP_HPP

#include <hdf5.h>

void* H5VL_as_rpc_group_create(void* obj, const H5VL_loc_params_t* loc_params,
                               const char* name, hid_t lcpl_id, hid_t gcpl_id,
                               hid_t gapl_id, hid_t dxpl_id, void** req);

void* H5VL_as_rpc_group_open(void* obj, const H5VL_loc_params_t* loc_params,
                             const char* name, hid_t gapl_id, hid_t dxpl_id,
                             void** req);

herr_t H5VL_as_rpc_group_get(void* obj, H5VL_group_get_args_t* args,
                             hid_t dxpl_id, void** req);

herr_t H5VL_as_rpc_group_specific(void* obj, H5VL_group_specific_args_t* args,
                                  hid_t dxpl_id, void** req);

herr_t H5VL_as_rpc_group_optional(void* obj, H5VL_optional_args_t* args,
                                  hid_t dxpl_id, void** req);

herr_t H5VL_as_rpc_group_close(void* obj, hid_t dxpl_id, void** req);

#endif

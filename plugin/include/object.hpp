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

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <hdf5.h>

void* H5VL_as_rpc_object_open(void* obj, const H5VL_loc_params_t* loc_params,
                              H5I_type_t* opened_type, hid_t dxpl_id,
                              void** req);
herr_t H5VL_as_rpc_object_copy(void* src_obj,
                               const H5VL_loc_params_t* src_loc_params,
                               const char* src_name, void* dst_obj,
                               const H5VL_loc_params_t* dst_loc_params,
                               const char* dst_name, hid_t ocpypl_id,
                               hid_t lcpl_id, hid_t dxpl_id, void** req);
herr_t H5VL_as_rpc_object_get(void* obj, const H5VL_loc_params_t* loc_params,
                              H5VL_object_get_args_t* args, hid_t dxpl_id,
                              void** req);
herr_t H5VL_as_rpc_object_specific(void* obj,
                                   const H5VL_loc_params_t* loc_params,
                                   H5VL_object_specific_args_t* args,
                                   hid_t dxpl_id, void** req);
herr_t H5VL_as_rpc_object_optional(void* obj,
                                   const H5VL_loc_params_t* loc_params,
                                   H5VL_optional_args_t* args, hid_t dxpl_id,
                                   void** req);

#endif

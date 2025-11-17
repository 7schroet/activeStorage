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

#ifndef LINK_HPP
#define LINK_HPP

#include <hdf5.h>

herr_t H5VL_as_rpc_link_create(H5VL_link_create_args_t* args, void* obj,
                               const H5VL_loc_params_t* loc_params,
                               hid_t lcpl_id, hid_t lapl_id, hid_t dxpl_id,
                               void** req);

herr_t
H5VL_as_rpc_link_copy(void* src_obj, const H5VL_loc_params_t* loc_params1,
                      void* dst_obj, const H5VL_loc_params_t* loc_params2,
                      hid_t lcpl_id, hid_t lapl_id, hid_t dxpl_id, void** req);

herr_t
H5VL_as_rpc_link_move(void* src_obj, const H5VL_loc_params_t* loc_params1,
                      void* dst_obj, const H5VL_loc_params_t* loc_params2,
                      hid_t lcpl_id, hid_t lapl_id, hid_t dxpl_id, void** req);

herr_t H5VL_as_rpc_link_get(void* obj, const H5VL_loc_params_t* loc_params,
                            H5VL_link_get_args_t* args, hid_t dxpl_id,
                            void** req);

herr_t H5VL_as_rpc_link_specific(void* obj, const H5VL_loc_params_t* loc_params,
                                 H5VL_link_specific_args_t* args, hid_t dxpl_id,
                                 void** req);

herr_t H5VL_as_rpc_link_optional(void* obj, const H5VL_loc_params_t* loc_params,
                                 H5VL_optional_args_t* args, hid_t dxpl_id,
                                 void** req);

#endif

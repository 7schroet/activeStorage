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

#ifndef FILE_HPP
#define FILE_HPP

#include <hdf5.h>

void* H5VL_as_rpc_file_create(const char* name, unsigned flags, hid_t fcpl_id,
                              hid_t fapl_id, hid_t dxpl_id, void** req);
void* H5VL_as_rpc_file_open(const char* name, unsigned flags, hid_t fapl_id,
                            hid_t dxpl_id, void** req);
herr_t H5VL_as_rpc_file_get(void* file, H5VL_file_get_args_t* args,
                            hid_t dxpl_id, void** req);
herr_t H5VL_as_rpc_file_specific(void* file, H5VL_file_specific_args_t* args,
                                 hid_t dxpl_id, void** req);
herr_t H5VL_as_rpc_file_optional(void* file, H5VL_optional_args_t* args,
                                 hid_t dxpl_id, void** req);
herr_t H5VL_as_rpc_file_close(void* file, hid_t dxpl_id, void** req);

#endif

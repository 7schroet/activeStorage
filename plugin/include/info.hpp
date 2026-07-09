/*
 * Copyright (c) 2025 - 2026 Niclas Schroeter
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

#ifndef INFO_HPP
#define INFO_HPP

#include <hdf5.h>

struct H5VL_as_rpc_info_t
{
  hid_t under_vol_id;            /* VOL ID for under VOL */
  void* under_vol_info{nullptr}; /* VOL info for under VOL */
};

void* H5VL_as_rpc_info_copy(const void* info);

herr_t H5VL_as_rpc_info_cmp(int* cmp_value, const void* info1,
                            const void* info2);

herr_t H5VL_as_rpc_info_free(void* info);

herr_t H5VL_as_rpc_info_to_str(const void* info, char** str);

herr_t H5VL_as_rpc_str_to_info(const char* str, void** info);

#endif

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

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <hdf5.h>

herr_t H5VL_as_rpc_request_wait(void* obj, uint64_t timeout,
                                H5VL_request_status_t* status);

herr_t H5VL_as_rpc_request_notify(void* obj, H5VL_request_notify_t cb,
                                  void* ctx);

herr_t H5VL_as_rpc_request_cancel(void* obj, H5VL_request_status_t* status);

herr_t H5VL_as_rpc_request_specific(void* obj,
                                    H5VL_request_specific_args_t* args);

herr_t H5VL_as_rpc_request_optional(void* obj, H5VL_optional_args_t* args);

herr_t H5VL_as_rpc_request_free(void* obj);

#endif

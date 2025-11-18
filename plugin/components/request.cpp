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

#include "request.hpp"
#include "log.hpp"
#include "utils.hpp"

herr_t H5VL_as_rpc_request_wait(void* obj, uint64_t timeout,
                                H5VL_request_status_t* status)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  log_msg("REQUEST Wait");
  return H5VLrequest_wait(o->under_object, o->under_vol_id, timeout, status);
}

herr_t H5VL_as_rpc_request_notify(void* obj, H5VL_request_notify_t cb,
                                  void* ctx)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  log_msg("REQUEST Notify");
  return H5VLrequest_notify(o->under_object, o->under_vol_id, cb, ctx);
}

herr_t H5VL_as_rpc_request_cancel(void* obj, H5VL_request_status_t* status)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  log_msg("REQUEST Cancel");
  return H5VLrequest_cancel(o->under_object, o->under_vol_id, status);
}

herr_t H5VL_as_rpc_request_specific(void* obj,
                                    H5VL_request_specific_args_t* args)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  log_msg("REQUEST Specific");
  return H5VLrequest_specific(o->under_object, o->under_vol_id, args);
}

herr_t H5VL_as_rpc_request_optional(void* obj, H5VL_optional_args_t* args)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  log_msg("REQUEST Optional");
  return H5VLrequest_optional(o->under_object, o->under_vol_id, args);
}

herr_t H5VL_as_rpc_request_free(void* obj)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  log_msg("REQUEST Free");
  const herr_t ret_value = H5VLrequest_free(o->under_object, o->under_vol_id);

  if (ret_value >= 0)
    H5VL_as_rpc_t_free_obj(o);

  return ret_value;
}

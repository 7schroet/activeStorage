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

#include "datatype.hpp"
#include "utils.hpp"

void* H5VL_as_rpc_dtype_commit(void* obj, const H5VL_loc_params_t* loc_params,
                               const char* name, hid_t type_id, hid_t lcpl_id,
                               hid_t tcpl_id, hid_t tapl_id, hid_t dxpl_id,
                               void** req)
{
  H5VL_as_rpc_t* dtype = nullptr;
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  void* under =
      H5VLdatatype_commit(o->under_object, loc_params, o->under_vol_id, name,
                          type_id, lcpl_id, tcpl_id, tapl_id, dxpl_id, req);

  if (under)
  {
    dtype = H5VL_as_rpc_new_obj(under, o->under_vol_id);

    if (req && *req)
      *req = H5VL_as_rpc_new_obj(under, o->under_vol_id);
  }
  return dtype;
}

void* H5VL_as_rpc_dtype_open(void* obj, const H5VL_loc_params_t* loc_params,
                             const char* name, hid_t tapl_id, hid_t dxpl_id,
                             void** req)
{
  H5VL_as_rpc_t* dtype = nullptr;
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  void* under = H5VLdatatype_open(o->under_object, loc_params, o->under_vol_id,
                                  name, tapl_id, dxpl_id, req);

  if (under)
  {
    dtype = H5VL_as_rpc_new_obj(under, o->under_vol_id);

    if (req && *req)
      *req = H5VL_as_rpc_new_obj(under, o->under_vol_id);
  }
  return dtype;
}

herr_t H5VL_as_rpc_dtype_get(void* obj, H5VL_datatype_get_args_t* args,
                             hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  const herr_t ret =
      H5VLdatatype_get(o->under_object, o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_new_obj(o->under_object, o->under_vol_id);

  return ret;
}

herr_t H5VL_as_rpc_dtype_specific(void* obj,
                                  H5VL_datatype_specific_args_t* args,
                                  hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  const hid_t under_vol_id = o->under_vol_id;
  const herr_t ret = H5VLdatatype_specific(o->under_object, o->under_vol_id,
                                           args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_new_obj(o->under_object, under_vol_id);

  return ret;
}

herr_t H5VL_as_rpc_dtype_optional(void* obj, H5VL_optional_args_t* args,
                                  hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  const herr_t ret = H5VLdatatype_optional(o->under_object, o->under_vol_id,
                                           args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_new_obj(o->under_object, o->under_vol_id);

  return ret;
}

herr_t H5VL_as_rpc_dtype_close(void* dt, hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(dt);
  const herr_t ret =
      H5VLdatatype_close(o->under_object, o->under_vol_id, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_new_obj(o->under_object, o->under_vol_id);

  if (ret >= 0)
    H5VL_as_rpc_free_obj(o);

  return ret;
}

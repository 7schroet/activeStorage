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

#include "attribute.hpp"
#include "utils.hpp"

void* H5VL_as_rpc_attr_create(void* obj, const H5VL_loc_params_t* loc_params,
                              const char* attr_name, hid_t type_id,
                              hid_t space_id, hid_t acpl_id, hid_t aapl_id,
                              hid_t dxpl_id, void** req)
{
  H5VL_as_rpc_t* attr = nullptr;
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  void* under;

  under =
      H5VLattr_create(o->under_object, loc_params, o->under_vol_id, attr_name,
                      type_id, space_id, acpl_id, aapl_id, dxpl_id, req);
  if (under)
  {
    attr = H5VL_as_rpc_new_obj(under, o->under_vol_id);

    // check for async
    if (req && *req)
      *req = H5VL_as_rpc_new_obj(*req, o->under_vol_id);
  }

  return attr;
}

void* H5VL_as_rpc_attr_open(void* obj, const H5VL_loc_params_t* loc_params,
                            const char* attr_name, hid_t aapl_id, hid_t dxpl_id,
                            void** req)
{
  H5VL_as_rpc_t* attr = nullptr;
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  void* under;

  under = H5VLattr_open(o->under_object, loc_params, o->under_vol_id, attr_name,
                        aapl_id, dxpl_id, req);
  if (under)
  {
    attr = H5VL_as_rpc_new_obj(under, o->under_vol_id);

    // check for async
    if (req && *req)
      *req = H5VL_as_rpc_new_obj(*req, o->under_vol_id);
  }

  return attr;
}

herr_t H5VL_as_rpc_read(void* attr, hid_t mem_type_id, void* buf, hid_t dxpl_id,
                        void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(attr);
  const herr_t ret = H5VLattr_read(o->under_object, o->under_vol_id,
                                   mem_type_id, buf, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_new_obj(*req, o->under_vol_id);

  return ret;
}

herr_t H5VL_as_rpc_write(void* attr, hid_t mem_type_id, const void* buf,
                         hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(attr);
  const herr_t ret = H5VLattr_write(o->under_object, o->under_vol_id,
                                    mem_type_id, buf, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_new_obj(*req, o->under_vol_id);

  return ret;
}
herr_t H5VL_as_rpc_get(void* obj, H5VL_attr_get_args_t* args, hid_t dxpl_id,
                       void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  const herr_t ret =
      H5VLattr_get(o->under_object, o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_new_obj(*req, o->under_vol_id);

  return ret;
}
herr_t H5VL_as_rpc_specific(void* obj, const H5VL_loc_params_t* loc_params,
                            H5VL_attr_specific_args_t* args, hid_t dxpl_id,
                            void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  const herr_t ret = H5VLattr_specific(o->under_object, loc_params,
                                       o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_new_obj(*req, o->under_vol_id);

  return ret;
}
herr_t H5VL_as_rpc_optional(void* obj, H5VL_optional_args_t* args,
                            hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  const herr_t ret =
      H5VLattr_optional(o->under_object, o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_new_obj(*req, o->under_vol_id);

  return ret;
}
herr_t H5VL_as_rpc_close(void* attr, hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(attr);
  const herr_t ret =
      H5VLattr_close(o->under_object, o->under_vol_id, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_new_obj(*req, o->under_vol_id);

  if (ret >= 0)
    H5VL_as_rpc_free_obj(o);

  return ret;
}

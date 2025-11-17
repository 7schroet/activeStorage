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

#include "object.hpp"
#include "log.hpp"
#include "utils.hpp"

void* H5VL_as_rpc_object_open(void* obj, const H5VL_loc_params_t* loc_params,
                              H5I_type_t* opened_type, hid_t dxpl_id,
                              void** req)
{
  H5VL_as_rpc_t* new_obj = nullptr;
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("OBJECT Open");

  void* under = H5VLobject_open(o->under_object, loc_params, o->under_vol_id,
                                opened_type, dxpl_id, req);
  if (under)
  {
    new_obj = H5VL_as_rpc_t_new_obj(under, o->under_vol_id);

    if (req && *req)
      *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);
  }

  return new_obj;
}

herr_t H5VL_as_rpc_object_copy(void* src_obj,
                               const H5VL_loc_params_t* src_loc_params,
                               const char* src_name, void* dst_obj,
                               const H5VL_loc_params_t* dst_loc_params,
                               const char* dst_name, hid_t ocpypl_id,
                               hid_t lcpl_id, hid_t dxpl_id, void** req)
{
  auto o_src = static_cast<H5VL_as_rpc_t*>(src_obj);
  auto o_dst = static_cast<H5VL_as_rpc_t*>(dst_obj);

  log_msg("OBJECT Copy");

  herr_t ret_value =
      H5VLobject_copy(o_src->under_object, src_loc_params, src_name,
                      o_dst->under_object, dst_loc_params, dst_name,
                      o_src->under_vol_id, ocpypl_id, lcpl_id, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o_src->under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_object_get(void* obj, const H5VL_loc_params_t* loc_params,
                              H5VL_object_get_args_t* args, hid_t dxpl_id,
                              void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("OBJECT Get");

  herr_t ret_value = H5VLobject_get(o->under_object, loc_params,
                                    o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_object_specific(void* obj,
                                   const H5VL_loc_params_t* loc_params,
                                   H5VL_object_specific_args_t* args,
                                   hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("OBJECT Specific");

  // Save copy of underlying VOL connector ID and prov helper, in case of
  // refresh destroying the current object
  hid_t under_vol_id = o->under_vol_id;

  herr_t ret_value = H5VLobject_specific(o->under_object, loc_params,
                                         o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_object_optional(void* obj,
                                   const H5VL_loc_params_t* loc_params,
                                   H5VL_optional_args_t* args, hid_t dxpl_id,
                                   void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("OBJECT Optional");

  herr_t ret_value = H5VLobject_optional(o->under_object, loc_params,
                                         o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  return ret_value;
}

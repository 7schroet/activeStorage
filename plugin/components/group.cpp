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

#include "group.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <cstring>

void* H5VL_as_rpc_group_create(void* obj, const H5VL_loc_params_t* loc_params,
                               const char* name, hid_t lcpl_id, hid_t gcpl_id,
                               hid_t gapl_id, hid_t dxpl_id, void** req)
{
  H5VL_as_rpc_t* group = nullptr;
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("GROUP Create");

  void* under = H5VLgroup_create(o->under_object, loc_params, o->under_vol_id,
                                 name, lcpl_id, gcpl_id, gapl_id, dxpl_id, req);
  if (under)
  {
    group = H5VL_as_rpc_t_new_obj(under, o->under_vol_id);

    if (req && *req)
      *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);
  }

  return group;
}

void* H5VL_as_rpc_group_open(void* obj, const H5VL_loc_params_t* loc_params,
                             const char* name, hid_t gapl_id, hid_t dxpl_id,
                             void** req)
{
  H5VL_as_rpc_t* group = nullptr;
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("GROUP Open");

  void* under = H5VLgroup_open(o->under_object, loc_params, o->under_vol_id,
                               name, gapl_id, dxpl_id, req);
  if (under)
  {
    group = H5VL_as_rpc_t_new_obj(under, o->under_vol_id);

    if (req && *req)
      *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);
  }

  return group;
}

herr_t H5VL_as_rpc_group_get(void* obj, H5VL_group_get_args_t* args,
                             hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("GROUP Get");

  const herr_t ret_value =
      H5VLgroup_get(o->under_object, o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_group_specific(void* obj, H5VL_group_specific_args_t* args,
                                  hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  H5VL_group_specific_args_t my_args;
  H5VL_group_specific_args_t* new_args;
  herr_t ret_value;

  log_msg("GROUP Specific");

  // Save copy of underlying VOL connector ID and prov helper, in case of
  // refresh destroying the current object
  const hid_t under_vol_id = o->under_vol_id;

  // Unpack arguments to get at the child file pointer when mounting a file
  if (args->op_type == H5VL_GROUP_MOUNT)
  {
    memcpy(&my_args, args, sizeof(my_args));

    // Set the object for the child file
    my_args.args.mount.child_file =
        static_cast<H5VL_as_rpc_t*>(args->args.mount.child_file)->under_object;

    new_args = &my_args;
  }
  else
    new_args = args;

  ret_value =
      H5VLgroup_specific(o->under_object, under_vol_id, new_args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_group_optional(void* obj, H5VL_optional_args_t* args,
                                  hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("GROUP Optional");

  const herr_t ret_value =
      H5VLgroup_optional(o->under_object, o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_group_close(void* obj, hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("GROUP Close");

  const herr_t ret_value =
      H5VLgroup_close(o->under_object, o->under_vol_id, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  if (ret_value >= 0)
    H5VL_as_rpc_t_free_obj(o);

  return ret_value;
}

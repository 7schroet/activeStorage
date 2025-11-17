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

#include "link.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <cstring>

herr_t H5VL_as_rpc_link_create(H5VL_link_create_args_t* args, void* obj,
                               const H5VL_loc_params_t* loc_params,
                               hid_t lcpl_id, hid_t lapl_id, hid_t dxpl_id,
                               void** req)
{
  H5VL_link_create_args_t my_args;
  H5VL_link_create_args_t* new_args;
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  hid_t under_vol_id = -1;

  log_msg("LINK Create\n");

  // Try to retrieve the "under" VOL id
  if (o)
    under_vol_id = o->under_vol_id;

  // Fix up the link target object for hard link creation
  if (H5VL_LINK_CREATE_HARD == args->op_type)
  {
    // If it's a non-NULL pointer, find the 'under object' and reset the args
    if (args->args.hard.curr_obj)
    {
      memcpy(&my_args, args, sizeof(my_args));

      // Check if we still need the "under" VOL ID
      if (under_vol_id < 0)
        under_vol_id =
            static_cast<H5VL_as_rpc_t*>(args->args.hard.curr_obj)->under_vol_id;

      // Set the object for the link target
      my_args.args.hard.curr_obj =
          static_cast<H5VL_as_rpc_t*>(args->args.hard.curr_obj)->under_object;

      // Set argument pointer to modified parameters
      new_args = &my_args;
    }
    else
      new_args = args;
  }
  else
    new_args = args;

  // Re-issue 'link create' call, possibly using the unwrapped pieces
  herr_t ret_value =
      H5VLlink_create(new_args, (o ? o->under_object : nullptr), loc_params,
                      under_vol_id, lcpl_id, lapl_id, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, under_vol_id);

  return ret_value;
}

herr_t
H5VL_as_rpc_link_copy(void* src_obj, const H5VL_loc_params_t* loc_params1,
                      void* dst_obj, const H5VL_loc_params_t* loc_params2,
                      hid_t lcpl_id, hid_t lapl_id, hid_t dxpl_id, void** req)
{
  auto o_src = static_cast<H5VL_as_rpc_t*>(src_obj);
  auto o_dst = static_cast<H5VL_as_rpc_t*>(dst_obj);
  hid_t under_vol_id = -1;

  log_msg("LINK Copy\n");

  if (o_src)
    under_vol_id = o_src->under_vol_id;
  else if (o_dst)
    under_vol_id = o_dst->under_vol_id;

  herr_t ret_value =
      H5VLlink_copy((o_src ? o_src->under_object : nullptr), loc_params1,
                    (o_dst ? o_dst->under_object : nullptr), loc_params2,
                    under_vol_id, lcpl_id, lapl_id, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, under_vol_id);

  return ret_value;
}

herr_t
H5VL_as_rpc_link_move(void* src_obj, const H5VL_loc_params_t* loc_params1,
                      void* dst_obj, const H5VL_loc_params_t* loc_params2,
                      hid_t lcpl_id, hid_t lapl_id, hid_t dxpl_id, void** req)
{
  auto o_src = static_cast<H5VL_as_rpc_t*>(src_obj);
  auto o_dst = static_cast<H5VL_as_rpc_t*>(dst_obj);
  hid_t under_vol_id = -1;

  log_msg("LINK Move\n");

  // Retrieve the "under" VOL id
  if (o_src)
    under_vol_id = o_src->under_vol_id;
  else if (o_dst)
    under_vol_id = o_dst->under_vol_id;

  herr_t ret_value =
      H5VLlink_move((o_src ? o_src->under_object : nullptr), loc_params1,
                    (o_dst ? o_dst->under_object : nullptr), loc_params2,
                    under_vol_id, lcpl_id, lapl_id, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_link_get(void* obj, const H5VL_loc_params_t* loc_params,
                            H5VL_link_get_args_t* args, hid_t dxpl_id,
                            void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("LINK Get\n");

  herr_t ret_value = H5VLlink_get(o->under_object, loc_params, o->under_vol_id,
                                  args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_link_specific(void* obj, const H5VL_loc_params_t* loc_params,
                                 H5VL_link_specific_args_t* args, hid_t dxpl_id,
                                 void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("LINK Specific\n");

  herr_t ret_value = H5VLlink_specific(o->under_object, loc_params,
                                       o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_link_optional(void* obj, const H5VL_loc_params_t* loc_params,
                                 H5VL_optional_args_t* args, hid_t dxpl_id,
                                 void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("LINK Optional\n");

  herr_t ret_value = H5VLlink_optional(o->under_object, loc_params,
                                       o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  return ret_value;
}

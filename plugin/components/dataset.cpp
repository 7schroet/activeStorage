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

#include "dataset.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <vector>

void* H5VL_as_rpc_dataset_create(void* obj, const H5VL_loc_params_t* loc_params,
                                 const char* name, hid_t lcpl_id, hid_t type_id,
                                 hid_t space_id, hid_t dcpl_id, hid_t dapl_id,
                                 hid_t dxpl_id, void** req)
{
  H5VL_as_rpc_t* dset = nullptr;
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("DATASET Create");

  void* under = H5VLdataset_create(o->under_object, loc_params, o->under_vol_id,
                                   name, lcpl_id, type_id, space_id, dcpl_id,
                                   dapl_id, dxpl_id, req);
  if (under)
  {
    dset = H5VL_as_rpc_t_new_obj(under, o->under_vol_id);

    if (req && *req)
      *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);
  }

  return dset;
}

void* H5VL_as_rpc_dataset_open(void* obj, const H5VL_loc_params_t* loc_params,
                               const char* name, hid_t dapl_id, hid_t dxpl_id,
                               void** req)
{
  H5VL_as_rpc_t* dset = nullptr;
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("DATASET Open");

  void* under = H5VLdataset_open(o->under_object, loc_params, o->under_vol_id,
                                 name, dapl_id, dxpl_id, req);
  if (under)
  {
    dset = H5VL_as_rpc_t_new_obj(under, o->under_vol_id);

    if (req && *req)
      *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);
  }

  return dset;
}

herr_t H5VL_as_rpc_dataset_read(size_t count, void* dset[], hid_t mem_type_id[],
                                hid_t mem_space_id[], hid_t file_space_id[],
                                hid_t plist_id, void* buf[], void** req)
{
  // Array of under objects
  std::vector<void*> o_arr(count);

  log_msg("DATASET Read");

  // VOL ID for all objects
  const hid_t under_vol_id =
      (static_cast<H5VL_as_rpc_t*>(dset[0]))->under_vol_id;
  for (size_t u = 0; u < count; u++)
    o_arr[u] = (static_cast<H5VL_as_rpc_t*>(dset[u]))->under_object;

  const herr_t ret_value =
      H5VLdataset_read(count, o_arr.data(), under_vol_id, mem_type_id,
                       mem_space_id, file_space_id, plist_id, buf, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_dataset_write(size_t count, void* dset[],
                                 hid_t mem_type_id[], hid_t mem_space_id[],
                                 hid_t file_space_id[], hid_t plist_id,
                                 const void* buf[], void** req)
{
  std::vector<void*> o_arr(count);

  log_msg("DATASET Write");

  const hid_t under_vol_id =
      (static_cast<H5VL_as_rpc_t*>(dset[0]))->under_vol_id;
  for (size_t u = 0; u < count; u++)
    o_arr[u] = (static_cast<H5VL_as_rpc_t*>(dset[u]))->under_object;

  const herr_t ret_value =
      H5VLdataset_write(count, o_arr.data(), under_vol_id, mem_type_id,
                        mem_space_id, file_space_id, plist_id, buf, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_dataset_get(void* obj, H5VL_dataset_get_args_t* args,
                               hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("DATASET Get");

  const herr_t ret_value =
      H5VLdataset_get(o->under_object, o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_dataset_specific(void* obj,
                                    H5VL_dataset_specific_args_t* args,
                                    hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("H5Dspecific");

  // Save copy of underlying VOL connector ID and prov helper, in case of
  // refresh destroying the current object
  const hid_t under_vol_id = o->under_vol_id;

  const herr_t ret_value = H5VLdataset_specific(
      o->under_object, o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_dataset_optional(void* obj, H5VL_optional_args_t* args,
                                    hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("DATASET Optional");

  const herr_t ret_value = H5VLdataset_optional(
      o->under_object, o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_dataset_close(void* obj, hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("DATASET Close");

  const herr_t ret_value =
      H5VLdataset_close(o->under_object, o->under_vol_id, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  if (ret_value >= 0)
    H5VL_as_rpc_t_free_obj(o);

  return ret_value;
}

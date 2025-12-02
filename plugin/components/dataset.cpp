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
#include "file.hpp"
#include "log.hpp"
#include "rpc_handler.hpp"
#include <vector>

H5VL_as_rpc_dset_t*
H5VL_as_rpc_dset_t_new_obj(void* under_obj, hid_t under_vol_id,
                           const std::filesystem::path& filename,
                           const std::string& dsetname)
{
  log_msg("DSET STRUCT creation");
  auto new_obj = new H5VL_as_rpc_dset_t();
  new_obj->under_object = under_obj;
  new_obj->under_vol_id = under_vol_id;
  new_obj->filename = filename;
  new_obj->dsetname = dsetname;
  H5Iinc_ref(new_obj->under_vol_id);
  return new_obj;
}

herr_t H5VL_as_rpc_dset_t_free_obj(H5VL_as_rpc_dset_t* obj)
{
  log_msg("DSET STRUCT delete");
  const hid_t err_id = H5Eget_current_stack();
  H5Idec_ref(obj->under_vol_id);
  H5Eset_current_stack(err_id);
  delete obj;
  return 0;
}

void* H5VL_as_rpc_dataset_create(void* obj, const H5VL_loc_params_t* loc_params,
                                 const char* name, hid_t lcpl_id, hid_t type_id,
                                 hid_t space_id, hid_t dcpl_id, hid_t dapl_id,
                                 hid_t dxpl_id, void** req)
{
  H5VL_as_rpc_dset_t* dset = nullptr;
  auto o = static_cast<H5VL_as_rpc_file_t*>(obj);
  log_msg("DATASET Create");

  void* under = H5VLdataset_create(o->under_object, loc_params, o->under_vol_id,
                                   name, lcpl_id, type_id, space_id, dcpl_id,
                                   dapl_id, dxpl_id, req);
  if (under)
  {
    dset = H5VL_as_rpc_dset_t_new_obj(
        under, o->under_vol_id, std::filesystem::absolute(o->filename), name);

    if (req && *req)
      *req = H5VL_as_rpc_dset_t_new_obj(
          *req, o->under_vol_id, std::filesystem::absolute(o->filename), name);
  }

  return dset;
}

void* H5VL_as_rpc_dataset_open(void* obj, const H5VL_loc_params_t* loc_params,
                               const char* name, hid_t dapl_id, hid_t dxpl_id,
                               void** req)
{
  H5VL_as_rpc_dset_t* dset = nullptr;
  auto o = static_cast<H5VL_as_rpc_file_t*>(obj);

  log_msg("DATASET Open");

  void* under = H5VLdataset_open(o->under_object, loc_params, o->under_vol_id,
                                 name, dapl_id, dxpl_id, req);
  if (under)
  {
    dset = H5VL_as_rpc_dset_t_new_obj(
        under, o->under_vol_id, std::filesystem::absolute(o->filename), name);

    if (req && *req)
      *req = H5VL_as_rpc_dset_t_new_obj(
          *req, o->under_vol_id, std::filesystem::absolute(o->filename), name);
  }

  return dset;
}

herr_t H5VL_as_rpc_dataset_read(size_t count, void* dset[], hid_t mem_type_id[],
                                hid_t mem_space_id[], hid_t file_space_id[],
                                hid_t plist_id, void* buf[], void** req)
{
  log_msg("DATASET Read");

  // VOL ID for all objects
  const hid_t under_vol_id =
      (static_cast<H5VL_as_rpc_dset_t*>(dset[0]))->under_vol_id;

  for (decltype(count) i = 0; i < count; i++)
  {
    auto o = static_cast<H5VL_as_rpc_dset_t*>(dset[i]);
    const herr_t ret_value = H5VLdataset_read(
        1, &(o->under_object), under_vol_id, &(mem_type_id[i]),
        &(mem_space_id[i]), &(file_space_id[i]), plist_id, &(buf[i]), req);

    if (ret_value != 0)
      return ret_value;

    if (req && *req)
      *req = H5VL_as_rpc_dset_t_new_obj(*req, under_vol_id, o->filename,
                                        o->dsetname);
  }
  return 0;
}

herr_t H5VL_as_rpc_dataset_write(size_t count, void* dset[],
                                 hid_t mem_type_id[], hid_t mem_space_id[],
                                 hid_t file_space_id[], hid_t plist_id,
                                 const void* buf[], void** req)
{
  log_msg("DATASET Write");

  const hid_t under_vol_id =
      (static_cast<H5VL_as_rpc_dset_t*>(dset[0]))->under_vol_id;

  for (decltype(count) i = 0; i < count; i++)
  {
    auto o = static_cast<H5VL_as_rpc_dset_t*>(dset[i]);

    const herr_t ret_value = H5VLdataset_write(
        1, &(o->under_object), under_vol_id, &(mem_type_id[i]),
        &(mem_space_id[i]), &(file_space_id[i]), plist_id, &(buf[i]), req);

    if (ret_value != 0)
      return ret_value;

    auto rank = H5Sget_simple_extent_ndims(file_space_id[i]);
    std::vector<hsize_t> start(rank);
    H5Sget_regular_hyperslab(file_space_id[i], start.data(), nullptr, nullptr,
                             nullptr);

    const std::vector<char> reduction{1, 1, 1};
    register_operation(as_rpc::Kernel::mean, o->filename, o->dsetname, start[0],
                       reduction);

    if (req && *req)
      *req = H5VL_as_rpc_dset_t_new_obj(*req, under_vol_id, o->filename,
                                        o->dsetname);
  }

  return 0;
}

herr_t H5VL_as_rpc_dataset_get(void* obj, H5VL_dataset_get_args_t* args,
                               hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_dset_t*>(obj);

  log_msg("DATASET Get");

  const herr_t ret_value =
      H5VLdataset_get(o->under_object, o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_dset_t_new_obj(*req, o->under_vol_id, o->filename,
                                      o->dsetname);

  return ret_value;
}

herr_t H5VL_as_rpc_dataset_specific(void* obj,
                                    H5VL_dataset_specific_args_t* args,
                                    hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_dset_t*>(obj);

  log_msg("H5Dspecific");

  // Save copy of underlying VOL connector ID and prov helper, in case of
  // refresh destroying the current object
  const hid_t under_vol_id = o->under_vol_id;

  const herr_t ret_value = H5VLdataset_specific(
      o->under_object, o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_dset_t_new_obj(*req, under_vol_id, o->filename,
                                      o->dsetname);

  if (args->op_type == H5VL_DATASET_FLUSH)
    dispatch_operation(o->filename, o->dsetname);

  return ret_value;
}

herr_t H5VL_as_rpc_dataset_optional(void* obj, H5VL_optional_args_t* args,
                                    hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_dset_t*>(obj);

  log_msg("DATASET Optional");

  const herr_t ret_value = H5VLdataset_optional(
      o->under_object, o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_dset_t_new_obj(*req, o->under_vol_id, o->filename,
                                      o->dsetname);

  return ret_value;
}

herr_t H5VL_as_rpc_dataset_close(void* obj, hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_dset_t*>(obj);

  log_msg("DATASET Close");

  const herr_t ret_value =
      H5VLdataset_close(o->under_object, o->under_vol_id, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_dset_t_new_obj(*req, o->under_vol_id, o->filename,
                                      o->dsetname);

  if (ret_value >= 0)
    H5VL_as_rpc_dset_t_free_obj(o);

  return ret_value;
}

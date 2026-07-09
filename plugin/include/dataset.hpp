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

#ifndef DATASET_HPP
#define DATASET_HPP

#include <hdf5.h>
#include <string>

/**
 * Datasets get a special struct that also logs the name of
 * the file and the corresponding dataset.
 */
struct H5VL_as_rpc_dset_t
{
  hid_t under_vol_id;
  void* under_object{nullptr};
  std::string filename;
  std::string dsetname;
};
H5VL_as_rpc_dset_t* H5VL_as_rpc_dset_t_new_obj(void* under_obj,
                                               hid_t under_vol_id,
                                               const std::string& filename,
                                               const std::string& dsetname);
herr_t H5VL_as_rpc_dset_t_free_obj(H5VL_as_rpc_dset_t* obj);

void* H5VL_as_rpc_dataset_create(void* obj, const H5VL_loc_params_t* loc_params,
                                 const char* name, hid_t lcpl_id, hid_t type_id,
                                 hid_t space_id, hid_t dcpl_id, hid_t dapl_id,
                                 hid_t dxpl_id, void** req);

void* H5VL_as_rpc_dataset_open(void* obj, const H5VL_loc_params_t* loc_params,
                               const char* name, hid_t dapl_id, hid_t dxpl_id,
                               void** req);

herr_t H5VL_as_rpc_dataset_read(size_t count, void* dset[], hid_t mem_type_id[],
                                hid_t mem_space_id[], hid_t file_space_id[],
                                hid_t plist_id, void* buf[], void** req);

herr_t H5VL_as_rpc_dataset_write(size_t count, void* dset[],
                                 hid_t mem_type_id[], hid_t mem_space_id[],
                                 hid_t file_space_id[], hid_t plist_id,
                                 const void* buf[], void** req);

herr_t H5VL_as_rpc_dataset_get(void* obj, H5VL_dataset_get_args_t* args,
                               hid_t dxpl_id, void** req);

herr_t H5VL_as_rpc_dataset_specific(void* obj,
                                    H5VL_dataset_specific_args_t* args,
                                    hid_t dxpl_id, void** req);

herr_t H5VL_as_rpc_dataset_optional(void* obj, H5VL_optional_args_t* args,
                                    hid_t dxpl_id, void** req);

herr_t H5VL_as_rpc_dataset_close(void* obj, hid_t dxpl_id, void** req);

#endif

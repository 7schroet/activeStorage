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
#include "utils.hpp"
#include <cassert>

void* H5VL_as_rpc_dataset_create(void* obj, const H5VL_loc_params_t* loc_params,
                                 const char* name, hid_t lcpl_id, hid_t type_id,
                                 hid_t space_id, hid_t dcpl_id, hid_t dapl_id,
                                 hid_t dxpl_id, void** req)
{
  H5VL_as_rpc_t* dset;
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  void* under;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL DATASET Create\n");
#endif

  under = H5VLdataset_create(o->under_object, loc_params, o->under_vol_id, name,
                             lcpl_id, type_id, space_id, dcpl_id, dapl_id,
                             dxpl_id, req);
  if (under)
  {
    dset = H5VL_as_rpc_t_new_obj(under, o->under_vol_id);

    if (req && *req)
      *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);
  }
  else
    dset = NULL;

  return (void*)dset;
}

void* H5VL_as_rpc_dataset_open(void* obj, const H5VL_loc_params_t* loc_params,
                               const char* name, hid_t dapl_id, hid_t dxpl_id,
                               void** req)
{
  H5VL_as_rpc_t* dset;
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  void* under;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL DATASET Open\n");
#endif

  under = H5VLdataset_open(o->under_object, loc_params, o->under_vol_id, name,
                           dapl_id, dxpl_id, req);
  if (under)
  {
    dset = H5VL_as_rpc_t_new_obj(under, o->under_vol_id);

    if (req && *req)
      *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);
  }
  else
    dset = NULL;

  return (void*)dset;
}

/*-------------------------------------------------------------------------
 * Function:    H5VL_pass_through_ext_dataset_read
 *
 * Purpose:     Reads data elements from a dataset into a buffer.
 *
 * Return:      Success:    0
 *              Failure:    -1
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_as_rpc_dataset_read(size_t count, void* dset[], hid_t mem_type_id[],
                                hid_t mem_space_id[], hid_t file_space_id[],
                                hid_t plist_id, void* buf[], void** req)
{
  void* o_arr[count]; /* Array of under objects */
  hid_t under_vol_id; /* VOL ID for all objects */
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL DATASET Read\n");
#endif

  under_vol_id = ((H5VL_as_rpc_t*)(dset[0]))->under_vol_id;
  for (size_t u = 0; u < count; u++)
  {
    o_arr[u] = ((H5VL_as_rpc_t*)(dset[u]))->under_object;
    assert(under_vol_id == ((H5VL_as_rpc_t*)(dset[u]))->under_vol_id);
  }

  ret_value = H5VLdataset_read(count, o_arr, under_vol_id, mem_type_id,
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
  void* o_arr[count];
  hid_t under_vol_id;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL DATASET Write\n");
#endif

  under_vol_id = ((H5VL_as_rpc_t*)(dset[0]))->under_vol_id;
  for (size_t u = 0; u < count; u++)
  {
    o_arr[u] = ((H5VL_as_rpc_t*)(dset[u]))->under_object;
    assert(under_vol_id == ((H5VL_as_rpc_t*)(dset[u]))->under_vol_id);
  }

  ret_value =
      H5VLdataset_write(count, o_arr, under_vol_id, mem_type_id, mem_space_id,
                        file_space_id, plist_id, buf, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_dataset_get(void* dset, H5VL_dataset_get_args_t* args,
                               hid_t dxpl_id, void** req)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)dset;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL DATASET Get\n");
#endif

  ret_value =
      H5VLdataset_get(o->under_object, o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_dataset_specific(void* obj,
                                    H5VL_dataset_specific_args_t* args,
                                    hid_t dxpl_id, void** req)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  hid_t under_vol_id;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL H5Dspecific\n");
#endif

  // Save copy of underlying VOL connector ID and prov helper, in case of
  // refresh destroying the current object
  under_vol_id = o->under_vol_id;

  ret_value = H5VLdataset_specific(o->under_object, o->under_vol_id, args,
                                   dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_dataset_optional(void* obj, H5VL_optional_args_t* args,
                                    hid_t dxpl_id, void** req)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL DATASET Optional\n");
#endif

  ret_value = H5VLdataset_optional(o->under_object, o->under_vol_id, args,
                                   dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_dataset_close(void* dset, hid_t dxpl_id, void** req)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)dset;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL DATASET Close\n");
#endif

  ret_value = H5VLdataset_close(o->under_object, o->under_vol_id, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  if (ret_value >= 0)
    H5VL_as_rpc_t_free_obj(o);

  return ret_value;
}

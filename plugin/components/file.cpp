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

#include "file.hpp"
#include "info.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <cstring>

void* H5VL_as_rpc_file_create(const char* name, unsigned flags, hid_t fcpl_id,
                              hid_t fapl_id, hid_t dxpl_id, void** req)
{
  void* info = nullptr;
  H5VL_as_rpc_t* file = nullptr;
  hid_t under_fapl_id;

  log_msg("FILE Create");

  /* Get copy of our VOL info from FAPL */
  H5Pget_vol_info(fapl_id, &info);

  if (!info)
    return nullptr;

  auto info_cast = static_cast<H5VL_as_rpc_info_t*>(info);
  under_fapl_id = H5Pcopy(fapl_id);

  /* Set the VOL ID and info for the underlying FAPL */
  H5Pset_vol(under_fapl_id, info_cast->under_vol_id, info_cast->under_vol_info);

  /* Open the file with the underlying VOL connector */
  void* under =
      H5VLfile_create(name, flags, fcpl_id, under_fapl_id, dxpl_id, req);
  if (under)
  {
    file = H5VL_as_rpc_t_new_obj(under, info_cast->under_vol_id);

    if (req && *req)
      *req = H5VL_as_rpc_t_new_obj(*req, info_cast->under_vol_id);
  }

  H5Pclose(under_fapl_id);

  H5VL_as_rpc_info_free(info);

  return file;
}

void* H5VL_as_rpc_file_open(const char* name, unsigned flags, hid_t fapl_id,
                            hid_t dxpl_id, void** req)
{
  void* info = nullptr;
  H5VL_as_rpc_t* file = nullptr;
  hid_t under_fapl_id;

  log_msg("FILE Open");

  H5Pget_vol_info(fapl_id, &info);

  if (!info)
    return nullptr;

  auto info_cast = static_cast<H5VL_as_rpc_info_t*>(info);

  under_fapl_id = H5Pcopy(fapl_id);

  H5Pset_vol(under_fapl_id, info_cast->under_vol_id, info_cast->under_vol_info);

  void* under = H5VLfile_open(name, flags, under_fapl_id, dxpl_id, req);
  if (under)
  {
    file = H5VL_as_rpc_t_new_obj(under, info_cast->under_vol_id);

    if (req && *req)
      *req = H5VL_as_rpc_t_new_obj(*req, info_cast->under_vol_id);
  }

  H5Pclose(under_fapl_id);

  H5VL_as_rpc_info_free(info);

  return file;
}

herr_t H5VL_as_rpc_file_get(void* obj, H5VL_file_get_args_t* args,
                            hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("FILE Get");

  herr_t ret_value =
      H5VLfile_get(o->under_object, o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_file_specific(void* obj, H5VL_file_specific_args_t* args,
                                 hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  H5VL_as_rpc_t* new_o = nullptr;
  H5VL_file_specific_args_t my_args;
  H5VL_file_specific_args_t* new_args;
  H5VL_as_rpc_info_t* info;
  hid_t under_vol_id = -1;

  log_msg("FILE Specific");

  if (args->op_type == H5VL_FILE_IS_ACCESSIBLE)
  {
    memcpy(&my_args, args, sizeof(my_args));

    // Set up the new FAPL for the updated arguments
    H5Pget_vol_info(args->args.is_accessible.fapl_id, (void**)&info);

    if (!info)
      return (-1);

    // Keep the correct underlying VOL ID for later
    under_vol_id = info->under_vol_id;

    my_args.args.is_accessible.fapl_id =
        H5Pcopy(args->args.is_accessible.fapl_id);

    // Set the VOL ID and info for the underlying FAPL
    H5Pset_vol(my_args.args.is_accessible.fapl_id, info->under_vol_id,
               info->under_vol_info);

    new_args = &my_args;
  }
  else if (args->op_type == H5VL_FILE_DELETE)
  {
    memcpy(&my_args, args, sizeof(my_args));

    H5Pget_vol_info(args->args.del.fapl_id, (void**)&info);

    if (!info)
      return (-1);

    under_vol_id = info->under_vol_id;

    my_args.args.del.fapl_id = H5Pcopy(args->args.del.fapl_id);

    H5Pset_vol(my_args.args.del.fapl_id, info->under_vol_id,
               info->under_vol_info);

    new_args = &my_args;
  }
  else
  {
    under_vol_id = o->under_vol_id;
    new_args = args;
    new_o = (H5VL_as_rpc_t*)o->under_object;
  }

  herr_t ret_value =
      H5VLfile_specific(new_o, under_vol_id, new_args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, under_vol_id);

  if (args->op_type == H5VL_FILE_IS_ACCESSIBLE)
  {
    H5Pclose(my_args.args.is_accessible.fapl_id);
    H5VL_as_rpc_info_free(info);
  }
  else if (args->op_type == H5VL_FILE_DELETE)
  {
    H5Pclose(my_args.args.del.fapl_id);
    H5VL_as_rpc_info_free(info);
  }
  else if (args->op_type == H5VL_FILE_REOPEN)
  {
    // Wrap reopened file struct pointer, if we reopened one
    if (ret_value >= 0 && args->args.reopen.file)
      *args->args.reopen.file =
          H5VL_as_rpc_t_new_obj(*args->args.reopen.file, o->under_vol_id);
  }

  return ret_value;
}

herr_t H5VL_as_rpc_file_optional(void* obj, H5VL_optional_args_t* args,
                                 hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("File Optional");

  herr_t ret_value =
      H5VLfile_optional(o->under_object, o->under_vol_id, args, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  return ret_value;
}

herr_t H5VL_as_rpc_file_close(void* obj, hid_t dxpl_id, void** req)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("FILE Close");

  herr_t ret_value =
      H5VLfile_close(o->under_object, o->under_vol_id, dxpl_id, req);

  if (req && *req)
    *req = H5VL_as_rpc_t_new_obj(*req, o->under_vol_id);

  if (ret_value >= 0)
    H5VL_as_rpc_t_free_obj(o);

  return ret_value;
}

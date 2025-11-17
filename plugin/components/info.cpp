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

#include "info.hpp"
#include "log.hpp"
#include <cstdlib>
#include <cstring>

void* H5VL_as_rpc_info_copy(const void* info)
{
  auto info_cast = static_cast<const H5VL_as_rpc_info_t*>(info);

  log_msg("INFO Copy\n");

  auto new_info = new H5VL_as_rpc_info_t();

  new_info->under_vol_id = info_cast->under_vol_id;
  H5Iinc_ref(new_info->under_vol_id);
  if (info_cast->under_vol_info)
    H5VLcopy_connector_info(new_info->under_vol_id, &(new_info->under_vol_info),
                            info_cast->under_vol_info);

  return new_info;
}

herr_t H5VL_as_rpc_info_cmp(int* cmp_value, const void* info1,
                            const void* info2)
{
  auto info1_cast = static_cast<const H5VL_as_rpc_info_t*>(info1);
  auto info2_cast = static_cast<const H5VL_as_rpc_info_t*>(info2);

  log_msg("INFO Compare\n");

  *cmp_value = 0;

  // Compare under VOL connector classes
  H5VLcmp_connector_cls(cmp_value, info1_cast->under_vol_id,
                        info2_cast->under_vol_id);
  if (*cmp_value != 0)
    return 1;

  // Compare under VOL connector info objects
  H5VLcmp_connector_info(cmp_value, info1_cast->under_vol_id,
                         info1_cast->under_vol_info,
                         info2_cast->under_vol_info);
  if (*cmp_value != 0)
    return 1;

  return 0;
}

herr_t H5VL_as_rpc_info_free(void* info)
{
  auto info_cast = static_cast<H5VL_as_rpc_info_t*>(info);

  log_msg("INFO Free\n");

  hid_t err_id = H5Eget_current_stack();

  if (info_cast->under_vol_info)
    H5VLfree_connector_info(info_cast->under_vol_id, info_cast->under_vol_info);
  H5Idec_ref(info_cast->under_vol_id);

  H5Eset_current_stack(err_id);

  delete info_cast;

  return 0;
}

herr_t H5VL_as_rpc_info_to_str(const void* info, char** str)
{
  auto info_cast = static_cast<const H5VL_as_rpc_info_t*>(info);
  H5VL_class_value_t under_value = -1;
  char* under_vol_string = nullptr;
  size_t under_vol_str_len = 0;

  log_msg("INFO To String\n");

  H5VLget_value(info_cast->under_vol_id, &under_value);
  H5VLconnector_info_to_str(info_cast->under_vol_info, info_cast->under_vol_id,
                            &under_vol_string);

  if (under_vol_string)
    under_vol_str_len = strlen(under_vol_string);

  *str = static_cast<char*>(H5allocate_memory(32 + under_vol_str_len, false));

  sprintf(*str, "under_vol=%u;under_info={%s}",
          static_cast<unsigned>(under_value),
          (under_vol_string ? under_vol_string : ""));

  if (under_vol_string)
    H5free_memory(under_vol_string);

  return 0;
}

herr_t H5VL_as_rpc_str_to_info(const char* str, void** info)
{
  H5VL_as_rpc_info_t* info_cast;
  unsigned under_vol_value;
  const char *under_vol_info_start, *under_vol_info_end;
  hid_t under_vol_id;
  void* under_vol_info = nullptr;

  log_msg("INFO String To Info\n");

  // Retrieve the underlying VOL connector value and info
  sscanf(str, "under_vol=%u;", &under_vol_value);
  under_vol_id = H5VLregister_connector_by_value(
      (H5VL_class_value_t)under_vol_value, H5P_DEFAULT);
  under_vol_info_start = strchr(str, '{');
  under_vol_info_end = strrchr(str, '}');
  if (under_vol_info_end != (under_vol_info_start + 1))
  {
    unsigned info_str_len = under_vol_info_end - under_vol_info_start;
    char* under_vol_info_str = static_cast<char*>(malloc(info_str_len));
    memcpy(under_vol_info_str, under_vol_info_start + 1, info_str_len - 1);
    *(under_vol_info_str + info_str_len) = '\0';

    H5VLconnector_str_to_info(under_vol_info_str, under_vol_id,
                              &under_vol_info);

    free(under_vol_info_str);
  }

  info_cast = new H5VL_as_rpc_info_t();
  info_cast->under_vol_id = under_vol_id;
  info_cast->under_vol_info = under_vol_info;

  *info = info_cast;

  return 0;
}

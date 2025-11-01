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

void* H5VL_as_rpc_info_copy(const void* info)
{
  auto info_cast = static_cast<const H5VL_as_rpc_info_t*>(info);
  auto new_info = new H5VL_as_rpc_info_t;
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
  *cmp_value = 0;

  H5VLcmp_connector_cls(cmp_value, info1_cast->under_vol_id,
                        info2_cast->under_vol_id);
  if (*cmp_value != 0)
    return 1;

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
  const herr_t err = H5Eget_current_stack();

  if (info_cast->under_vol_info)
    H5VLfree_connector_info(info_cast->under_vol_id, info_cast->under_vol_info);

  H5Idec_ref(info_cast->under_vol_id);
  H5Eset_current_stack(err);
  delete info_cast;

  return 0;
}

herr_t H5VL_as_rpc_info_to_str([[maybe_unused]] const void* info,
                               [[maybe_unused]] char** str)
{
  return 0;
}

herr_t H5VL_as_rpc_info_from_str([[maybe_unused]] const char* str,
                                 [[maybe_unused]] void** info)
{

  return 0;
}

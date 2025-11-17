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
#include <cassert>
#include <cstdlib>
#include <cstring>

void* H5VL_as_rpc_info_copy(const void* _info)
{
  const H5VL_as_rpc_info_t* info = (const H5VL_as_rpc_info_t*)_info;
  H5VL_as_rpc_info_t* new_info;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL INFO Copy\n");
#endif

  new_info = (H5VL_as_rpc_info_t*)calloc(1, sizeof(H5VL_as_rpc_info_t));

  new_info->under_vol_id = info->under_vol_id;
  H5Iinc_ref(new_info->under_vol_id);
  if (info->under_vol_info)
    H5VLcopy_connector_info(new_info->under_vol_id, &(new_info->under_vol_info),
                            info->under_vol_info);

  return new_info;
}

herr_t H5VL_as_rpc_info_cmp(int* cmp_value, const void* _info1,
                            const void* _info2)
{
  const H5VL_as_rpc_info_t* info1 = (const H5VL_as_rpc_info_t*)_info1;
  const H5VL_as_rpc_info_t* info2 = (const H5VL_as_rpc_info_t*)_info2;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL INFO Compare\n");
#endif

  *cmp_value = 0;

  /* Compare under VOL connector classes */
  H5VLcmp_connector_cls(cmp_value, info1->under_vol_id, info2->under_vol_id);
  if (*cmp_value != 0)
    return 0;

  /* Compare under VOL connector info objects */
  H5VLcmp_connector_info(cmp_value, info1->under_vol_id, info1->under_vol_info,
                         info2->under_vol_info);
  if (*cmp_value != 0)
    return 0;

  return 0;
}

herr_t H5VL_as_rpc_info_free(void* _info)
{
  H5VL_as_rpc_info_t* info = (H5VL_as_rpc_info_t*)_info;
  hid_t err_id;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL INFO Free\n");
#endif

  err_id = H5Eget_current_stack();

  if (info->under_vol_info)
    H5VLfree_connector_info(info->under_vol_id, info->under_vol_info);
  H5Idec_ref(info->under_vol_id);

  H5Eset_current_stack(err_id);

  free(info);

  return 0;
}

herr_t H5VL_as_rpc_info_to_str(const void* _info, char** str)
{
  const H5VL_as_rpc_info_t* info = (const H5VL_as_rpc_info_t*)_info;
  H5VL_class_value_t under_value = (H5VL_class_value_t)-1;
  char* under_vol_string = NULL;
  size_t under_vol_str_len = 0;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL INFO To String\n");
#endif

  H5VLget_value(info->under_vol_id, &under_value);
  H5VLconnector_info_to_str(info->under_vol_info, info->under_vol_id,
                            &under_vol_string);

  if (under_vol_string)
    under_vol_str_len = strlen(under_vol_string);

  *str = (char*)H5allocate_memory(32 + under_vol_str_len, (hbool_t)0);
  assert(*str);

  sprintf(*str, "under_vol=%u;under_info={%s}", (unsigned)under_value,
          (under_vol_string ? under_vol_string : ""));

  if (under_vol_string)
    H5free_memory(under_vol_string);

  return 0;
}

herr_t H5VL_as_rpc_str_to_info(const char* str, void** _info)
{
  H5VL_as_rpc_info_t* info;
  unsigned under_vol_value;
  const char *under_vol_info_start, *under_vol_info_end;
  hid_t under_vol_id;
  void* under_vol_info = NULL;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL INFO String To Info\n");
#endif

  /* Retrieve the underlying VOL connector value and info */
  sscanf(str, "under_vol=%u;", &under_vol_value);
  under_vol_id = H5VLregister_connector_by_value(
      (H5VL_class_value_t)under_vol_value, H5P_DEFAULT);
  under_vol_info_start = strchr(str, '{');
  under_vol_info_end = strrchr(str, '}');
  assert(under_vol_info_end > under_vol_info_start);
  if (under_vol_info_end != (under_vol_info_start + 1))
  {
    char* under_vol_info_str;

    under_vol_info_str =
        (char*)malloc((size_t)(under_vol_info_end - under_vol_info_start));
    memcpy(under_vol_info_str, under_vol_info_start + 1,
           (size_t)((under_vol_info_end - under_vol_info_start) - 1));
    *(under_vol_info_str + (under_vol_info_end - under_vol_info_start)) = '\0';

    H5VLconnector_str_to_info(under_vol_info_str, under_vol_id,
                              &under_vol_info);

    free(under_vol_info_str);
  }

  info = (H5VL_as_rpc_info_t*)calloc(1, sizeof(H5VL_as_rpc_info_t));
  info->under_vol_id = under_vol_id;
  info->under_vol_info = under_vol_info;

  printf("%ld, %p\n", under_vol_id, under_vol_info);

  *_info = info;

  return 0;
}

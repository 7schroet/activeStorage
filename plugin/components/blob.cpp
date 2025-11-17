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

#include "blob.hpp"
#include "utils.hpp"

herr_t H5VL_as_rpc_blob_put(void* obj, const void* buf, size_t size,
                            void* blob_id, void* ctx)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL BLOB Put\n");
#endif

  ret_value =
      H5VLblob_put(o->under_object, o->under_vol_id, buf, size, blob_id, ctx);

  return ret_value;
}

herr_t H5VL_as_rpc_blob_get(void* obj, const void* blob_id, void* buf,
                            size_t size, void* ctx)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL BLOB Get\n");
#endif

  ret_value =
      H5VLblob_get(o->under_object, o->under_vol_id, blob_id, buf, size, ctx);

  return ret_value;
}

herr_t H5VL_as_rpc_blob_specific(void* obj, void* blob_id,
                                 H5VL_blob_specific_args_t* args)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL BLOB Specific\n");
#endif

  ret_value =
      H5VLblob_specific(o->under_object, o->under_vol_id, blob_id, args);

  return ret_value;
}

herr_t H5VL_as_rpc_blob_optional(void* obj, void* blob_id,
                                 H5VL_optional_args_t* args)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL BLOB Optional\n");
#endif

  ret_value =
      H5VLblob_optional(o->under_object, o->under_vol_id, blob_id, args);

  return ret_value;
}

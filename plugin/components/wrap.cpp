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

#include "wrap.hpp"
#include "utils.hpp"
#include <cstdlib>

void* H5VL_as_rpc_get_object(const void* obj)
{
  const H5VL_as_rpc_t* o = (const H5VL_as_rpc_t*)obj;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL Get object\n");
#endif

  return H5VLget_object(o->under_object, o->under_vol_id);
}

herr_t H5VL_as_rpc_get_wrap_ctx(const void* obj, void** wrap_ctx)
{
  const H5VL_as_rpc_t* o = (const H5VL_as_rpc_t*)obj;
  H5VL_as_rpc_wrap_ctx_t* new_wrap_ctx;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL WRAP CTX Get\n");
#endif

  new_wrap_ctx =
      (H5VL_as_rpc_wrap_ctx_t*)calloc(1, sizeof(H5VL_as_rpc_wrap_ctx_t));

  new_wrap_ctx->under_vol_id = o->under_vol_id;
  H5Iinc_ref(new_wrap_ctx->under_vol_id);
  H5VLget_wrap_ctx(o->under_object, o->under_vol_id,
                   &new_wrap_ctx->under_wrap_ctx);

  *wrap_ctx = new_wrap_ctx;

  return 0;
}

void* H5VL_as_rpc_wrap_object(void* obj, H5I_type_t obj_type, void* _wrap_ctx)
{
  H5VL_as_rpc_wrap_ctx_t* wrap_ctx = (H5VL_as_rpc_wrap_ctx_t*)_wrap_ctx;
  H5VL_as_rpc_t* new_obj;
  void* under;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL WRAP Object\n");
#endif

  under = H5VLwrap_object(obj, obj_type, wrap_ctx->under_vol_id,
                          wrap_ctx->under_wrap_ctx);
  if (under)
    new_obj = H5VL_as_rpc_t_new_obj(under, wrap_ctx->under_vol_id);
  else
    new_obj = NULL;

  return new_obj;
}

void* H5VL_as_rpc_unwrap_object(void* obj)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  void* under;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL UNWRAP Object\n");
#endif

  under = H5VLunwrap_object(o->under_object, o->under_vol_id);

  if (under)
    H5VL_as_rpc_t_free_obj(o);

  return under;
}

herr_t H5VL_as_rpc_free_wrap_ctx(void* _wrap_ctx)
{
  H5VL_as_rpc_wrap_ctx_t* wrap_ctx = (H5VL_as_rpc_wrap_ctx_t*)_wrap_ctx;
  hid_t err_id;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL WRAP CTX Free\n");
#endif

  err_id = H5Eget_current_stack();

  if (wrap_ctx->under_wrap_ctx)
    H5VLfree_wrap_ctx(wrap_ctx->under_wrap_ctx, wrap_ctx->under_vol_id);
  H5Idec_ref(wrap_ctx->under_vol_id);

  H5Eset_current_stack(err_id);

  free(wrap_ctx);

  return 0;
}

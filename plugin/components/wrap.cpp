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
#include "log.hpp"
#include "utils.hpp"

void* H5VL_as_rpc_get_object(const void* obj)
{
  auto o = static_cast<const H5VL_as_rpc_t*>(obj);
  log_msg("Get object");
  return H5VLget_object(o->under_object, o->under_vol_id);
}

herr_t H5VL_as_rpc_get_wrap_ctx(const void* obj, void** wrap_ctx)
{
  auto o = static_cast<const H5VL_as_rpc_t*>(obj);
  auto new_wrap_ctx = new H5VL_as_rpc_wrap_ctx_t();

  log_msg("WRAP CTX Get");

  new_wrap_ctx->under_vol_id = o->under_vol_id;
  H5Iinc_ref(new_wrap_ctx->under_vol_id);
  H5VLget_wrap_ctx(o->under_object, o->under_vol_id,
                   &new_wrap_ctx->under_wrap_ctx);

  *wrap_ctx = new_wrap_ctx;

  return 0;
}

void* H5VL_as_rpc_wrap_object(void* obj, H5I_type_t obj_type, void* wrap_ctx)
{
  auto wrap_ctx_cast = static_cast<H5VL_as_rpc_wrap_ctx_t*>(wrap_ctx);
  H5VL_as_rpc_t* new_obj = nullptr;

  log_msg("WRAP Object");

  void* under = H5VLwrap_object(obj, obj_type, wrap_ctx_cast->under_vol_id,
                                wrap_ctx_cast->under_wrap_ctx);
  if (under)
    new_obj = H5VL_as_rpc_t_new_obj(under, wrap_ctx_cast->under_vol_id);

  return new_obj;
}

void* H5VL_as_rpc_unwrap_object(void* obj)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("UNWRAP Object");

  void* under = H5VLunwrap_object(o->under_object, o->under_vol_id);

  if (under)
    H5VL_as_rpc_t_free_obj(o);

  return under;
}

herr_t H5VL_as_rpc_free_wrap_ctx(void* obj)
{
  auto wrap_ctx = static_cast<H5VL_as_rpc_wrap_ctx_t*>(obj);

  log_msg("WRAP CTX Free");

  const hid_t err_id = H5Eget_current_stack();

  if (wrap_ctx->under_wrap_ctx)
    H5VLfree_wrap_ctx(wrap_ctx->under_wrap_ctx, wrap_ctx->under_vol_id);
  H5Idec_ref(wrap_ctx->under_vol_id);

  H5Eset_current_stack(err_id);

  delete wrap_ctx;

  return 0;
}

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
#include <H5Epublic.h>
#include <H5Ipublic.h>
#include <H5VLconnector_passthru.h>
#include <cstdlib>

namespace
{
H5VL_as_rpc_t* H5VL_as_rpc_new_obj(void* under_obj, hid_t under_vol_id)
{
  H5VL_as_rpc_t* new_obj;
  new_obj = new H5VL_as_rpc_t;
  new_obj->under_object = under_obj;
  new_obj->under_vol_id = under_vol_id;
  H5Iinc_ref(under_vol_id);
  return new_obj;
}

herr_t H5VL_as_rpc_free_obj(H5VL_as_rpc_t* obj)
{
  hid_t err;
  err = H5Eget_current_stack();
  H5Idec_ref(obj->under_vol_id);
  H5Eset_current_stack(err);
  delete obj;
  return 0;
}

} // namespace

void* H5VL_as_rpc_get_object(const void* obj)
{
  auto o = static_cast<const H5VL_as_rpc_t*>(obj);
  return H5VLget_object(o->under_object, o->under_vol_id);
}

herr_t H5VL_as_rpc_get_wrap_ctx(const void* obj, void** wrap_ctx)
{
  auto o = static_cast<const H5VL_as_rpc_t*>(obj);
  H5VL_as_rpc_ctx_t* new_wrap_ctx;

  new_wrap_ctx = new H5VL_as_rpc_ctx_t;
  new_wrap_ctx->under_vol_id = o->under_vol_id;
  H5Iinc_ref(new_wrap_ctx->under_vol_id);
  H5VLget_wrap_ctx(o->under_object, o->under_vol_id,
                   &new_wrap_ctx->under_wrap_ctx);

  *wrap_ctx = new_wrap_ctx;

  return 0;
}

void* H5VL_as_rpc_wrap_object(void* obj, H5I_type_t obj_type, void* wrap_ctx)
{
  auto ctx = static_cast<H5VL_as_rpc_ctx_t*>(wrap_ctx);
  H5VL_as_rpc_t* new_obj = nullptr;
  void* under;

  under =
      H5VLwrap_object(obj, obj_type, ctx->under_vol_id, ctx->under_wrap_ctx);
  if (under)
    new_obj = H5VL_as_rpc_new_obj(under, ctx->under_vol_id);

  return new_obj;
}

void* H5VL_as_rpc_unwrap_object(void* obj)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  void* under = H5VLunwrap_object(o->under_object, o->under_vol_id);

  if (under)
    H5VL_as_rpc_free_obj(o);

  return under;
}

herr_t H5VL_as_rpc_free_wrap_ctx(void* wrap_ctx)
{
  auto ctx = static_cast<H5VL_as_rpc_ctx_t*>(wrap_ctx);
  hid_t err;

  err = H5Eget_current_stack();
  if (ctx->under_wrap_ctx)
    H5VLfree_wrap_ctx(ctx->under_wrap_ctx, ctx->under_vol_id);
  H5Idec_ref(ctx->under_vol_id);
  H5Eset_current_stack(err);
  delete ctx;

  return 0;
}

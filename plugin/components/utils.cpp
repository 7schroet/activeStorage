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

#include "utils.hpp"

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

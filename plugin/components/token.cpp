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

#include "token.hpp"
#include "log.hpp"
#include "utils.hpp"

herr_t H5VL_as_rpc_token_cmp(void* obj, const H5O_token_t* token1,
                             const H5O_token_t* token2, int* cmp_value)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  log_msg("TOKEN Compare");
  return H5VLtoken_cmp(o->under_object, o->under_vol_id, token1, token2,
                       cmp_value);
}

herr_t H5VL_as_rpc_token_to_str(void* obj, H5I_type_t obj_type,
                                const H5O_token_t* token, char** token_str)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  log_msg("TOKEN To string");
  return H5VLtoken_to_str(o->under_object, obj_type, o->under_vol_id, token,
                          token_str);
}

herr_t H5VL_as_rpc_token_from_str(void* obj, H5I_type_t obj_type,
                                  const char* token_str, H5O_token_t* token)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  log_msg("TOKEN From string");
  return H5VLtoken_from_str(o->under_object, obj_type, o->under_vol_id,
                            token_str, token);
}

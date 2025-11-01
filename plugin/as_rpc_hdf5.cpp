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

#include "as_rpc_hdf5.hpp"
#include "attribute.hpp"
#include "wrap.hpp"
#include <H5PLextern.h>
#include <H5VLpassthru.h>
#include <H5VLpublic.h>
#include <print>

herr_t H5VL_as_rpc_init([[maybe_unused]] hid_t vipl_id)
{
  std::println("Calling the init");
  return 0;
}

herr_t H5VL_as_rpc_terminate()
{
  std::println("Calling the term");
  return 0;
}

// clang-format off
static const H5VL_class_t H5VL_as_rpc_g = {
  .version = 3,
  .value = 567,
  .name = "as-rpc-hdf5",
  .conn_version = 1,
  .cap_flags = 0,
  .initialize = H5VL_as_rpc_init,
  .terminate = H5VL_as_rpc_terminate,
  .info_cls = {
    .size = 0,
    .copy = nullptr,
    .cmp = nullptr,
    .free = nullptr,
    .to_str = nullptr,
    .from_str = nullptr,
  },
  .wrap_cls = {
    .get_object = H5VL_as_rpc_get_object,
    .get_wrap_ctx = H5VL_as_rpc_get_wrap_ctx,
    .wrap_object = H5VL_as_rpc_wrap_object,
    .unwrap_object = H5VL_as_rpc_unwrap_object,
    .free_wrap_ctx = H5VL_as_rpc_free_wrap_ctx,
  },
  .attr_cls = {
    .create = H5VL_as_rpc_attr_create,
    .open = H5VL_as_rpc_attr_open,
    .read = H5VL_as_rpc_attr_read,
    .write = H5VL_as_rpc_attr_write,
    .get = H5VL_as_rpc_attr_get,
    .specific = H5VL_as_rpc_attr_specific,
    .optional = H5VL_as_rpc_attr_optional,
    .close = H5VL_as_rpc_attr_close,
   },
  .dataset_cls = {
    .create = nullptr,
    .open = nullptr,
    .read = nullptr,
    .write = nullptr,
    .get = nullptr,
    .specific = nullptr,
    .optional = nullptr,
    .close = nullptr,
   },
  .datatype_cls = {
    .commit = nullptr,
    .open = nullptr,
    .get = nullptr,
    .specific = nullptr,
    .optional = nullptr,
    .close = nullptr,
  },
  .file_cls = {
    .create = nullptr,
    .open = nullptr,
    .get = nullptr,
    .specific = nullptr,
    .optional = nullptr,
    .close = nullptr,
  },
  .group_cls = {
    .create = nullptr,
    .open = nullptr,
    .get = nullptr,
    .specific = nullptr,
    .optional = nullptr,
    .close = nullptr,
  },
  .link_cls = {
    .create = nullptr,
    .copy = nullptr,
    .move = nullptr,
    .get = nullptr,
    .specific = nullptr,
    .optional = nullptr,
  },
  .object_cls = {
    .open = nullptr,
    .copy = nullptr,
    .get = nullptr,
    .specific = nullptr,
    .optional = nullptr,
  },
  .introspect_cls = {
    .get_conn_cls = nullptr,
    .get_cap_flags = nullptr,
    .opt_query = nullptr,
  },
  .request_cls = {
    .wait = nullptr,
    .notify = nullptr,
    .cancel = nullptr,
    .specific = nullptr,
    .optional = nullptr,
    .free = nullptr,
  },
  .blob_cls = {
    .put = nullptr,
    .get = nullptr,
    .specific = nullptr,
    .optional = nullptr,
  },
  .token_cls = {
    .cmp = nullptr,
    .to_str = nullptr,
    .from_str = nullptr,
  },
  .optional = nullptr
};
// clang-format on

H5PL_type_t H5PLget_plugin_type(void) { return H5PL_TYPE_VOL; }

const void* H5PLget_plugin_info(void) { return &H5VL_as_rpc_g; }

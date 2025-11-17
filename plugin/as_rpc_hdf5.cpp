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

#include "attribute.hpp"
#include "blob.hpp"
#include "dataset.hpp"
#include "datatype.hpp"
#include "file.hpp"
#include "group.hpp"
#include "info.hpp"
#include "init.hpp"
#include "link.hpp"
#include "log.hpp"
#include "object.hpp"
#include "optional.hpp"
#include "request.hpp"
#include "token.hpp"
#include "utils.hpp"
#include "wrap.hpp"

#include <H5PLextern.h>
#include <H5VLpublic.h>
#include <hdf5.h>

namespace
{
/* The connector identification number, initialized at runtime */
hid_t H5VL_PASSTHRU_EXT_g = H5I_INVALID_HID;

/* Defined separately due to dependency on connector struct */
herr_t H5VL_as_rpc_introspect_get_conn_cls(void* obj, H5VL_get_conn_lvl_t lvl,
                                           const H5VL_class_t** conn_cls);
herr_t H5VL_as_rpc_introspect_get_cap_flags(const void* info,
                                            uint64_t* cap_flags);
herr_t H5VL_as_rpc_introspect_opt_query(void* obj, H5VL_subclass_t cls,
                                        int op_type, uint64_t* flags);

/* Pass through VOL connector class struct */
const H5VL_class_t H5VL_pass_through_ext_g = {
    .version = H5VL_VERSION,
    .value = 1234,
    .name = "as-rpc-hdf5",
    .conn_version = 1,
    .cap_flags = 0,
    .initialize = H5VL_as_rpc_init,
    .terminate = H5VL_as_rpc_term,
    .info_cls = {.size = sizeof(H5VL_as_rpc_info_t),
                 .copy = H5VL_as_rpc_info_copy,
                 .cmp = H5VL_as_rpc_info_cmp,
                 .free = H5VL_as_rpc_info_free,
                 .to_str = H5VL_as_rpc_info_to_str,
                 .from_str = H5VL_as_rpc_str_to_info},
    .wrap_cls = {.get_object = H5VL_as_rpc_get_object,
                 .get_wrap_ctx = H5VL_as_rpc_get_wrap_ctx,
                 .wrap_object = H5VL_as_rpc_wrap_object,
                 .unwrap_object = H5VL_as_rpc_unwrap_object,
                 .free_wrap_ctx = H5VL_as_rpc_free_wrap_ctx},
    .attr_cls = {.create = H5VL_as_rpc_attr_create,
                 .open = H5VL_as_rpc_attr_open,
                 .read = H5VL_as_rpc_attr_read,
                 .write = H5VL_as_rpc_attr_write,
                 .get = H5VL_as_rpc_attr_get,
                 .specific = H5VL_as_rpc_attr_specific,
                 .optional = H5VL_as_rpc_attr_optional,
                 .close = H5VL_as_rpc_attr_close},
    .dataset_cls = {.create = H5VL_as_rpc_dataset_create,
                    .open = H5VL_as_rpc_dataset_open,
                    .read = H5VL_as_rpc_dataset_read,
                    .write = H5VL_as_rpc_dataset_write,
                    .get = H5VL_as_rpc_dataset_get,
                    .specific = H5VL_as_rpc_dataset_specific,
                    .optional = H5VL_as_rpc_dataset_optional,
                    .close = H5VL_as_rpc_dataset_close},
    .datatype_cls = {.commit = H5VL_as_rpc_datatype_commit,
                     .open = H5VL_as_rpc_datatype_open,
                     .get = H5VL_as_rpc_datatype_get,
                     .specific = H5VL_as_rpc_datatype_specific,
                     .optional = H5VL_as_rpc_datatype_optional,
                     .close = H5VL_as_rpc_datatype_close},
    .file_cls = {.create = H5VL_as_rpc_file_create,
                 .open = H5VL_as_rpc_file_open,
                 .get = H5VL_as_rpc_file_get,
                 .specific = H5VL_as_rpc_file_specific,
                 .optional = H5VL_as_rpc_file_optional,
                 .close = H5VL_as_rpc_file_close},
    .group_cls = {.create = H5VL_as_rpc_group_create,
                  .open = H5VL_as_rpc_group_open,
                  .get = H5VL_as_rpc_group_get,
                  .specific = H5VL_as_rpc_group_specific,
                  .optional = H5VL_as_rpc_group_optional,
                  .close = H5VL_as_rpc_group_close},
    .link_cls = {.create = H5VL_as_rpc_link_create,
                 .copy = H5VL_as_rpc_link_copy,
                 .move = H5VL_as_rpc_link_move,
                 .get = H5VL_as_rpc_link_get,
                 .specific = H5VL_as_rpc_link_specific,
                 .optional = H5VL_as_rpc_link_optional},
    .object_cls = {.open = H5VL_as_rpc_object_open,
                   .copy = H5VL_as_rpc_object_copy,
                   .get = H5VL_as_rpc_object_get,
                   .specific = H5VL_as_rpc_object_specific,
                   .optional = H5VL_as_rpc_object_optional},
    .introspect_cls = {.get_conn_cls = H5VL_as_rpc_introspect_get_conn_cls,
                       .get_cap_flags = H5VL_as_rpc_introspect_get_cap_flags,
                       .opt_query = H5VL_as_rpc_introspect_opt_query},
    .request_cls = {.wait = H5VL_as_rpc_request_wait,
                    .notify = H5VL_as_rpc_request_notify,
                    .cancel = H5VL_as_rpc_request_cancel,
                    .specific = H5VL_as_rpc_request_specific,
                    .optional = H5VL_as_rpc_request_optional,
                    .free = H5VL_as_rpc_request_free},
    .blob_cls = {.put = H5VL_as_rpc_blob_put,
                 .get = H5VL_as_rpc_blob_get,
                 .specific = H5VL_as_rpc_blob_specific,
                 .optional = H5VL_as_rpc_blob_optional},
    .token_cls = {.cmp = H5VL_as_rpc_token_cmp,
                  .to_str = H5VL_as_rpc_token_to_str,
                  .from_str = H5VL_as_rpc_token_from_str},
    .optional = H5VL_as_rpc_optional};
} // namespace

/* Required shim routines, to enable dynamic loading of shared library.
 * The HDF5 library _must_ find routines with these names and signatures
 * for a shared library that contains a VOL connector to be detected
 * and loaded at runtime.
 */
extern "C"
{
  H5PL_type_t H5PLget_plugin_type(void) { return H5PL_TYPE_VOL; }
  const void* H5PLget_plugin_info(void) { return &H5VL_pass_through_ext_g; }
  hid_t H5VL_pass_through_ext_register(void)
  {
    /* Singleton register the pass-through VOL connector ID */
    if (H5VL_PASSTHRU_EXT_g < 0)
      H5VL_PASSTHRU_EXT_g =
          H5VLregister_connector(&H5VL_pass_through_ext_g, H5P_DEFAULT);

    return H5VL_PASSTHRU_EXT_g;
  }
}

/* Introspection implementation */
namespace
{
herr_t H5VL_as_rpc_introspect_get_conn_cls(void* obj, H5VL_get_conn_lvl_t lvl,
                                           const H5VL_class_t** conn_cls)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);
  herr_t ret_value;

  log_msg("INTROSPECT GetConnCls\n");

  /* Check for querying this connector's class */
  if (H5VL_GET_CONN_LVL_CURR == lvl)
  {
    *conn_cls = &H5VL_pass_through_ext_g;
    ret_value = 0;
  }
  else
    ret_value = H5VLintrospect_get_conn_cls(o->under_object, o->under_vol_id,
                                            lvl, conn_cls);

  return ret_value;
}

herr_t H5VL_as_rpc_introspect_get_cap_flags(const void* info,
                                            uint64_t* cap_flags)
{
  auto info_cast = static_cast<const H5VL_as_rpc_info_t*>(info);

  log_msg("INTROSPECT GetCapFlags\n");

  /* Invoke the query on the underlying VOL connector */
  herr_t ret_value = H5VLintrospect_get_cap_flags(
      info_cast->under_vol_info, info_cast->under_vol_id, cap_flags);

  /* Bitwise OR our capability flags in */
  if (ret_value >= 0)
    *cap_flags |= H5VL_pass_through_ext_g.cap_flags;

  return ret_value;
}

herr_t H5VL_as_rpc_introspect_opt_query(void* obj, H5VL_subclass_t cls,
                                        int op_type, uint64_t* flags)
{
  auto o = static_cast<H5VL_as_rpc_t*>(obj);

  log_msg("INTROSPECT OptQuery\n");

  herr_t ret_value = H5VLintrospect_opt_query(o->under_object, o->under_vol_id,
                                              cls, op_type, flags);

  return ret_value;
}
} // namespace

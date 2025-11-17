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
#include "dataset.hpp"
#include "datatype.hpp"
#include "file.hpp"
#include "group.hpp"
#include "info.hpp"
#include "link.hpp"
#include "object.hpp"
#include "request.hpp"
#include "utils.hpp"
#include "wrap.hpp"
#include <assert.h>
#include <hdf5.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <H5VLpublic.h>

#define ENABLE_EXT_PASSTHRU_LOGGING

/********************* */
/* Function prototypes */
/********************* */

/* "Management" callbacks */
static herr_t H5VL_pass_through_ext_init(hid_t vipl_id);
static herr_t H5VL_pass_through_ext_term(void);

/* Container/connector introspection callbacks */
static herr_t H5VL_pass_through_ext_introspect_get_conn_cls(
    void* obj, H5VL_get_conn_lvl_t lvl, const H5VL_class_t** conn_cls);
static herr_t
H5VL_pass_through_ext_introspect_get_cap_flags(const void* info,
                                               uint64_t* cap_flags);
static herr_t H5VL_pass_through_ext_introspect_opt_query(void* obj,
                                                         H5VL_subclass_t cls,
                                                         int op_type,
                                                         uint64_t* flags);

/* Blob callbacks */
static herr_t H5VL_pass_through_ext_blob_put(void* obj, const void* buf,
                                             size_t size, void* blob_id,
                                             void* ctx);
static herr_t H5VL_pass_through_ext_blob_get(void* obj, const void* blob_id,
                                             void* buf, size_t size, void* ctx);
static herr_t
H5VL_pass_through_ext_blob_specific(void* obj, void* blob_id,
                                    H5VL_blob_specific_args_t* args);
static herr_t H5VL_pass_through_ext_blob_optional(void* obj, void* blob_id,
                                                  H5VL_optional_args_t* args);

/* Token callbacks */
static herr_t H5VL_pass_through_ext_token_cmp(void* obj,
                                              const H5O_token_t* token1,
                                              const H5O_token_t* token2,
                                              int* cmp_value);
static herr_t H5VL_pass_through_ext_token_to_str(void* obj, H5I_type_t obj_type,
                                                 const H5O_token_t* token,
                                                 char** token_str);
static herr_t H5VL_pass_through_ext_token_from_str(void* obj,
                                                   H5I_type_t obj_type,
                                                   const char* token_str,
                                                   H5O_token_t* token);

/* Generic optional callback */
static herr_t H5VL_pass_through_ext_optional(void* obj,
                                             H5VL_optional_args_t* args,
                                             hid_t dxpl_id, void** req);
/*******************/
/* Local variables */
/*******************/

/* Pass through VOL connector class struct */
static const H5VL_class_t H5VL_pass_through_ext_g = {
    H5VL_VERSION,               /* VOL class struct version */
    1234,                       /* value        */
    "as-rpc-hdf5",              /* name         */
    1,                          /* connector version */
    0,                          /* capability flags */
    H5VL_pass_through_ext_init, /* initialize   */
    H5VL_pass_through_ext_term, /* terminate    */
    {
        sizeof(H5VL_as_rpc_info_t), /* size    */
        H5VL_as_rpc_info_copy,      /* copy    */
        H5VL_as_rpc_info_cmp,       /* compare */
        H5VL_as_rpc_info_free,      /* free    */
        H5VL_as_rpc_info_to_str,    /* to_str  */
        H5VL_as_rpc_str_to_info     /* from_str */
    },
    {
        H5VL_as_rpc_get_object,    /* get_object   */
        H5VL_as_rpc_get_wrap_ctx,  /* get_wrap_ctx */
        H5VL_as_rpc_wrap_object,   /* wrap_object  */
        H5VL_as_rpc_unwrap_object, /* unwrap_object */
        H5VL_as_rpc_free_wrap_ctx  /* free_wrap_ctx */
    },
    {
        H5VL_as_rpc_attr_create,   /* create */
        H5VL_as_rpc_attr_open,     /* open */
        H5VL_as_rpc_attr_read,     /* read */
        H5VL_as_rpc_attr_write,    /* write */
        H5VL_as_rpc_attr_get,      /* get */
        H5VL_as_rpc_attr_specific, /* specific */
        H5VL_as_rpc_attr_optional, /* optional */
        H5VL_as_rpc_attr_close     /* close */
    },
    {
        H5VL_as_rpc_dataset_create,   /* create */
        H5VL_as_rpc_dataset_open,     /* open */
        H5VL_as_rpc_dataset_read,     /* read */
        H5VL_as_rpc_dataset_write,    /* write */
        H5VL_as_rpc_dataset_get,      /* get */
        H5VL_as_rpc_dataset_specific, /* specific */
        H5VL_as_rpc_dataset_optional, /* optional */
        H5VL_as_rpc_dataset_close     /* close */
    },
    {
        H5VL_as_rpc_datatype_commit,   /* commit */
        H5VL_as_rpc_datatype_open,     /* open */
        H5VL_as_rpc_datatype_get,      /* get_size */
        H5VL_as_rpc_datatype_specific, /* specific */
        H5VL_as_rpc_datatype_optional, /* optional */
        H5VL_as_rpc_datatype_close     /* close */
    },
    {
        H5VL_as_rpc_file_create,   /* create */
        H5VL_as_rpc_file_open,     /* open */
        H5VL_as_rpc_file_get,      /* get */
        H5VL_as_rpc_file_specific, /* specific */
        H5VL_as_rpc_file_optional, /* optional */
        H5VL_as_rpc_file_close     /* close */
    },
    {
        H5VL_as_rpc_group_create,   /* create */
        H5VL_as_rpc_group_open,     /* open */
        H5VL_as_rpc_group_get,      /* get */
        H5VL_as_rpc_group_specific, /* specific */
        H5VL_as_rpc_group_optional, /* optional */
        H5VL_as_rpc_group_close     /* close */
    },
    {
        H5VL_as_rpc_link_create,   /* create */
        H5VL_as_rpc_link_copy,     /* copy */
        H5VL_as_rpc_link_move,     /* move */
        H5VL_as_rpc_link_get,      /* get */
        H5VL_as_rpc_link_specific, /* specific */
        H5VL_as_rpc_link_optional  /* optional */
    },
    {
        H5VL_as_rpc_object_open,     /* open */
        H5VL_as_rpc_object_copy,     /* copy */
        H5VL_as_rpc_object_get,      /* get */
        H5VL_as_rpc_object_specific, /* specific */
        H5VL_as_rpc_object_optional  /* optional */
    },
    {
        /* introspect_cls */
        H5VL_pass_through_ext_introspect_get_conn_cls,  /* get_conn_cls */
        H5VL_pass_through_ext_introspect_get_cap_flags, /* get_cap_flags */
        H5VL_pass_through_ext_introspect_opt_query,     /* opt_query */
    },
    {
        H5VL_as_rpc_request_wait,     /* wait */
        H5VL_as_rpc_request_notify,   /* notify */
        H5VL_as_rpc_request_cancel,   /* cancel */
        H5VL_as_rpc_request_specific, /* specific */
        H5VL_as_rpc_request_optional, /* optional */
        H5VL_as_rpc_request_free      /* free */
    },
    {
        /* blob_cls */
        H5VL_pass_through_ext_blob_put,      /* put */
        H5VL_pass_through_ext_blob_get,      /* get */
        H5VL_pass_through_ext_blob_specific, /* specific */
        H5VL_pass_through_ext_blob_optional  /* optional */
    },
    {
        /* token_cls */
        H5VL_pass_through_ext_token_cmp,     /* cmp */
        H5VL_pass_through_ext_token_to_str,  /* to_str */
        H5VL_pass_through_ext_token_from_str /* from_str */
    },
    H5VL_pass_through_ext_optional /* optional */
};

/* The connector identification number, initialized at runtime */
static hid_t H5VL_PASSTHRU_EXT_g = H5I_INVALID_HID;

/* Required shim routines, to enable dynamic loading of shared library */
/* The HDF5 library _must_ find routines with these names and signatures
 *      for a shared library that contains a VOL connector to be detected
 *      and loaded at runtime.
 */
extern "C"
{
  H5PL_type_t H5PLget_plugin_type(void) { return H5PL_TYPE_VOL; }
  const void* H5PLget_plugin_info(void) { return &H5VL_pass_through_ext_g; }
  /*-------------------------------------------------------------------------
   * Function:    H5VL_pass_through_ext_register
   *
   * Purpose:     Register the pass-through VOL connector and retrieve an ID
   *              for it.
   *
   * Return:      Success:    The ID for the pass-through VOL connector
   *              Failure:    -1
   *
   * Programmer:  Quincey Koziol
   *              Wednesday, November 28, 2018
   *
   *-------------------------------------------------------------------------
   */
  hid_t H5VL_pass_through_ext_register(void)
  {
    /* Singleton register the pass-through VOL connector ID */
    if (H5VL_PASSTHRU_EXT_g < 0)
      H5VL_PASSTHRU_EXT_g =
          H5VLregister_connector(&H5VL_pass_through_ext_g, H5P_DEFAULT);

    return H5VL_PASSTHRU_EXT_g;
  } /* end H5VL_pass_through_ext_register() */
}

/*-------------------------------------------------------------------------
 * Function:    H5VL_pass_through_ext_init
 *
 * Purpose:     Initialize this VOL connector, performing any necessary
 *              operations for the connector that will apply to all containers
 *              accessed with the connector.
 *
 * Return:      Success:    0
 *              Failure:    -1
 *
 *-------------------------------------------------------------------------
 */
static herr_t H5VL_pass_through_ext_init(hid_t vipl_id)
{
#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL INIT\n");
#endif

  /* Shut compiler up about unused parameter */
  (void)vipl_id;

  return 0;
} /* end H5VL_pass_through_ext_init() */

/*---------------------------------------------------------------------------
 * Function:    H5VL_pass_through_ext_term
 *
 * Purpose:     Terminate this VOL connector, performing any necessary
 *              operations for the connector that release connector-wide
 *              resources (usually created / initialized with the 'init'
 *              callback).
 *
 * Return:      Success:    0
 *              Failure:    (Can't fail)
 *
 *---------------------------------------------------------------------------
 */
static herr_t H5VL_pass_through_ext_term(void)
{
#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL TERM\n");
#endif

  /* Reset VOL ID */
  H5VL_PASSTHRU_EXT_g = H5I_INVALID_HID;

  return 0;
} /* end H5VL_pass_through_ext_term() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_pass_through_ext_introspect_get_conn_clss
 *
 * Purpose:     Query the connector class.
 *
 * Return:      SUCCEED / FAIL
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_pass_through_ext_introspect_get_conn_cls(
    void* obj, H5VL_get_conn_lvl_t lvl, const H5VL_class_t** conn_cls)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL INTROSPECT GetConnCls\n");
#endif

  /* Check for querying this connector's class */
  if (H5VL_GET_CONN_LVL_CURR == lvl)
  {
    *conn_cls = &H5VL_pass_through_ext_g;
    ret_value = 0;
  } /* end if */
  else
    ret_value = H5VLintrospect_get_conn_cls(o->under_object, o->under_vol_id,
                                            lvl, conn_cls);

  return ret_value;
} /* end H5VL_pass_through_ext_introspect_get_conn_cls() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_pass_through_ext_introspect_get_cap_flags
 *
 * Purpose:     Query the capability flags for this connector and any
 *              underlying connector(s).
 *
 * Return:      SUCCEED / FAIL
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_pass_through_ext_introspect_get_cap_flags(const void* _info,
                                                      uint64_t* cap_flags)
{
  const H5VL_as_rpc_info_t* info = (const H5VL_as_rpc_info_t*)_info;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL INTROSPECT GetCapFlags\n");
#endif

  /* Invoke the query on the underlying VOL connector */
  ret_value = H5VLintrospect_get_cap_flags(info->under_vol_info,
                                           info->under_vol_id, cap_flags);

  /* Bitwise OR our capability flags in */
  if (ret_value >= 0)
    *cap_flags |= H5VL_pass_through_ext_g.cap_flags;

  return ret_value;
} /* end H5VL_pass_through_introspect_ext_get_cap_flags() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_pass_through_ext_introspect_opt_query
 *
 * Purpose:     Query if an optional operation is supported by this connector
 *
 * Return:      SUCCEED / FAIL
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_pass_through_ext_introspect_opt_query(void* obj,
                                                  H5VL_subclass_t cls,
                                                  int op_type, uint64_t* flags)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL INTROSPECT OptQuery\n");
#endif

  ret_value = H5VLintrospect_opt_query(o->under_object, o->under_vol_id, cls,
                                       op_type, flags);

  return ret_value;
} /* end H5VL_pass_through_ext_introspect_opt_query() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_pass_through_ext_blob_put
 *
 * Purpose:     Handles the blob 'put' callback
 *
 * Return:      SUCCEED / FAIL
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_pass_through_ext_blob_put(void* obj, const void* buf, size_t size,
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
} /* end H5VL_pass_through_ext_blob_put() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_pass_through_ext_blob_get
 *
 * Purpose:     Handles the blob 'get' callback
 *
 * Return:      SUCCEED / FAIL
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_pass_through_ext_blob_get(void* obj, const void* blob_id, void* buf,
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
} /* end H5VL_pass_through_ext_blob_get() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_pass_through_ext_blob_specific
 *
 * Purpose:     Handles the blob 'specific' callback
 *
 * Return:      SUCCEED / FAIL
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_pass_through_ext_blob_specific(void* obj, void* blob_id,
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
} /* end H5VL_pass_through_ext_blob_specific() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_pass_through_ext_blob_optional
 *
 * Purpose:     Handles the blob 'optional' callback
 *
 * Return:      SUCCEED / FAIL
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_pass_through_ext_blob_optional(void* obj, void* blob_id,
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
} /* end H5VL_pass_through_ext_blob_optional() */

/*---------------------------------------------------------------------------
 * Function:    H5VL_pass_through_ext_token_cmp
 *
 * Purpose:     Compare two of the connector's object tokens, setting
 *              *cmp_value, following the same rules as strcmp().
 *
 * Return:      Success:    0
 *              Failure:    -1
 *
 *---------------------------------------------------------------------------
 */
static herr_t H5VL_pass_through_ext_token_cmp(void* obj,
                                              const H5O_token_t* token1,
                                              const H5O_token_t* token2,
                                              int* cmp_value)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL TOKEN Compare\n");
#endif

  /* Sanity checks */
  assert(obj);
  assert(token1);
  assert(token2);
  assert(cmp_value);

  ret_value = H5VLtoken_cmp(o->under_object, o->under_vol_id, token1, token2,
                            cmp_value);

  return ret_value;
} /* end H5VL_pass_through_ext_token_cmp() */

/*---------------------------------------------------------------------------
 * Function:    H5VL_pass_through_ext_token_to_str
 *
 * Purpose:     Serialize the connector's object token into a string.
 *
 * Return:      Success:    0
 *              Failure:    -1
 *
 *---------------------------------------------------------------------------
 */
static herr_t H5VL_pass_through_ext_token_to_str(void* obj, H5I_type_t obj_type,
                                                 const H5O_token_t* token,
                                                 char** token_str)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL TOKEN To string\n");
#endif

  /* Sanity checks */
  assert(obj);
  assert(token);
  assert(token_str);

  ret_value = H5VLtoken_to_str(o->under_object, obj_type, o->under_vol_id,
                               token, token_str);

  return ret_value;
} /* end H5VL_pass_through_ext_token_to_str() */

/*---------------------------------------------------------------------------
 * Function:    H5VL_pass_through_ext_token_from_str
 *
 * Purpose:     Deserialize the connector's object token from a string.
 *
 * Return:      Success:    0
 *              Failure:    -1
 *
 *---------------------------------------------------------------------------
 */
static herr_t H5VL_pass_through_ext_token_from_str(void* obj,
                                                   H5I_type_t obj_type,
                                                   const char* token_str,
                                                   H5O_token_t* token)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL TOKEN From string\n");
#endif

  /* Sanity checks */
  assert(obj);
  assert(token);
  assert(token_str);

  ret_value = H5VLtoken_from_str(o->under_object, obj_type, o->under_vol_id,
                                 token_str, token);

  return ret_value;
} /* end H5VL_pass_through_ext_token_from_str() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_pass_through_ext_optional
 *
 * Purpose:     Handles the generic 'optional' callback
 *
 * Return:      SUCCEED / FAIL
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_pass_through_ext_optional(void* obj, H5VL_optional_args_t* args,
                                      hid_t dxpl_id, void** req)
{
  H5VL_as_rpc_t* o = (H5VL_as_rpc_t*)obj;
  herr_t ret_value;

#ifdef ENABLE_EXT_PASSTHRU_LOGGING
  printf("------- EXT PASS THROUGH VOL generic Optional\n");
#endif

  ret_value =
      H5VLoptional(o->under_object, o->under_vol_id, args, dxpl_id, req);

  return ret_value;
} /* end H5VL_pass_through_ext_optional() */

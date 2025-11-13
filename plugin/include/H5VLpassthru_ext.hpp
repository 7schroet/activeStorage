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

/*
 * Purpose:	The public header file for the pass-through VOL connector.
 */

#ifndef _H5VLpassthru_ext_H
#define _H5VLpassthru_ext_H

/* Public headers needed by this file */
#include "H5VLpublic.h" /* Virtual Object Layer                 */

/* Identifier for the pass-through VOL connector */
#define H5VL_PASSTHRU_EXT (H5VL_pass_through_ext_register())

/* Public characteristics of the pass-through VOL connector */
#define H5VL_PASSTHRU_EXT_NAME "pass_through_ext"
#define H5VL_PASSTHRU_EXT_VALUE 517 /* VOL connector ID */
#define H5VL_PASSTHRU_EXT_VERSION 0

/* Pass-through VOL connector info */
typedef struct H5VL_as_rpc_info_t
{
  hid_t under_vol_id;   /* VOL ID for under VOL */
  void* under_vol_info; /* VOL info for under VOL */
} H5VL_as_rpc_info_t;

#ifdef __cplusplus
extern "C"
{
#endif

  /* Technically a private function call, but prototype must be declared here */
  extern hid_t H5VL_pass_through_ext_register(void);

#ifdef __cplusplus
}
#endif

#endif /* _H5VLpassthru_H */

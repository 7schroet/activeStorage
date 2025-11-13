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
 * Purpose:	The private header file for the pass-through VOL connector.
 */

#ifndef _H5VLpassthru_ext_private_H
#define _H5VLpassthru_ext_private_H

/* Public headers needed by this file */
#include "H5VLpassthru_ext.hpp" /* Public header for connector */

/* Private characteristics of the pass-through VOL connector */
#define H5VL_PASSTHRU_EXT_VERSION 0

/* Names for dynamically registered operations */
#define H5VL_PASSTHRU_EXT_DYN_FOO "org.example.ext_passthru.foo"
#define H5VL_PASSTHRU_EXT_DYN_BAR "org.example.ext_passthru.bar"
#define H5VL_PASSTHRU_EXT_DYN_FIDDLE "org.example.ext_passthru.fiddle"

/* Parameters for each of the dynamically registered operations */

/* H5VL_PASSTHRU_EXT_DYN_FOO */
typedef struct H5VL_passthru_ext_dataset_foo_args_t
{
  int i;
  double d;
} H5VL_passthru_ext_dataset_foo_args_t;

/* H5VL_PASSTHRU_EXT_DYN_BAR */
typedef struct H5VL_passthru_ext_dataset_bar_args_t
{
  double* dp;
  unsigned* up;
} H5VL_passthru_ext_dataset_bar_args_t;

/* H5VL_PASSTHRU_EXT_DYN_FIDDLE */
/* No args */

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#endif /* _H5VLpassthru_ext_private_H */

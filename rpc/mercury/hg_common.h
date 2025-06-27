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

#ifndef HG_COMMON_H
#define HG_COMMON_H

#include "rpc.h"
#include <mercury.h>

#define HG_RETURN_CHECK(func)                                                  \
  do                                                                           \
  {                                                                            \
    hg_return_t ret = (func);                                                  \
    if (ret != HG_SUCCESS)                                                     \
    {                                                                          \
      fprintf(stderr, "Failure in HG function in file %s at line %d\n",        \
              __FILE__, __LINE__);                                             \
      fprintf(stderr, "Function call: %s\n", #func);                           \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

typedef struct rpc_handle
{
  hg_class_t* class;
  hg_context_t* context;
} rpc_handle;

#endif

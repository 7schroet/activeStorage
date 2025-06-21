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

#include <mercury.h>
#include <stdio.h>
#include <stdlib.h>

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

void setup(hg_class_t** class, hg_context_t** context)
{
  *class = HG_Init("tcp://:12345", HG_TRUE);
  *context = HG_Context_create(*class);
}

void print_address(hg_class_t* class)
{
  hg_addr_t addr;
  HG_RETURN_CHECK(HG_Addr_self(class, &addr));

  hg_size_t buf_size = 128;
  char buf[buf_size];
  HG_RETURN_CHECK(HG_Addr_to_string(class, buf, &buf_size, addr));

  printf("Server address is: %s\n", buf);

  HG_RETURN_CHECK(HG_Addr_free(class, addr));
}

int main(void)
{
  hg_class_t* class = NULL;
  hg_context_t* context = NULL;

  setup(&class, &context);
  print_address(class);

  HG_RETURN_CHECK(HG_Context_destroy(context));
  HG_RETURN_CHECK(HG_Finalize(class));

  return EXIT_SUCCESS;
}

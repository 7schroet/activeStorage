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

#include "file_writer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void write_to_file(char* content, char* filename)
{
  assert(filename);
  assert(content);

  FILE* fd = fopen(filename, "w");
  if (!fd)
  {
    fprintf(stderr, "fopen failed for %s, aborting!\n", filename);
    exit(EXIT_FAILURE);
  }

  unsigned long length = strlen(content);
  size_t ret = fwrite(content, sizeof(char), length, fd);
  if (ret != length)
  {
    fprintf(stderr,
            "Write to %s failed, wrote %zu, but expected %ld, aborting!\n",
            filename, ret, length);
    exit(EXIT_FAILURE);
  }

  fclose(fd);
}

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

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <setjmp.h>

#include <cmocka.h>

#include "../argparse.c"

static void null_test(void** state) { (void)state; }

int main(void)
{
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(null_test),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

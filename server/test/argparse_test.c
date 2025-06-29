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

static int reset_getopt(void** state)
{
  optind = 1;
  return 0;
}

static void assert_config_equal(config* expected, config* actual)
{
  assert_string_equal(expected->port, actual->port);
  assert_string_equal(expected->address_file, actual->address_file);
  assert_int_equal(expected->protocol, actual->protocol);
}

static void default_test(void** state)
{
  config expected = {
      .address_file = "servername",
      .port = "8080",
      .protocol = tcp,
  };

  char* argv[1] = {"as-server"};
  config actual = parse_args(1, argv);
  assert_config_equal(&expected, &actual);
}

static void pass_port_test(void** state)
{
  config expected = {
      .address_file = "servername",
      .port = "12345",
      .protocol = tcp,
  };

  char* argv[3] = {"as-server", "--port", "12345"};
  config actual = parse_args(3, argv);
  assert_config_equal(&expected, &actual);
}

static void pass_protocol_test(void** state)
{
  config expected = {
      .address_file = "servername",
      .port = "8080",
      .protocol = verbs,
  };

  char* argv[3] = {"as-server", "--protocol", "verbs"};
  config actual = parse_args(3, argv);
  assert_config_equal(&expected, &actual);
}

static void pass_address_file_test(void** state)
{
  config expected = {
      .address_file = "../filename",
      .port = "8080",
      .protocol = tcp,
  };

  char* argv[3] = {"as-server", "--addressfile", "../filename"};
  config actual = parse_args(3, argv);
  assert_config_equal(&expected, &actual);
}

static void pass_all_test(void** state)
{
  config expected = {
      .address_file = "file",
      .port = "6666",
      .protocol = tcp,
  };

  char* argv[7] = {"as-server", "--port",        "6666", "--protocol",
                   "tcp",       "--addressfile", "file"};
  config actual = parse_args(7, argv);
  assert_config_equal(&expected, &actual);
}

void __real_exit(int);

void __wrap_exit(int exit_code)
{
  function_called();
  /*__real_exit(0);*/
}

static void missing_arg_test(void** state)
{
  expect_function_call(__wrap_exit);

  char* argv[6] = {"as-server",  "--port", "6666",
                   "--protocol", "tcp",    "--addressfile"};
  config actual = parse_args(6, argv);
}

static void pass_help_test(void** state)
{
  expect_function_call(__wrap_exit);

  char* argv[2] = {"as-server", "--help"};
  config actual = parse_args(2, argv);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
      cmocka_unit_test_setup(default_test, reset_getopt),
      cmocka_unit_test_setup(pass_port_test, reset_getopt),
      cmocka_unit_test_setup(pass_protocol_test, reset_getopt),
      cmocka_unit_test_setup(pass_address_file_test, reset_getopt),
      cmocka_unit_test_setup(pass_all_test, reset_getopt),
      cmocka_unit_test_setup(missing_arg_test, reset_getopt),
      cmocka_unit_test_setup(pass_help_test, reset_getopt),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

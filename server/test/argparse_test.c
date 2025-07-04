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

#include <check.h>
#include <stdlib.h>

#include "../argparse.c"

void reset_getopt(void) { optind = 1; }

static void assert_config_equal(config* expected, config* actual)
{
  ck_assert_str_eq(expected->port, actual->port);
  ck_assert_str_eq(expected->address_file, actual->address_file);
  ck_assert_int_eq(expected->protocol, actual->protocol);
}

START_TEST(test_default)
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
END_TEST

START_TEST(test_pass_port)
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
END_TEST

START_TEST(test_pass_protocol)
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
END_TEST

START_TEST(test_pass_address)
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
END_TEST

START_TEST(test_pass_all)
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
END_TEST

START_TEST(test_help)
{
  char* argv[2] = {"as-server", "--help"};
  parse_args(2, argv);
}
END_TEST

START_TEST(test_missing_arg)
{
  char* argv[6] = {"as-server",  "--port", "6666",
                   "--protocol", "tcp",    "--addressfile"};
  parse_args(6, argv);
}
END_TEST

START_TEST(test_unknown_protocol)
{
  char* argv[3] = {"as-server", "--protocol", "unknown"};
  parse_args(3, argv);
}
END_TEST

Suite* parser_suite(void)
{
  Suite* s = suite_create("Server Arg Parsing");
  TCase* tc_success = tcase_create("Sucessful parses");
  TCase* tc_failure = tcase_create("Failing parses");

  tcase_add_checked_fixture(tc_success, NULL, reset_getopt);
  tcase_add_test(tc_success, test_default);
  tcase_add_test(tc_success, test_pass_port);
  tcase_add_test(tc_success, test_pass_protocol);
  tcase_add_test(tc_success, test_pass_address);
  tcase_add_test(tc_success, test_pass_all);
  tcase_add_exit_test(tc_success, test_help, 0);

  tcase_add_checked_fixture(tc_failure, NULL, reset_getopt);
  tcase_add_exit_test(tc_failure, test_missing_arg, 1);
  tcase_add_exit_test(tc_failure, test_unknown_protocol, 1);

  suite_add_tcase(s, tc_success);
  suite_add_tcase(s, tc_failure);
  return s;
}

int main(void)
{
  Suite* parse = parser_suite();

  SRunner* sr = srunner_create(parse);
  srunner_set_tap(sr, "-");

  srunner_run_all(sr, CK_SILENT);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

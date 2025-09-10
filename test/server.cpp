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

#include <iostream>
#include <thallium.hpp>

namespace tl = thallium;

void sum(const tl::request& req, int x, int y)
{
  std::cout << "Computing " << x << "+" << y << std::endl;
}

int main(int argc, char** argv)
{

  tl::engine myEngine("tcp://127.0.0.1:1234", THALLIUM_SERVER_MODE);
  std::cout << "Server running at address " << myEngine.self() << std::endl;
  myEngine.define("sum", sum).disable_response();

  myEngine.wait_for_finalize();

  return 0;
}

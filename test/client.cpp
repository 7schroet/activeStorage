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

#include "test.hpp"

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <address>" << std::endl;
    exit(0);
  }
  tl::engine myEngine("tcp", THALLIUM_CLIENT_MODE);
  /*tl::remote_procedure sum = myEngine.define("sum").disable_response();*/
  auto a = get(myEngine);
  auto found = a.find(0);
  tl::endpoint server = myEngine.lookup(argv[1]);
  found->second.on(server)(42, 63);

  return 0;
}

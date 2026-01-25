/*
 * Copyright (c) 2025 - 2026 Niclas Schroeter
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

#ifndef TL_KERNEL_IMPL_HPP
#define TL_KERNEL_IMPL_HPP

#include <string>
#include <thallium.hpp>
#include <thallium/serialization/stl/string.hpp>
#include <thallium/serialization/stl/vector.hpp>

namespace as_rpc::kernel_impl
{
void hello(const thallium::request& req);
void mean(const thallium::request& req, const std::string& infile,
          const std::string& outfile, const std::string& dataset,
          unsigned timestep, const std::vector<char>& reduce_along_dim);
} // namespace as_rpc::kernel_impl

#endif

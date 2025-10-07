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

#include "tl_kernel_impl.hpp"
#include "as_rpc_kernel_math.hpp"
#include "h5_helpers.hpp"
#include <H5Cpp.h>
#include <algorithm>
#include <filesystem>
#include <print>
#include <string>
#include <thallium.hpp>
#include <tuple>
#include <vector>

namespace
{
std::string generate_result_filename(const std::string& filename,
                                     const std::string& dataset,
                                     const std::string& op)
{
  std::string stripped_dataset(dataset);
  std::replace(stripped_dataset.begin(), stripped_dataset.end(), '/', '_');
  std::filesystem::path basename{filename};
  return "asrpc_results_" + basename.stem().string() + "_" + stripped_dataset +
         "_" + op + ".h5";
}
} // namespace

namespace as_rpc
{
namespace kernel_impl
{

void hello([[maybe_unused]] const thallium::request& req)
{
  std::println("Received RPC");
}

void mean([[maybe_unused]] const thallium::request& req, std::string filename,
          std::string dataset, int timestep, std::vector<char> reduce_along_dim)
{
  H5::H5File file{filename, H5F_ACC_RDONLY | H5F_ACC_SWMR_READ};
  auto dset = file.openDataSet(dataset);
  auto dtype = h5::determine_datatype(dset);
  std::vector<double> avg;
  std::vector<hsize_t> avg_dims;

  if (dtype == H5::PredType::NATIVE_DOUBLE)
  {
    const auto [data, dims] = h5::read_data<double>(dset, timestep);
    std::tie(avg, avg_dims) = mean_reduction(data, dims, reduce_along_dim);
  }
  else if (dtype == H5::PredType::NATIVE_FLOAT)
  {
    const auto [data, dims] = h5::read_data<float>(dset, timestep);
    std::tie(avg, avg_dims) = mean_reduction(data, dims, reduce_along_dim);
  }
  else
  {
    const auto [data, dims] = h5::read_data<int>(dset, timestep);
    std::tie(avg, avg_dims) = mean_reduction(data, dims, reduce_along_dim);
  }

  std::string result_filename =
      generate_result_filename(filename, dataset, "mean");
  h5::write_data(avg, avg_dims, timestep, result_filename);

  dset.close();
  file.close();
}
} // namespace kernel_impl
} // namespace as_rpc

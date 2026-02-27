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

#include "tl_kernel_impl.hpp"
#include "as_rpc_kernel_math.hpp"
#include "h5_helpers.hpp"
#include <H5Cpp.h>
#include <print>
#include <string>
#include <thallium.hpp>
#include <tuple>
#include <vector>

namespace
{
std::pair<std::vector<double>, std::vector<hsize_t>>
apply_mean(const H5::PredType& dtype, const H5::DataSet& dset,
           unsigned timestep, const std::vector<char>& reduction_dims)
{
  if (dtype == H5::PredType::NATIVE_DOUBLE)
  {
    const auto [data, dims] = as_rpc::h5::read_data<double>(dset, timestep);
    return as_rpc::kernel_impl::reduction_operation(data, dims, reduction_dims);
  }
  else if (dtype == H5::PredType::NATIVE_FLOAT)
  {
    const auto [data, dims] = as_rpc::h5::read_data<float>(dset, timestep);
    return as_rpc::kernel_impl::reduction_operation(data, dims, reduction_dims);
  }
  else
  {
    const auto [data, dims] = as_rpc::h5::read_data<int>(dset, timestep);
    return as_rpc::kernel_impl::reduction_operation(data, dims, reduction_dims);
  }
}
} // namespace

namespace as_rpc::kernel_impl
{

void hello([[maybe_unused]] const thallium::request& req)
{
  std::println("Received RPC");
}

void mean([[maybe_unused]] const thallium::request& req,
          const std::string& infile, const std::string& outfile,
          const std::string& dataset, unsigned timestep,
          const std::vector<char>& reduce_along_dim)
{
  H5::H5File file{infile, H5F_ACC_RDONLY};
  auto dset = file.openDataSet(dataset);
  auto dtype = h5::determine_datatype(dset);

  std::vector<double> avg;
  std::vector<hsize_t> avg_dims;
  const std::vector<char> reduction_dims_without_time{
      reduce_along_dim.begin() + 1, reduce_along_dim.end()};

  std::tie(avg, avg_dims) =
      apply_mean(dtype, dset, timestep, reduction_dims_without_time);

  if (reduce_along_dim[0] == 0 || timestep == 0)
  {
    h5::write_data(avg, avg_dims, timestep, outfile);
  }
  else
  {
    H5::H5File tmp{outfile, H5F_ACC_RDONLY};
    auto result_dset = tmp.openDataSet("result");
    const auto [prev_avg, _] = h5::read_data<double>(result_dset, timestep - 1);
    result_dset.close();
    tmp.close();
    const auto running_avg = running_mean(avg, prev_avg, timestep);
    h5::write_data(running_avg, avg_dims, timestep, outfile);
  }

  dset.close();
  file.close();
}
} // namespace as_rpc::kernel_impl

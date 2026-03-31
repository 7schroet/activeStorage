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
#include "as_rpc_kernels.hpp"
#include "h5_helpers.hpp"
#include <H5Cpp.h>
#include <print>
#include <string>
#include <thallium.hpp>
#include <vector>

namespace
{
template <typename T>
void apply_mean(const std::vector<T>& data, const std::vector<hsize_t>& dims,
                const std::string& outfile, unsigned timestep,
                const std::vector<char>& reduce_along_dim)
{
  const std::vector<char> reduction_dims_without_time{
      reduce_along_dim.begin() + 1, reduce_along_dim.end()};

  const auto [avg, avg_dims] = as_rpc::kernel_impl::reduction_operation(
      data, dims, reduction_dims_without_time);

  if (reduce_along_dim[0] == 0 || timestep == 0)
  {
    as_rpc::h5::write_data(avg, avg_dims, timestep, outfile);
  }
  else
  {
    H5::H5File tmp{outfile, H5F_ACC_RDONLY};
    auto result_dset = tmp.openDataSet("result");
    const auto [prev_avg, _] =
        as_rpc::h5::read_data<double>(result_dset, timestep - 1);
    result_dset.close();
    tmp.close();
    const auto running_avg =
        as_rpc::kernel_impl::running_mean(avg, prev_avg, timestep);
    as_rpc::h5::write_data(running_avg, avg_dims, timestep, outfile);
  }
}

template <typename T, auto GlobalFunc, auto ElementFunc>
void apply_comparison(const std::vector<T>& data,
                      const std::vector<hsize_t>& dims,
                      const std::string& outfile, unsigned timestep,
                      const std::vector<char>& reduce_along_dim)
{
  const std::vector<char> reduction_dims_without_time{
      reduce_along_dim.begin() + 1, reduce_along_dim.end()};

  const auto [result_data, result_dims] =
      as_rpc::kernel_impl::reduction_operation<T, T, GlobalFunc, ElementFunc,
                                               false>(
          data, dims, reduction_dims_without_time);

  if (reduce_along_dim[0] == 0 || timestep == 0)
  {
    as_rpc::h5::write_data<T>(result_data, result_dims, timestep, outfile);
  }
  else
  {
    H5::H5File tmp{outfile, H5F_ACC_RDONLY};
    auto result_dset = tmp.openDataSet("result");
    const auto [prev_result, _] =
        as_rpc::h5::read_data<T>(result_dset, timestep - 1);
    result_dset.close();
    tmp.close();
    const auto running_result =
        as_rpc::kernel_impl::running_reduction<T, ElementFunc>(result_data,
                                                               prev_result);
    as_rpc::h5::write_data<T>(running_result, result_dims, timestep, outfile);
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

  if (dtype == H5::PredType::NATIVE_DOUBLE)
  {
    const auto [data, dims] = as_rpc::h5::read_data<double>(dset, timestep);
    apply_mean(data, dims, outfile, timestep, reduce_along_dim);
  }
  else if (dtype == H5::PredType::NATIVE_FLOAT)
  {
    const auto [data, dims] = as_rpc::h5::read_data<float>(dset, timestep);
    apply_mean(data, dims, outfile, timestep, reduce_along_dim);
  }
  else
  {
    const auto [data, dims] = as_rpc::h5::read_data<int>(dset, timestep);
    apply_mean(data, dims, outfile, timestep, reduce_along_dim);
  }

  dset.close();
  file.close();
}

void max([[maybe_unused]] const thallium::request& req,
         const std::string& infile, const std::string& outfile,
         const std::string& dataset, unsigned timestep,
         const std::vector<char>& reduce_along_dim)
{
  H5::H5File file{infile, H5F_ACC_RDONLY};
  auto dset = file.openDataSet(dataset);
  auto dtype = h5::determine_datatype(dset);

  if (dtype == H5::PredType::NATIVE_DOUBLE)
  {
    const auto [data, dims] = as_rpc::h5::read_data<double>(dset, timestep);
    apply_comparison<double, global_max, elementwise_max>(
        data, dims, outfile, timestep, reduce_along_dim);
  }
  else if (dtype == H5::PredType::NATIVE_FLOAT)
  {
    const auto [data, dims] = as_rpc::h5::read_data<float>(dset, timestep);
    apply_comparison<float, global_max, elementwise_max>(
        data, dims, outfile, timestep, reduce_along_dim);
  }
  else
  {
    const auto [data, dims] = as_rpc::h5::read_data<int>(dset, timestep);
    apply_comparison<int, global_max, elementwise_max>(
        data, dims, outfile, timestep, reduce_along_dim);
  }

  dset.close();
  file.close();
}

void min([[maybe_unused]] const thallium::request& req,
         const std::string& infile, const std::string& outfile,
         const std::string& dataset, unsigned timestep,
         const std::vector<char>& reduce_along_dim)
{
  H5::H5File file{infile, H5F_ACC_RDONLY};
  auto dset = file.openDataSet(dataset);
  auto dtype = h5::determine_datatype(dset);

  if (dtype == H5::PredType::NATIVE_DOUBLE)
  {
    const auto [data, dims] = as_rpc::h5::read_data<double>(dset, timestep);
    apply_comparison<double, global_min, elementwise_min>(
        data, dims, outfile, timestep, reduce_along_dim);
  }
  else if (dtype == H5::PredType::NATIVE_FLOAT)
  {
    const auto [data, dims] = as_rpc::h5::read_data<float>(dset, timestep);
    apply_comparison<float, global_min, elementwise_min>(
        data, dims, outfile, timestep, reduce_along_dim);
  }
  else
  {
    const auto [data, dims] = as_rpc::h5::read_data<int>(dset, timestep);
    apply_comparison<int, global_min, elementwise_min>(
        data, dims, outfile, timestep, reduce_along_dim);
  }

  dset.close();
  file.close();
}

void analyse_doubles([[maybe_unused]] const thallium::request& req,
                     const std::vector<double>& data,
                     const std::vector<hsize_t>& dims,
                     const std::string& outfile, unsigned timestep,
                     const std::vector<char>& reduce_along_dim,
                     std::uint8_t operation)
{
  const Kernel op{operation};
  switch (op)
  {
  case Kernel::mean:
    apply_mean(data, dims, outfile, timestep, reduce_along_dim);
    break;
  case Kernel::min:
    apply_comparison<double, global_min, elementwise_min>(
        data, dims, outfile, timestep, reduce_along_dim);
    break;
  case Kernel::max:
    apply_comparison<double, global_max, elementwise_max>(
        data, dims, outfile, timestep, reduce_along_dim);
    break;
  default:
    std::println("Operation {} is not valid input for {}!", operation,
                 __func__);
  }
}

void analyse_floats([[maybe_unused]] const thallium::request& req,
                    const std::vector<float>& data,
                    const std::vector<hsize_t>& dims,
                    const std::string& outfile, unsigned timestep,
                    const std::vector<char>& reduce_along_dim,
                    std::uint8_t operation)
{
  const Kernel op{operation};
  switch (op)
  {
  case Kernel::mean:
    apply_mean(data, dims, outfile, timestep, reduce_along_dim);
    break;
  case Kernel::min:
    apply_comparison<float, global_min, elementwise_min>(
        data, dims, outfile, timestep, reduce_along_dim);
    break;
  case Kernel::max:
    apply_comparison<float, global_max, elementwise_max>(
        data, dims, outfile, timestep, reduce_along_dim);
    break;
  default:
    std::println("Operation {} is not valid input for {}!", operation,
                 __func__);
  }
}

void analyse_ints([[maybe_unused]] const thallium::request& req,
                  const std::vector<int>& data,
                  const std::vector<hsize_t>& dims, const std::string& outfile,
                  unsigned timestep, const std::vector<char>& reduce_along_dim,
                  std::uint8_t operation)
{
  const Kernel op{operation};
  switch (op)
  {
  case Kernel::mean:
    apply_mean(data, dims, outfile, timestep, reduce_along_dim);
    break;
  case Kernel::min:
    apply_comparison<int, global_min, elementwise_min>(
        data, dims, outfile, timestep, reduce_along_dim);
    break;
  case Kernel::max:
    apply_comparison<int, global_max, elementwise_max>(
        data, dims, outfile, timestep, reduce_along_dim);
    break;
  default:
    std::println("Operation {} is not valid input for {}!", operation,
                 __func__);
  }
}
} // namespace as_rpc::kernel_impl

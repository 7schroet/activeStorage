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

#include "argparse.hpp"
#include "as_rpc.hpp"
#include <array>
#include <cstdlib>
#include <hdf5.h>
#include <print>
#include <random>
#include <string>
#include <vector>

#define H5ERROR_CHECK(func)                                                    \
  do                                                                           \
  {                                                                            \
    const herr_t err = (func);                                                 \
    if (err < 0)                                                               \
    {                                                                          \
      std::println(stderr,                                                     \
                   "Error in HDF5 function call at {} in {}, aborting",        \
                   __LINE__, __FILE__);                                        \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

#define FILE_NAME "file.h5"
#define DSET_NAME "/dataset"
#define RANK 3
#define DSET_X 10
#define DSET_Y 10

namespace
{
void close_file(hid_t file) { H5ERROR_CHECK(H5Fclose(file)); }

template <typename T>
hid_t create_file()
{
  hid_t file = H5Fcreate(FILE_NAME, H5F_ACC_TRUNC | H5F_ACC_SWMR_WRITE,
                         H5P_DEFAULT, H5P_DEFAULT);

  constexpr std::array<hsize_t, RANK> dims = {1, DSET_X, DSET_Y};
  constexpr std::array<hsize_t, RANK> max_dims = {H5S_UNLIMITED, DSET_X,
                                                  DSET_Y};

  auto dcpl = H5Pcreate(H5P_DATASET_CREATE);
  H5ERROR_CHECK(H5Pset_chunk(dcpl, RANK, dims.data()));

  const hid_t dtype = []
  {
    if constexpr (std::is_same_v<T, double>)
      return H5T_NATIVE_DOUBLE;
    else if constexpr (std::is_same_v<T, float>)
      return H5T_NATIVE_FLOAT;
    else if constexpr (std::is_same_v<T, int>)
      return H5T_NATIVE_INT;
  }();

  auto dspace = H5Screate_simple(RANK, dims.data(), max_dims.data());
  auto dset =
      H5Dcreate(file, DSET_NAME, dtype, dspace, H5P_DEFAULT, dcpl, H5P_DEFAULT);

  H5ERROR_CHECK(H5Pclose(dcpl));
  H5ERROR_CHECK(H5Dclose(dset));
  H5ERROR_CHECK(H5Sclose(dspace));

  // close and reopen to enable swmr
  close_file(file);
  file = H5Fopen(FILE_NAME, H5F_ACC_RDWR | H5F_ACC_SWMR_WRITE, H5P_DEFAULT);
  return file;
}

template <typename T>
std::vector<T> generate_data(unsigned base_value, bool randomize)
{
  std::vector<T> result(DSET_X * DSET_Y);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<T> dis(0.0, 1.0);
  for (auto i = 0u; i < DSET_X; i++)
  {
    for (auto j = 0u; j < DSET_Y; j++)
    {
      const T random_value = randomize ? dis(gen) : static_cast<T>(0.5) * j;
      result[i * DSET_Y + j] = static_cast<T>(1.0) * base_value +
                               static_cast<T>(10.0) + random_value;
    }
  }
  return result;
}

template <>
std::vector<int> generate_data<int>(unsigned base_value,
                                    [[maybe_unused]] bool randomize)
{
  std::vector<int> result(DSET_X * DSET_Y);
  for (auto i = 0u; i < DSET_X; i++)
  {
    for (auto j = 0u; j < DSET_Y; j++)
    {
      result[i * DSET_Y + j] = base_value + 10 + j;
    }
  }
  return result;
}

template <typename T>
void add_timestep(hid_t file, bool randomize)
{
  static int current_timestep = 0;
  auto dset = H5Dopen2(file, DSET_NAME, H5P_DEFAULT);
  H5ERROR_CHECK(dset);
  auto dspace = H5Dget_space(dset);
  H5ERROR_CHECK(dspace);

  if (current_timestep != 0)
  {
    std::array<hsize_t, RANK> dims{};
    H5ERROR_CHECK(H5Sget_simple_extent_dims(dspace, dims.data(), nullptr));
    dims[0]++;
    H5ERROR_CHECK(H5Dset_extent(dset, dims.data()));
    // dspace must be reopened according to docs
    H5ERROR_CHECK(H5Sclose(dspace));
    dspace = H5Dget_space(dset);
    H5ERROR_CHECK(dspace);
  }

  constexpr std::array<hsize_t, RANK> count = {1, DSET_X, DSET_Y};
  const std::array<hsize_t, RANK> offset = {
      static_cast<hsize_t>(current_timestep), 0, 0};
  H5ERROR_CHECK(H5Sselect_hyperslab(dspace, H5S_SELECT_SET, offset.data(),
                                    nullptr, count.data(), nullptr));

  auto data = generate_data<T>(offset[0], randomize);

  const hid_t dtype = []
  {
    if constexpr (std::is_same_v<T, double>)
      return H5T_NATIVE_DOUBLE;
    else if constexpr (std::is_same_v<T, float>)
      return H5T_NATIVE_FLOAT;
    else if constexpr (std::is_same_v<T, int>)
      return H5T_NATIVE_INT;
  }();

  auto memspace = H5Screate_simple(RANK, count.data(), nullptr);
  H5ERROR_CHECK(memspace);

  H5ERROR_CHECK(
      H5Dwrite(dset, dtype, memspace, dspace, H5P_DEFAULT, data.data()));
  H5ERROR_CHECK(H5Dflush(dset));

  H5ERROR_CHECK(H5Sclose(dspace));
  H5ERROR_CHECK(H5Sclose(memspace));
  H5ERROR_CHECK(H5Dclose(dset));
  current_timestep++;
}
} // namespace

int main(int argc, char** argv)
{
  const ClientConfig config = parse_args(argc, argv);
  const std::string client_address =
      as_rpc::protocol_to_string(config.protocol);

  as_rpc::Engine engine = as_rpc::init_engine(client_address, false);
  const as_rpc::RemoteProcedures rpc_kernels =
      as_rpc::register_kernels_at_client(engine);

  const std::string server_address =
      as_rpc::get_address_from_file(config.server_address_file);
  const as_rpc::ServerEndpoint server =
      as_rpc::connect_to_server(engine, server_address);

  auto search = rpc_kernels.find(as_rpc::Kernel::hello);
  if (search != rpc_kernels.end())
  {
    search->second.on(server)();
  }

  search = rpc_kernels.find(as_rpc::Kernel::mean);
  if (search == rpc_kernels.end())
  {
    std::println(stderr, "Couldn't find mean RPC!");
    exit(1);
  }

  const hid_t file = [&]
  {
    if (config.value_type == Datatype::DOUBLE)
      return create_file<double>();
    else if (config.value_type == Datatype::FLOAT)
      return create_file<float>();
    else
      return create_file<int>();
  }();

  const std::string filename{FILE_NAME};
  const std::string dset_name{DSET_NAME};
  std::println("Press Enter to write a new time step, or Ctrl+D to terminate");
  auto count = 0;
  for (std::string in; std::getline(std::cin, in);)
  {
    if (config.value_type == Datatype::DOUBLE)
      add_timestep<double>(file, config.randomize_data);
    else if (config.value_type == Datatype::FLOAT)
      add_timestep<float>(file, config.randomize_data);
    else
      add_timestep<int>(file, config.randomize_data);

    std::println("Appended time step {}", count);
    search->second.on(server)(filename, dset_name, count,
                              config.reduce_along_dim);
    count++;
  }
  std::println("Terminating...");
  close_file(file);
}

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
#include <cstdlib>
#include <hdf5.h>
#include <print>
#include <random>
#include <string>

#define H5ERROR_CHECK(func)                                                    \
  do                                                                           \
  {                                                                            \
    herr_t err = (func);                                                       \
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

void close_file(hid_t file) { H5ERROR_CHECK(H5Fclose(file)); }

hid_t create_file(void)
{
  hid_t file = H5Fcreate(FILE_NAME, H5F_ACC_TRUNC | H5F_ACC_SWMR_WRITE,
                         H5P_DEFAULT, H5P_DEFAULT);

  constexpr hsize_t dims[RANK] = {1, DSET_X, DSET_Y};
  constexpr hsize_t max_dims[RANK] = {H5S_UNLIMITED, DSET_X, DSET_Y};

  auto dcpl = H5Pcreate(H5P_DATASET_CREATE);
  H5ERROR_CHECK(H5Pset_chunk(dcpl, RANK, dims));

  auto dspace = H5Screate_simple(RANK, dims, max_dims);
  auto dset = H5Dcreate(file, DSET_NAME, H5T_NATIVE_DOUBLE, dspace, H5P_DEFAULT,
                        dcpl, H5P_DEFAULT);

  H5ERROR_CHECK(H5Pclose(dcpl));
  H5ERROR_CHECK(H5Dclose(dset));
  H5ERROR_CHECK(H5Sclose(dspace));

  // close and reopen to enable swmr
  close_file(file);
  file = H5Fopen(FILE_NAME, H5F_ACC_RDWR | H5F_ACC_SWMR_WRITE, H5P_DEFAULT);
  return file;
}

void add_timestep(hid_t file, bool randomize)
{
  static int current_timestep = 0;
  auto dset = H5Dopen2(file, DSET_NAME, H5P_DEFAULT);
  H5ERROR_CHECK(dset);
  auto dspace = H5Dget_space(dset);
  H5ERROR_CHECK(dspace);

  if (current_timestep != 0)
  {
    hsize_t dims[RANK];
    H5ERROR_CHECK(H5Sget_simple_extent_dims(dspace, dims, nullptr));
    dims[0]++;
    H5ERROR_CHECK(H5Dset_extent(dset, dims));
    // dspace must be reopened according to docs
    H5ERROR_CHECK(H5Sclose(dspace));
    dspace = H5Dget_space(dset);
    H5ERROR_CHECK(dspace);
  }

  constexpr hsize_t count[RANK] = {1, DSET_X, DSET_Y};
  hsize_t offset[RANK] = {0, 0, 0};
  offset[0] = current_timestep;
  H5ERROR_CHECK(H5Sselect_hyperslab(dspace, H5S_SELECT_SET, offset, nullptr,
                                    count, nullptr));

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dis(0.0, 1.0);
  double data[DSET_X * DSET_Y];
  for (auto i = 0; i < DSET_X; i++)
  {
    for (auto j = 0; j < DSET_Y; j++)
    {
      double random_value = 0.5 * j;
      if (randomize)
        random_value = dis(gen);

      data[i * DSET_Y + j] = 1.0 * offset[0] + 10.0 + random_value;
    }
  }

  auto memspace = H5Screate_simple(RANK, count, nullptr);
  H5ERROR_CHECK(memspace);

  H5ERROR_CHECK(
      H5Dwrite(dset, H5T_NATIVE_DOUBLE, memspace, dspace, H5P_DEFAULT, data));
  H5ERROR_CHECK(H5Dflush(dset));

  H5ERROR_CHECK(H5Sclose(dspace));
  H5ERROR_CHECK(H5Sclose(memspace));
  H5ERROR_CHECK(H5Dclose(dset));
  current_timestep++;
}

int main(int argc, char** argv)
{
  Config config = parse_args(argc, argv);
  const std::string client_address =
      as_rpc::protocol_to_string(config.protocol);

  as_rpc::Engine engine = as_rpc::init_engine(client_address, false);
  const as_rpc::RemoteProcedures rpc_kernels =
      as_rpc::register_kernels_at_client(engine);

  const std::string server_address =
      as_rpc::get_address_from_file(config.server_address_file);
  as_rpc::ServerEndpoint server =
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
  auto file = create_file();
  const std::string filename{FILE_NAME};
  const std::string dset_name{DSET_NAME};
  std::println("Press Enter to write a new time step, or Ctrl+D to terminate");
  auto count = 0;
  for (std::string in; std::getline(std::cin, in);)
  {
    add_timestep(file, config.randomize_data);
    std::println("Appended time step {}", count);
    search->second.on(server)(filename, dset_name, count,
                              config.reduce_along_dim);
    count++;
  }
  std::println("Terminating...");
  close_file(file);
}

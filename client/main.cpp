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
#include <iostream>

#define H5ERROR_CHECK(func)                                                    \
  do                                                                           \
  {                                                                            \
    herr_t err = (func);                                                       \
    if (err < 0)                                                               \
    {                                                                          \
      std::cout << "Error in HDF5 function call at " << __LINE__ << " in "     \
                << __FILE__ << ", aborting\n";                                 \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

void do_hdf5_stuff(void)
{
  hid_t file = H5Fcreate("file.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

  constexpr unsigned rank = 3;
  constexpr hsize_t dims[rank] = {1, 10, 10};
  constexpr hsize_t max_dims[rank] = {H5S_UNLIMITED, 10, 10};

  auto dcpl = H5Pcreate(H5P_DATASET_CREATE);
  H5ERROR_CHECK(H5Pset_chunk(dcpl, rank, dims));

  auto dspace = H5Screate_simple(rank, dims, max_dims);
  auto dset = H5Dcreate(file, "/dataset", H5T_NATIVE_DOUBLE, dspace,
                        H5P_DEFAULT, dcpl, H5P_DEFAULT);

  H5ERROR_CHECK(H5Pclose(dcpl));
  H5ERROR_CHECK(H5Dclose(dset));
  H5ERROR_CHECK(H5Sclose(dspace));
  H5ERROR_CHECK(H5Fclose(file));
}

int main(int argc, char** argv)
{
  Config config = parse_args(argc, argv);
  const std::string client_address =
      as_rpc::protocol_to_string(config.protocol);

  as_rpc::Engine engine = as_rpc::init_engine(client_address, false);
  as_rpc::RemoteProcedures rpc_kernels =
      as_rpc::register_kernels_at_client(engine);

  const std::string server_address =
      as_rpc::get_address_from_file(config.server_address_file);
  as_rpc::ServerEndpoint server =
      as_rpc::connect_to_server(engine, server_address);

  do_hdf5_stuff();
  auto search = rpc_kernels.find(as_rpc::Kernel::hello);
  if (search != rpc_kernels.end())
  {
    search->second.on(server)();
  }
}

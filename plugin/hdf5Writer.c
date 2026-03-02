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

#include <hdf5.h>
#include <stdio.h>
#include <stdlib.h>

#define H5ERROR_CHECK(func)                                                    \
  do                                                                           \
  {                                                                            \
    const herr_t err = (func);                                                 \
    if (err < 0)                                                               \
    {                                                                          \
      fprintf(stderr, "Error in HDF5 function call at %d in %s, aborting\n",   \
              __LINE__, __FILE__);                                             \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

#define FILE_NAME "dummyWrite.h5"
#define DSET_NAME "dummyDataset"
#define RANK 3
#define DSET_X 10
#define DSET_Y 10
#define TSTEP_INIT 10

int main()
{
  const hid_t file =
      H5Fcreate(FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

  constexpr hsize_t initial_dims[RANK] = {TSTEP_INIT, DSET_X, DSET_Y};
  constexpr hsize_t max_dims[RANK] = {H5S_UNLIMITED, DSET_X, DSET_Y};

  const hid_t dcpl = H5Pcreate(H5P_DATASET_CREATE);
  H5ERROR_CHECK(H5Pset_chunk(dcpl, RANK, initial_dims));

  hid_t dspace = H5Screate_simple(RANK, initial_dims, max_dims);
  const hid_t dset = H5Dcreate(file, DSET_NAME, H5T_NATIVE_DOUBLE, dspace,
                               H5P_DEFAULT, dcpl, H5P_DEFAULT);

  H5ERROR_CHECK(H5Dflush(dset));

  int current_timestep = 0;
  constexpr int bufsize = 256;
  char input[bufsize];

  printf("Press Enter to write a new time step, or Ctrl+D to terminate\n");
  while (fgets(input, bufsize, stdin))
  {
    if (current_timestep % TSTEP_INIT == 0 && current_timestep != 0)
    {
      hsize_t dims[RANK];
      H5ERROR_CHECK(H5Sget_simple_extent_dims(dspace, dims, nullptr));
      dims[0] += TSTEP_INIT;
      H5ERROR_CHECK(H5Dset_extent(dset, dims));
      // dspace must be reopened according to docs
      H5ERROR_CHECK(H5Sclose(dspace));
      dspace = H5Dget_space(dset);
      H5ERROR_CHECK(dspace);
    }
    constexpr hsize_t count[RANK] = {1, DSET_X, DSET_Y};
    const hsize_t offset[RANK] = {current_timestep, 0, 0};
    H5ERROR_CHECK(H5Sselect_hyperslab(dspace, H5S_SELECT_SET, offset, nullptr,
                                      count, nullptr));

    double data[DSET_X * DSET_Y];
    for (int i = 0; i < DSET_X; i++)
    {
      for (int j = 0; j < DSET_Y; j++)
        data[i * DSET_Y + j] = 1.0 * offset[0] + 10.0;
    }

    const hid_t memspace = H5Screate_simple(RANK, count, nullptr);
    H5ERROR_CHECK(memspace);

    H5ERROR_CHECK(
        H5Dwrite(dset, H5T_NATIVE_DOUBLE, memspace, dspace, H5P_DEFAULT, data));
    H5ERROR_CHECK(H5Fflush(file, H5F_SCOPE_LOCAL));

    H5ERROR_CHECK(H5Sclose(memspace));
    printf("Appended time step %d\n", current_timestep);
    current_timestep++;
  }

  printf("Terminating...\n");

  H5ERROR_CHECK(H5Pclose(dcpl));
  H5ERROR_CHECK(H5Dclose(dset));
  H5ERROR_CHECK(H5Sclose(dspace));
  H5ERROR_CHECK(H5Fclose(file));
}

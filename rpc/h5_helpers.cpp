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

#include "h5_helpers.hpp"
#include <numeric>
#include <vector>

std::vector<double> read_data(H5::DataSet dset, int timestep)
{
  auto dspace = dset.getSpace();
  auto rank = dspace.getSimpleExtentNdims();

  std::vector<hsize_t> dims(rank);
  std::vector<hsize_t> offset(rank, 0);
  std::vector<hsize_t> count(rank, 0);
  dspace.getSimpleExtentDims(dims.data());
  offset[0] = timestep;
  count[0] = 1;
  for (auto i = 1; i < rank; i++)
    count[i] = dims[i];

  dspace.selectHyperslab(H5S_SELECT_SET, count.data(), offset.data());
  H5::DataSpace memspace{rank, count.data()};

  auto elements =
      std::reduce(count.begin(), count.end(), 1, std::multiplies<>());
  std::vector<double> data(elements);
  dset.read(data.data(), H5::PredType::NATIVE_DOUBLE, memspace, dspace);

  memspace.close();
  dspace.close();

  return data;
}

void write_data(const std::vector<double>& data, const std::string& filename,
                const std::string& dataset, int timestep)
{
  return;
}

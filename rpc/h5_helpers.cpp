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
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace as_rpc::h5
{

const H5::PredType& determine_datatype(const H5::DataSet& dset)
{
  auto dataclass = dset.getTypeClass();
  if (dataclass == H5T_FLOAT)
  {
    auto float_type = dset.getFloatType();
    auto byte_size = float_type.getSize();

    if (byte_size == 4)
    {
      return H5::PredType::NATIVE_FLOAT;
    }
    else
    {
      return H5::PredType::NATIVE_DOUBLE;
    }
  }
  else if (dataclass == H5T_INTEGER)
  {
    return H5::PredType::NATIVE_INT;
  }
  else
  {
    throw std::runtime_error("Dataclass unknown");
  }
}

template <typename T>
std::pair<std::vector<T>, std::vector<hsize_t>>
read_data(const H5::DataSet& dset, int timestep)
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

  const H5::PredType dtype = []
  {
    if constexpr (std::is_same_v<T, double>)
      return H5::PredType::NATIVE_DOUBLE;
    else if constexpr (std::is_same_v<T, float>)
      return H5::PredType::NATIVE_FLOAT;
    else if constexpr (std::is_same_v<T, int>)
      return H5::PredType::NATIVE_INT;
  }();

  std::vector<T> data(elements);
  dset.read(data.data(), dtype, memspace, dspace);

  memspace.close();
  dspace.close();

  std::vector<hsize_t> data_dims;
  if (rank == 1)
  {
    data_dims = {1};
  }
  else
  {
    data_dims = {count.begin() + 1, count.end()};
  }
  return {std::move(data), std::move(data_dims)};
}

template std::pair<std::vector<double>, std::vector<hsize_t>>
read_data<double>(const H5::DataSet& dset, int timestep);

template std::pair<std::vector<float>, std::vector<hsize_t>>
read_data<float>(const H5::DataSet& dset, int timestep);

template std::pair<std::vector<int>, std::vector<hsize_t>>
read_data<int>(const H5::DataSet& dset, int timestep);

void write_data(const std::vector<double>& data,
                const std::vector<hsize_t>& dims, int timestep,
                const std::string& filename, const std::string& dset_name)
{
  H5::H5File file;
  H5::DataSet dset;
  H5::DataSpace dspace;
  constexpr hsize_t chunk_size_first_dim = 16;

  if (timestep == 0)
  {
    file = {filename, H5F_ACC_EXCL};

    std::vector<hsize_t> dset_dims{dims};
    std::vector<hsize_t> max_dims{dims};
    if (dims[0] == 1)
    {
      dset_dims[0] = chunk_size_first_dim;
      max_dims[0] = H5S_UNLIMITED;
    }
    else
    {
      dset_dims.insert(dset_dims.begin(), chunk_size_first_dim);
      max_dims.insert(max_dims.begin(), H5S_UNLIMITED);
    }
    auto dcpl = H5::DSetCreatPropList();
    dcpl.setChunk(dset_dims.size(), dset_dims.data());

    dspace = H5::DataSpace{static_cast<int>(dset_dims.size()), dset_dims.data(),
                           max_dims.data()};
    dset = file.createDataSet(dset_name.c_str(), H5::PredType::NATIVE_DOUBLE,
                              dspace, dcpl);

    dcpl.close();
  }
  else
  {
    file = {filename, H5F_ACC_RDWR};
    dset = file.openDataSet(dset_name.c_str());
    dspace = dset.getSpace();
  }

  std::vector<hsize_t> current_dset_dims(dspace.getSimpleExtentNdims());
  dspace.getSimpleExtentDims(current_dset_dims.data());
  if (static_cast<int>(current_dset_dims[0]) == timestep + 1)
  {
    std::vector<hsize_t> new_dims{current_dset_dims};
    new_dims[0] += chunk_size_first_dim;
    dset.extend(new_dims.data());
    dspace = dset.getSpace();
  }

  std::vector<hsize_t> count{current_dset_dims};
  std::vector<hsize_t> offset(current_dset_dims.size(), 0);
  count[0] = 1;
  offset[0] = timestep;
  dspace.selectHyperslab(H5S_SELECT_SET, count.data(), offset.data());
  H5::DataSpace memspace{static_cast<int>(current_dset_dims.size()),
                         count.data()};

  dset.write(data.data(), H5::PredType::NATIVE_DOUBLE, memspace, dspace);

  memspace.close();
  dspace.close();
  dset.close();
  file.close();
}
} // namespace as_rpc::h5

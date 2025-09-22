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
#include <vector>

const H5::PredType& determine_datatype(H5::DataSet dset)
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
std::vector<T> read_data(H5::DataSet dset, int timestep,
                         const H5::PredType& dtype)
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
  std::vector<T> data(elements);
  dset.read(data.data(), dtype, memspace, dspace);

  memspace.close();
  dspace.close();

  return data;
}

template std::vector<double> read_data<double>(H5::DataSet dset, int timestep,
                                               const H5::PredType& dtype);
template std::vector<float> read_data<float>(H5::DataSet dset, int timestep,
                                             const H5::PredType& dtype);
template std::vector<int> read_data<int>(H5::DataSet dset, int timestep,
                                         const H5::PredType& dtype);

void write_data(const std::vector<double>& data, const std::string& filename,
                int timestep)
{
  static constexpr std::string dset_name{"/result"};
  H5::H5File file;
  H5::DataSet dset;
  H5::DataSpace dspace;

  if (timestep == 0)
  {
    file = {filename, H5F_ACC_EXCL};

    std::vector<hsize_t> dims{16};
    std::vector<hsize_t> max_dims{H5S_UNLIMITED};
    auto dcpl = H5::DSetCreatPropList();
    dcpl.setChunk(dims.size(), dims.data());

    dspace = H5::DataSpace{static_cast<int>(dims.size()), dims.data(),
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

  std::vector<hsize_t> dims(dspace.getSimpleExtentNdims());
  dspace.getSimpleExtentDims(dims.data());
  if (static_cast<int>(dims[0]) == timestep + 1)
  {
    dims[0] *= 2;
    dset.extend(dims.data());
    dspace = dset.getSpace();
  }

  std::vector<hsize_t> count{dims};
  std::vector<hsize_t> offset(dims.size(), 0);
  count[0] = 1;
  offset[0] = timestep;
  dspace.selectHyperslab(H5S_SELECT_SET, count.data(), offset.data());
  H5::DataSpace memspace{static_cast<int>(dims.size()), count.data()};

  dset.write(data.data(), H5::PredType::NATIVE_DOUBLE, memspace, dspace);

  memspace.close();
  dspace.close();
  dset.close();
  file.close();
}

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

#ifndef H5_HELPERS_HPP
#define H5_HELPERS_HPP

#include <H5Cpp.h>
#include <string>
#include <vector>

const H5::PredType& determine_datatype(const H5::DataSet& dset);

template <typename T>
std::vector<T> read_data(const H5::DataSet& dset, int timestep);

extern template std::vector<double> read_data<double>(const H5::DataSet& dset,
                                                      int timestep);
extern template std::vector<float> read_data<float>(const H5::DataSet& dset,
                                                    int timestep);
extern template std::vector<int> read_data<int>(const H5::DataSet& dset,
                                                int timestep);

// As of 09/2025, mdspan is not yet supported by the gcc libstdc++, so
// we pass the dims of the data separately.
// For any given filename, it is required to always call this with the
// same dims, otherwise the write might fail/lead to unexpected results.
void write_data(const std::vector<double>& data,
                const std::vector<hsize_t>& dims, int timestep,
                const std::string& filename,
                const std::string& dset_name = "/result");

#endif

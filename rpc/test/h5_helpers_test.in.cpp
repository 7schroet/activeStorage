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

#include "../h5_helpers.cpp"
#include <H5Cpp.h>
#include <cstdio>
#include <filesystem>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#define DSET_NAME "/dataset"
#define DSET_NAME_WRITE "/result"

namespace
{

static const std::string TEST_INPUT_BASE{"@H5_TEST_INPUT_DIR@/"};

class Write : public testing::Test
{
protected:
  void SetUp() override { tmp_filename = std::tmpnam(nullptr); }

  void TearDown() override { std::filesystem::remove(tmp_filename); }

  std::string tmp_filename;
};

TEST(Read, SingleDatum)
{
  std::vector<double> expected{10.0};
  H5::H5File file{TEST_INPUT_BASE + "singleDouble.h5", H5F_ACC_RDONLY};
  auto dset = file.openDataSet(DSET_NAME);
  auto actual = read_data<double>(dset, 0);
  EXPECT_EQ(expected, actual);
  dset.close();
  file.close();
}

TEST(Read, twoDimData)
{
  H5::H5File file{TEST_INPUT_BASE + "2dDoubles.h5", H5F_ACC_RDONLY};
  auto dset = file.openDataSet(DSET_NAME);
  for (auto timestep = 0; timestep < 10; timestep++)
  {
    std::vector<double> expected(10, 10.0 + timestep);
    auto actual = read_data<double>(dset, timestep);
    EXPECT_EQ(expected, actual);
  }
  dset.close();
  file.close();
}

TEST(Read, threeDimData)
{
  H5::H5File file{TEST_INPUT_BASE + "3dDoubles.h5", H5F_ACC_RDONLY};
  auto dset = file.openDataSet(DSET_NAME);
  for (auto timestep = 0; timestep < 2; timestep++)
  {
    std::vector<double> expected(100, 10.0 + timestep);
    auto actual = read_data<double>(dset, timestep);
    EXPECT_EQ(expected, actual);
  }
  dset.close();
  file.close();
}

TEST(Read, twoDimInts)
{
  H5::H5File file{TEST_INPUT_BASE + "2dInts.h5", H5F_ACC_RDONLY};
  auto dset = file.openDataSet(DSET_NAME);
  for (auto timestep = 0; timestep < 3; timestep++)
  {
    std::vector<int> expected(10, 10 + timestep);
    auto actual = read_data<int>(dset, timestep);
    EXPECT_EQ(expected, actual);
  }
  dset.close();
  file.close();
}

TEST(Read, threeDimFloats)
{
  H5::H5File file{TEST_INPUT_BASE + "3dFloats.h5", H5F_ACC_RDONLY};
  auto dset = file.openDataSet(DSET_NAME);
  for (auto timestep = 0; timestep < 2; timestep++)
  {
    std::vector<float> expected(100, 20.0f + timestep);
    auto actual = read_data<float>(dset, timestep);
    EXPECT_EQ(expected, actual);
  }
  dset.close();
  file.close();
}

TEST(Read, oobDeathTest)
{
  H5::H5File file{TEST_INPUT_BASE + "singleDouble.h5", H5F_ACC_RDONLY};
  auto dset = file.openDataSet(DSET_NAME);

  EXPECT_ANY_THROW(auto actual = read_data<double>(dset, 1));
  EXPECT_ANY_THROW(auto actual = read_data<double>(dset, -1));

  dset.close();
  file.close();
}

TEST(Dtype, double)
{
  H5::H5File file{TEST_INPUT_BASE + "singleDouble.h5", H5F_ACC_RDONLY};
  auto dset = file.openDataSet(DSET_NAME);
  auto expected = H5::PredType::NATIVE_DOUBLE;

  auto actual = determine_datatype(dset);
  EXPECT_EQ(expected, actual);
}

TEST(Dtype, float)
{
  H5::H5File file{TEST_INPUT_BASE + "3dFloats.h5", H5F_ACC_RDONLY};
  auto dset = file.openDataSet(DSET_NAME);
  auto expected = H5::PredType::NATIVE_FLOAT;

  auto actual = determine_datatype(dset);
  EXPECT_EQ(expected, actual);
}

TEST(Dtype, int)
{
  H5::H5File file{TEST_INPUT_BASE + "2dInts.h5", H5F_ACC_RDONLY};
  auto dset = file.openDataSet(DSET_NAME);
  auto expected = H5::PredType::NATIVE_INT;

  auto actual = determine_datatype(dset);
  EXPECT_EQ(expected, actual);
}

TEST_F(Write, oneDim)
{
  const auto max_timesteps = 3;
  const std::vector<double> data{1.0, 2.0, 3.0};
  for (auto timestep = 0; timestep < max_timesteps; timestep++)
  {
    const std::vector<double> expected{data[timestep]};
    write_data(expected, {1}, tmp_filename, timestep);

    H5::H5File file{tmp_filename, H5F_ACC_RDONLY};
    auto dset = file.openDataSet(DSET_NAME_WRITE);
    auto actual = read_data<double>(dset, timestep);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(1, actual.size());
  }
}

TEST_F(Write, twoDim)
{
  const auto max_timesteps = 4;
  const auto elements_per_timestep = 3;
  for (auto timestep = 0; timestep < max_timesteps; timestep++)
  {
    std::vector<double> expected{};
    for (auto i = 0; i < elements_per_timestep; i++)
      expected.push_back(10.0 * timestep + 1.5 * i);

    write_data(expected, {elements_per_timestep}, tmp_filename, timestep);

    H5::H5File file{tmp_filename, H5F_ACC_RDONLY};
    auto dset = file.openDataSet(DSET_NAME_WRITE);
    auto actual = read_data<double>(dset, timestep);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(elements_per_timestep, actual.size());
  }
}

TEST_F(Write, threeDim)
{
  const auto max_timesteps = 4;
  const std::vector<hsize_t> dims{5, 5};
  const int elements_per_timestep = dims[0] * dims[1];
  for (auto timestep = 0; timestep < max_timesteps; timestep++)
  {
    std::vector<double> expected{};
    for (auto i = 0; i < elements_per_timestep; i++)
      expected.push_back(10.0 * timestep + 1.5 * i);

    write_data(expected, dims, tmp_filename, timestep);

    H5::H5File file{tmp_filename, H5F_ACC_RDONLY};
    auto dset = file.openDataSet(DSET_NAME_WRITE);
    auto actual = read_data<double>(dset, timestep);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(elements_per_timestep, actual.size());
  }
}
} // namespace

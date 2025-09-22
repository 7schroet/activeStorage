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
#include "gtest/gtest.h"
#include <H5Cpp.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#define DSET_NAME "/dataset"

namespace
{

static const std::string TEST_INPUT_BASE{"@H5_TEST_INPUT_DIR@/"};

TEST(Read, SingleDatum)
{
  std::vector<double> expected{10.0};
  H5::H5File file{TEST_INPUT_BASE + "singleDouble.h5", H5F_ACC_RDONLY};
  auto dset = file.openDataSet(DSET_NAME);
  auto actual = read_data(dset, 0);
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
    auto actual = read_data(dset, timestep);
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
    auto actual = read_data(dset, timestep);
    EXPECT_EQ(expected, actual);
  }
  dset.close();
  file.close();
}

// Does not build in current setup
/*TEST(Read, twoDimInts)*/
/*{*/
/*  H5::H5File file{TEST_INPUT_BASE + "2dInts.h5", H5F_ACC_RDONLY};*/
/*  auto dset = file.openDataSet(DSET_NAME);*/
/*  for (auto timestep = 0; timestep < 10; timestep++)*/
/*  {*/
/*    std::vector<int> expected(10, 10 + timestep);*/
/*    auto actual = read_data(dset, timestep);*/
/*    EXPECT_EQ(expected, actual);*/
/*  }*/
/*  dset.close();*/
/*  file.close();*/
/*}*/

TEST(Read, oobDeathTest)
{
  H5::H5File file{TEST_INPUT_BASE + "singleDouble.h5", H5F_ACC_RDONLY};
  auto dset = file.openDataSet(DSET_NAME);

  EXPECT_ANY_THROW(auto actual = read_data(dset, 1));
  EXPECT_ANY_THROW(auto actual = read_data(dset, -1));

  dset.close();
  file.close();
}
} // namespace

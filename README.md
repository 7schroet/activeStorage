# Active Storage

[![CI](https://github.com/7schroet/activeStorage/actions/workflows/tests.yaml/badge.svg)](https://github.com/7schroet/activeStorage/actions/workflows/tests.yaml)

yes

## Dependencies
All dependencies can be built via spack.
### Required
- [Thallium](https://github.com/mochi-hpc/mochi-thallium)
- [HDF5](https://github.com/HDFGroup/hdf5)
### Optional
- [GoogleTest](https://github.com/google/googletest)

## Building
Execute the following commands to build:
```sh
mkdir build
cd build
cmake .. <your options here>
make && make install
```
This will build all the components with the default build type (`Release`) and install
the components into `${CMAKE_CURRENT_SOURCE_DIR}/install`. Tests can be enabled by
setting the option `-DENABLE_TESTS=ON`. Run them via `ctest` after `make`.

For an overview of all available CMake options, run `cmake -LH`.

## Development
### Pre-commit
To keep license information and formatting consistent, we use
pre-commit. Install pre-commit as such:
```sh
python3 -m pip install pre-commit
```
Then run the following command in the root of this repository:
```sh
pre-commit install
```

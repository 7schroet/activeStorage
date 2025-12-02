# Active Storage

[![CI](https://github.com/7schroet/activeStorage/actions/workflows/tests.yaml/badge.svg)](https://github.com/7schroet/activeStorage/actions/workflows/tests.yaml)
![Coverage](https://gist.githubusercontent.com/7schroet/8e4e4da82638ecfb5c987b161dc405da/raw/4ff3c9d68421a9249d5ece70fa49f4ad91a74064/coverage.svg)

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

## Using the HDF5 plugin
The `as-rpc` library is connected to HDF5 via a [VOL plugin](https://support.hdfgroup.org/documentation/hdf5/latest/_h5_v_l__u_g.html).
As such, you only need to set two environment variables whenever you want to use the `as-rpc` library with your HDF5 application.
After building the plugin, set:
```sh
export HDF5_PLUGIN_PATH=<path/to/dir/with/plugin>
export HDF5_VOL_CONNECTOR="as-rpc-hdf5 under_vol=0;under_info={};"
export AS_RPC_SERVER_ADDRESS=</path/to/servername/file>
```
Now all HDF5 calls will go through this connector. The plugin is currently built as a pass-through connector. This means that all
your HDF5 calls will behave as they normally do (i.e., write calls will still write the data), but some functions also invoke
`as-rpc` calls to calculate statistics in-situ. For a thorough overview, refer to TODO.

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

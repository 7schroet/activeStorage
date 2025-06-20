# Active Storage
yes

## Dependencies
This project is reliant on the RPC framework [Mercury](https://github.com/mercury-hpc/mercury).
Make sure it is installed and that it can be found by CMake.

## Building
Execute the following commands to build:
```sh
mkdir build
cd build
cmake .. <your options here>
make && make install
```
This will build all the components with the default build type (`Release`) and install
the components into `${CMAKE_CURRENT_SOURCE_DIR}/install`.

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

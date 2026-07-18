#!/usr/bin/env bash

set -ue

# $1: the activeStorage install dir

script_dir="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
install_dir=$(realpath "$1")

benchmark_exe=${install_dir}/bin/hdf5Writer_bench

if [[ ! -d $install_dir || ! -e ${benchmark_exe} ]]; then
  echo "Provide the path to the Active Storage install dir as the only arg!"
  exit 1
fi

bench_out="${install_dir}/bench_out"
mkdir -p "${bench_out}"

python_out="${bench_out}/python_result.h5"
h5_file="${bench_out}/dummyWriteBench.h5"
plugin_out="${bench_out}/plugin_result.h5"
servername="${bench_out}/servername"
python_env="${bench_out}/bench_h5py"
plugin_config="${bench_out}/config.toml"
PID=

function cleanup_final(){
  rm -rf "${bench_out}"
  kill $PID
}
trap cleanup_final EXIT

ulimit -s unlimited
python3 -mvenv "${python_env}"
. "${python_env}/bin/activate"
python3 -m pip install h5py

set -x
time {
  eval "${benchmark_exe} ${h5_file}"
  python3 "${script_dir}"/mean.py "${h5_file}" "${python_out}"
}

set +x
deactivate
set -x
rm -f "${h5_file}"

export HDF5_USE_FILE_LOCKING=FALSE
"${install_dir}/"bin/as-server --addressfile "${servername}" --port 8081 &
PID=$!
sleep 1

export HDF5_PLUGIN_PATH="${install_dir}/lib64/"
export HDF5_VOL_CONNECTOR="as-rpc-hdf5 under_vol=0;under_info={};"
export AS_RPC_SERVER_ADDRESS="${servername}"
export AS_RPC_OPERATIONS="${plugin_config}"
sed "s|PATH|${bench_out}/|" "${script_dir}"/config.toml.in > "${plugin_config}"

time eval "${benchmark_exe} ${h5_file}"

# Validation, if required
# unset HDF5_VOL_CONNECTOR
# h5diff -c "${python_out}" "${plugin_out}"

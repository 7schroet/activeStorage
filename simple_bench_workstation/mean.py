import h5py
import numpy as np
import sys

with h5py.File(sys.argv[1], "r") as input:
    dset = input["dummyDataset"]
    result = np.empty(dset.shape[0], dtype = dset.dtype)
    for i in range(dset.shape[0]):
        result[i] = np.mean(dset[i,:,:])

with h5py.File("result.h5", "w") as output:
    res_dset = output.create_dataset("result", data=result)

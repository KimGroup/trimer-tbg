import array
import struct
import numpy as np

def read_histogram(str fname, int skip=0):
    cdef list e = []
    cdef list k = []
    cdef list sl = []
    cdef bytes read

    with open(fname, mode="rb") as f:
        f.seek(skip*12, 0)
        read = f.read(12)
        while read and len(read) == 12:
            vals = struct.unpack("=ffi", read)
            e.append(vals[0])
            k.append(vals[1])
            sl.append(vals[2])
            read = f.read(12)
    return np.array(e), np.array(k), np.array(sl)

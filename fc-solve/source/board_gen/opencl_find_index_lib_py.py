#!/usr/bin/env python3

import platform

from cffi import FFI

ffi = FFI()
lib = ffi.dlopen("./libopencl_find_deal_idx." +
                 ("dll" if (platform.system() == 'Windows') else "so"))

ffi.cdef('''
long long fc_solve_user__opencl_find_deal( int first_int, int myints[]);
''')

#!/usr/bin/env python3

from cffi import FFI
import platform

ffi = FFI()
lib = ffi.dlopen("./libfcs_find_deal." +
                 ("dll" if (platform.system() == 'Windows') else "so"))

ffi.cdef('''
long long fc_solve_find_deal_in_range(
const long long start, const long long end, const uint_fast32_t *const ints);
''')

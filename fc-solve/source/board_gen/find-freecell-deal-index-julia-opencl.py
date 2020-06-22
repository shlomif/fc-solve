#!/usr/bin/env python3
#
# find-freecell-deal-index.py - a program to find out the Freecell deal index
# based on the initial cards layout.
#
# Copyright by Shlomi Fish, 2018
#
# Licensed under the MIT/Expat License.

import sys

from make_pysol_freecell_board import find_index_main


def _to_bytes(s):
    if sys.version_info > (3,):
        return bytes(s, 'UTF-8')
    else:
        return s


def find_ret(ints):
    first_int = ints.pop(0)
    assert len(ints) == 50
    with open("test.jl", "wt") as f:
        f.write('''

using OpenCL

const sum_kernel = "
   __kernel void sum(__global unsigned int *r,
                     __global unsigned int *i)
    {{
      int gid = get_global_id(0);
      i[gid] = ((((r[gid] = (r[gid]* 214013 + 2531011)) >> 16) & 0x7fff) % 52);
    }}
"
r = Array{{UInt32}}(UnitRange{{UInt32}}(1:50000))
i = zeros(UInt32, 50000)

myints = [{myints}]
device, ctx, queue = cl.create_compute_context()

r_buff = cl.Buffer(UInt32, ctx, (:r, :copy), hostbuf=r)
i_buff = cl.Buffer(UInt32, ctx, (:r, :copy), hostbuf=i)

p = cl.Program(ctx, source=sum_kernel) |> cl.build!
k = cl.Kernel(p, "sum")

queue(k, size(r), nothing, r_buff, i_buff)

r = cl.read(queue, r_buff)
i = cl.read(queue, i_buff)

for myiterint in 1:50000
    if i[myiterint] == {first_int}
        is_right = true
        for n in 51:-1:2
            r[myiterint] = ((r[myiterint] * 214013 + 2531011) & 0xFFFFFFFF)
            if ( ((r[myiterint] >> 16) & 0x7fff) % n != myints[n])
                is_right = false
                break
            end
        end
        if is_right
            @show myiterint
            break
        end
    end
end
'''.format(first_int=first_int,
           myints=",".join(['0']+list(reversed([str(x) for x in ints]))))
          )
    return 0


if __name__ == "__main__":
    sys.exit(find_index_main(sys.argv, find_ret))

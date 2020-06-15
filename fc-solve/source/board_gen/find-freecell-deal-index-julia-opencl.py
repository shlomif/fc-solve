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
    with open("test.jl", "wt") as f:
        f.write('''

using OpenCL

const sum_kernel = "
   __kernel void sum(__global uint32_t *r,
                     __global uint32_t *i)
    {{
      int gid = get_global_id(0);
      i[gid] = ((((r[gid] = (r[gid]* 214013 + 2531011)) >> 16) & 0x7fff) % 52);
    }}
"
r = 1:50_000
i = zeros(UInt32, 50_000)

myints = [{myints}]
device, ctx, queue = cl.create_compute_context()

r_buff = cl.Buffer(UInt32, ctx, (:r, :copy), hostbuf=r)
i_buff = cl.Buffer(UInt32, ctx, (:r, :copy), hostbuf=i)

p = cl.Program(ctx, source=sum_kernel) |> cl.build!
k = cl.Kernel(p, "sum")

queue(k, size(r), nothing, r_buff, i_buff)

r = cl.read(queue, r_buff)
i = cl.read(queue, i_buff)

for ii in 1:50_000
    if i[ii] == {first_int}
        is_right = True
        for n in 51:2:-1
            r[ii] = (r[ii] * 214013 + 2531011)
            if ((r[ii] >> 16) & 0x7fff) % n != myints[n]
                is_right = False
                break
            end
        end
        if is_right
            @show ii
            break
        end
    end
end
'''.format(first_int=first_int,
           myints=",".join(list(reversed([str(x) for x in ints]))))
          )
    return 0


if __name__ == "__main__":
    sys.exit(find_index_main(sys.argv, find_ret))

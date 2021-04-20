#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2021 Shlomi Fish < https://www.shlomifish.org/ >
#
# Licensed under the terms of the MIT license.

"""

"""


def _unused():
    print(fn="test.jl", template=('''

using OpenCL

device, ctx, queue = cl.create_compute_context()

const first_int = -1
const bufsize = {bufsize}
const num_ints_in_first = {num_ints_in_first};
const num_remaining_ints = 4*13 - num_ints_in_first;

const sum_kernel = "
   __kernel void sum(__global unsigned int *r,
                     __global unsigned int *i)
    {{
      int gid = get_global_id(0);
      {kernel_sum_cl_code}
    }}
"
p = cl.Program(ctx, source=sum_kernel) |> cl.build!
k = cl.Kernel(p, "sum")
is_right = false
mystart = 1
myints = [{myints}]
while (! is_right)
    r = Array{{UInt32}}(UnitRange{{UInt32}}(mystart:mystart+bufsize-1))
    i = zeros(UInt32, bufsize)


    r_buff = cl.Buffer(UInt32, ctx, (:r, :copy), hostbuf=r)
    i_buff = cl.Buffer(UInt32, ctx, (:r, :copy), hostbuf=i)
    queue(k, size(r), nothing, r_buff, i_buff)

    r = cl.read(queue, r_buff)
    i = cl.read(queue, i_buff)

    for myiterint in 1:bufsize
        if i[myiterint] == first_int
            global is_right = true
            rr = r[myiterint]
            for n in num_remaining_ints:-1:2
                rr = ((rr * 214013 + 2531011) & 0xFFFFFFFF)
                if ( ((rr >> 16) & 0x7fff) % n != myints[n])
                    global is_right = false
                    break
                end
            end
            if is_right
                @show mystart+myiterint-1
                break
            end
        end
    end

    global mystart += bufsize
    if mystart > {limit}
        break
    end
end
'''))

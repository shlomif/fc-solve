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
    first_int |= (ints.pop(0) << 6)
    first_int |= (ints.pop(0) << 12)
    first_int |= (ints.pop(0) << 18)

    def _myrand(mod):
        return ('((((r[gid] = (r[gid]*214013 + 2531011))' +
                ' >> 16) & 0x7fff) % {})').format(mod)
    assert len(ints) == 47
    with open("test.jl", "wt") as f:
        f.write('''

using OpenCL

device, ctx, queue = cl.create_compute_context()

const bufsize = {bufsize}

const sum_kernel = "
   __kernel void sum(__global unsigned int *r,
                     __global unsigned int *i)
    {{
      int gid = get_global_id(0);
      i[gid] = {_myrand[52]};
      i[gid] |= ({_myrand[51]} << 6);
      i[gid] |= ({_myrand[50]} << 12);
      i[gid] |= ({_myrand[49]} << 18);
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
        if i[myiterint] == {first_int}
            global is_right = true
            rr = r[myiterint]
            for n in 48:-1:2
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
'''.format(first_int=first_int,
           bufsize=300000,
           _myrand={
               52: _myrand(52),
               51: _myrand(51),
               50: _myrand(50),
               49: _myrand(49),
            },
           limit=((1 << 31)-1),
           myints=",".join(['0']*1+list(reversed([str(x) for x in ints]))))
          )
    return 0


if __name__ == "__main__":
    sys.exit(find_index_main(sys.argv, find_ret))

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

    def _myformat(template):
        return template.format(
            first_int=first_int,
            bufsize=300000,
            _myrand={
                52: _myrand(52),
                51: _myrand(51),
                50: _myrand(50),
                49: _myrand(49),
                },
            limit=((1 << 31)-1),
            myints=",".join(['0']*1+list(reversed([str(x) for x in ints]))))
    with open("vecinit_prog.ocl", "wt") as f:
        f.write(_myformat(
            '''kernel void vecinit(global unsigned * restrict r)
    {{
      const int gid = get_global_id(0);
      r[gid] = gid;
    }}'''))
    with open("test.ocl", "wt") as f:
        f.write(_myformat(
            '''kernel void sum(global unsigned * restrict r,
                     global unsigned * restrict i)
    {{
      const int gid = get_global_id(0);
      i[gid] = {_myrand[52]};
      i[gid] |= ({_myrand[51]} << 6);
      i[gid] |= ({_myrand[50]} << 12);
      i[gid] |= ({_myrand[49]} << 18);
    }}'''))
    with open("test.c", "wt") as f:
        f.write(_myformat('''
/*
    Code based on
    https://www.dmi.unict.it/bilotta/gpgpu/svolti/aa201920/opencl/
    under CC0 / Public Domain.

    Also see:
    https://www.dmi.unict.it/bilotta/gpgpu/libro/gpu-programming.html
    by Giuseppe Bilotta.

    Thanks!

*/
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define CL_TARGET_OPENCL_VERSION 120
#include "ocl_boiler.h"

static size_t gws_align_init;
static size_t gws_align_sum;

static cl_event vecinit(cl_kernel vecinit_k, cl_command_queue que,
        cl_mem r_buff, cl_int nels)
{{
        const size_t gws[] = {{ round_mul_up(((size_t)nels),
            gws_align_init) }};
        printf("init gws: %d | %zu = %zu\\n", nels, gws_align_init, gws[0]);
        cl_event vecinit_evt;
        cl_int err;

        cl_uint i = 0;
        err = clSetKernelArg(vecinit_k, i++, sizeof(r_buff), &r_buff);
        ocl_check(err, "r_buff set vecinit arg", i-1);
        #if 0
        err = clSetKernelArg(vecinit_k, i++, sizeof(nels), &nels);
        ocl_check(err, "nels set vecinit arg", i-1);
        #endif


        err = clEnqueueNDRangeKernel(que, vecinit_k, 1,
                NULL, gws, NULL,
                0, NULL, &vecinit_evt);

        ocl_check(err, "enqueue vecinit");

        return vecinit_evt;
}}

static cl_event vecsum(cl_kernel vecsum_k, cl_command_queue que,
        cl_mem i_buff, cl_mem r_buff, cl_int nels,
        cl_event init_evt)
{{
        const size_t gws[] = {{ round_mul_up(((size_t)nels), gws_align_sum) }};
        printf("sum gws: %d | %zu = %zu\\n", nels, gws_align_sum, gws[0]);
        cl_event vecsum_evt;
        cl_int err;

        cl_uint i = 0;
        err = clSetKernelArg(vecsum_k, i++, sizeof(i_buff), &i_buff);
        ocl_check(err, "i_buff set vecsum arg", i-1);
        err = clSetKernelArg(vecsum_k, i++, sizeof(r_buff), &r_buff);
        ocl_check(err, "r_buff set vecsum arg", i-1);
        #if 0
        err = clSetKernelArg(vecsum_k, i++, sizeof(nels), &nels);
        ocl_check(err, "nels set vecsum arg", i-1);
        #endif

        err = clEnqueueNDRangeKernel(que, vecsum_k, 1,
                NULL, gws, NULL,
                1, &init_evt, &vecsum_evt);

        ocl_check(err, "enqueue vecsum");

        return vecsum_evt;
}}

int main(
        #if 0
int argc, char *argv[]
        #else
        #endif
)
{{
#if 0
        if (argc <= 1) {{
                fprintf(stderr, "specify number of elements\\n");
                exit(1);
        }}
        const int nels = atoi(argv[1]);
        #endif
        const size_t bufsize = {bufsize};

        cl_platform_id p = select_platform();
        cl_device_id d = select_device(p);
        cl_context ctx = create_context(p, d);
        cl_command_queue que = create_queue(ctx, d);
        cl_program vecinit_prog = create_program("vecinit_prog.ocl", ctx, d);
        cl_program prog = create_program("test.ocl", ctx, d);
        cl_int err;

        cl_kernel vecinit_k = clCreateKernel(vecinit_prog, "vecinit", &err);
        ocl_check(err, "create kernel vecinit");
        cl_kernel vecsum_k = clCreateKernel(prog, "sum", &err);
        ocl_check(err, "create kernel vecsum");

        /* get information about the preferred work-group size multiple */
        err = clGetKernelWorkGroupInfo(vecinit_k, d,
                CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
                sizeof(gws_align_init), &gws_align_init, NULL);
        ocl_check(err, "preferred wg multiple for init");
        err = clGetKernelWorkGroupInfo(vecsum_k, d,
                CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
                sizeof(gws_align_sum), &gws_align_sum, NULL);
        ocl_check(err, "preferred wg multiple for sum");


bool is_right = false;
int mystart = 1;
int myints[48] = {{ {myints} }};
cl_mem r_buff = NULL, i_buff = NULL;
r_buff = clCreateBuffer(ctx,
        CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
                bufsize, NULL,
                        &err);
        ocl_check(err, "create buffer r_buff");
i_buff = clCreateBuffer(ctx,
        CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
                bufsize, NULL,
                        &err);
        ocl_check(err, "create buffer i_buff");
        const cl_int nels = bufsize;
while (! is_right)
{{
    // queue(k, size(r), nothing, r_buff, i_buff)
    cl_event init_evt, sum_evt, read_evt;
    init_evt = vecinit(vecinit_k, que, r_buff, nels);
    sum_evt = vecsum(vecsum_k, que, i_buff, r_buff, nels, init_evt);

    #if 0
    r = cl.read(queue, r_buff);
    i = cl.read(queue, i_buff);
    #endif
        cl_int *r_buff_arr = clEnqueueMapBuffer(que, r_buff, CL_FALSE,
                CL_MAP_READ,
                0, bufsize,
                1, &sum_evt, &read_evt, &err);

        clWaitForEvents(1, &read_evt);
        cl_int *i_buff_arr = clEnqueueMapBuffer(que, i_buff, CL_FALSE,
                CL_MAP_READ,
                0, bufsize,
                1, &sum_evt, &read_evt, &err);

        clWaitForEvents(1, &read_evt);

for(cl_int myiterint=0;myiterint < nels; ++myiterint)
{{
        if (i_buff_arr[myiterint] == {first_int})
        {{
            is_right = true;
            cl_int rr = r_buff_arr[myiterint];
            for (int n= 48; n >=2; --n)
            {{
                rr = ((rr * ((cl_int)214013) +
                    ((cl_int)2531011)) & ((cl_int)0xFFFFFFFF));
                if ( ((rr >> 16) & 0x7fff) % n != myints[n])
                {{
                    is_right = false;
                    break;
                }}
            }}
            if ( is_right)
            {{
                printf("%lu\\n", ((unsigned long)(mystart+myiterint)));
                break;
            }}
        }}
    }}

    mystart += bufsize;
    #if 0
    if (mystart > {limit})
    #else
    if (mystart < 0)
    #endif
    {{
        break;
    }}
}}
return 0;
}}
'''))
    with open("test.jl", "wt") as f:
        f.write(_myformat('''

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
'''))
    return 0


if __name__ == "__main__":
    sys.exit(find_index_main(sys.argv, find_ret))

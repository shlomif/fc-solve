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

    def _update_file(fn, newtext):
        update = False
        try:
            with open(fn, "rt") as f:
                oldtext = f.read()
        except FileNotFoundError:
            update = True
        if update or (oldtext != newtext):
            with open(fn, "wt") as f:
                f.write(newtext)

    def _update_file_using_template(fn, template):
        return _update_file(fn=fn, newtext=_myformat(template))
    _update_file_using_template(fn="vecinit_prog.ocl", template=(
                '''kernel void vecinit(global unsigned * restrict r, unsigned mystart)
    {{
      const unsigned gid = get_global_id(0);
      r[gid] = gid + mystart;
    }}'''))
    _update_file_using_template(fn="test.ocl", template=(
                '''kernel void sum(global unsigned * restrict r,
                     global unsigned * restrict i)
    {{
      const unsigned gid = get_global_id(0);
      i[gid] = {_myrand[52]};
      i[gid] |= ({_myrand[51]} << 6);
      i[gid] |= ({_myrand[50]} << 12);
      i[gid] |= ({_myrand[49]} << 18);
    }}'''))
    _update_file_using_template(fn="opencl_find_deal_idx.c", template=('''
/*
    Code based on
    https://www.dmi.unict.it/bilotta/gpgpu/svolti/aa201920/opencl/
    under CC0 / Public Domain.

    Also see:
    https://www.dmi.unict.it/bilotta/gpgpu/libro/gpu-programming.html
    by Giuseppe Bilotta.

    Thanks!

*/
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "freecell-solver/fcs_dllexport.h"

#define CL_TARGET_OPENCL_VERSION 120
#include "ocl_boiler.h"

static size_t gws_align_init;
static size_t gws_align_sum;

static cl_event vecinit(cl_kernel vecinit_k, cl_command_queue que,
        cl_mem r_buff, cl_int mystart, cl_int num_elems)
{{
        const size_t gws[] = {{ round_mul_up(((size_t)num_elems),
            gws_align_init) }};
#if 0
        printf("vecinit gws: mystart = %d ; num_elems = %d | gws_align_init ="
            " %zu ; gws[0] = %zu\\n",
            mystart, num_elems, gws_align_init, gws[0]);
#endif
        cl_event vecinit_evt;
        cl_int err;

        cl_uint i = 0;
        err = clSetKernelArg(vecinit_k, i++, sizeof(r_buff), &r_buff);
        ocl_check(err, "r_buff set vecinit arg", i-1);
        err = clSetKernelArg(vecinit_k, i++, sizeof(mystart), &mystart);
        ocl_check(err, "mystart set vecinit arg", i-1);


        err = clEnqueueNDRangeKernel(que, vecinit_k, 1,
                NULL, gws, NULL,
                0, NULL, &vecinit_evt);

        ocl_check(err, "enqueue vecinit");

        return vecinit_evt;
}}

static cl_event vecsum(cl_kernel vecsum_k, cl_command_queue que,
        cl_mem i_buff, cl_mem r_buff, cl_int num_elems,
        cl_event init_evt)
{{
        const size_t gws[] = {{
        round_mul_up(((size_t)num_elems), gws_align_sum) }};
        #if 0
        printf("vecsum gws: %d | %zu = %zu\\n",
        num_elems, gws_align_sum, gws[0]);
        #endif
        cl_event vecsum_evt;
        cl_int err;

        cl_uint i = 0;
        err = clSetKernelArg(vecsum_k, i++, sizeof(r_buff), &r_buff);
        ocl_check(err, "r_buff set vecsum arg", i-1);
        err = clSetKernelArg(vecsum_k, i++, sizeof(i_buff), &i_buff);
        ocl_check(err, "i_buff set vecsum arg", i-1);

        err = clEnqueueNDRangeKernel(que, vecsum_k, 1,
                NULL, gws, NULL,
                1, &init_evt, &vecsum_evt);

        ocl_check(err, "enqueue vecsum");

        return vecsum_evt;
}}

DLLEXPORT long long fc_solve_user__opencl_find_deal(
const int first_int,
    const int * myints
        #if 0
int argc, char *argv[]
        #endif
)
{{
#if 0
        if (argc <= 1) {{
                fprintf(stderr, "specify number of elements\\n");
                exit(1);
        }}
        const int num_elems = atoi(argv[1]);
for(cl_int myiterint=0;myiterint < 49; ++myiterint)
{{
printf("myints[%d]=%d\\n", myiterint, myints[myiterint]);
}}
#endif
        const size_t num_elems = {bufsize};
        const cl_int cl_int_num_elems = (cl_int)num_elems;
        const size_t bufsize = num_elems * sizeof(cl_int);

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
cl_mem r_buff = NULL, i_buff = NULL;
r_buff = clCreateBuffer(ctx,
        CL_MEM_READ_WRITE, // | CL_MEM_HOST_NO_ACCESS,
                bufsize, NULL,
                        &err);
        ocl_check(err, "create buffer r_buff");
i_buff = clCreateBuffer(ctx,
        CL_MEM_READ_WRITE, // | CL_MEM_HOST_NO_ACCESS,
                bufsize, NULL,
                        &err);
        ocl_check(err, "create buffer i_buff");
while (! is_right)
{{
    // queue(k, size(r), nothing, r_buff, i_buff)
    cl_event init_evt = vecinit(vecinit_k, que, r_buff, mystart, num_elems);
    cl_event sum_evt = vecsum(
        vecsum_k, que, i_buff, r_buff, num_elems, init_evt
    );
    cl_int *r_buff_arr;
    #define BOTH 1
#if 1
    r_buff_arr = clEnqueueMapBuffer(que, r_buff, CL_FALSE,
            CL_MAP_READ,
            0, num_elems,
            1, &sum_evt, &init_evt, &err);
    ocl_check(err, "clEnqueueMapBuffer r_buff_arr");
    assert(r_buff_arr);
#endif

    // clWaitForEvents(1, &init_evt);
    cl_int *i_buff_arr = clEnqueueMapBuffer(que, i_buff, CL_FALSE,
            CL_MAP_READ,
            0, num_elems,
            1, &sum_evt, &init_evt, &err);
    ocl_check(err, "clEnqueueMapBuffer i_buff_arr");
    assert(i_buff_arr);

    clWaitForEvents(1, &sum_evt);

#if BOTH
    r_buff_arr = clEnqueueMapBuffer(que, r_buff, CL_FALSE,
            CL_MAP_READ,
            0, num_elems,
            1, &sum_evt, &init_evt, &err);
    ocl_check(err, "clEnqueueMapBuffer r_buff_arr");
    assert(r_buff_arr);

    clWaitForEvents(1, &sum_evt);
#endif
for(cl_int myiterint=0;myiterint < cl_int_num_elems; ++myiterint)
{{
        if (i_buff_arr[myiterint] == first_int)
        {{
            is_right = true;
            //exit(0);
            cl_int rr = r_buff_arr[myiterint];
            for (int n= 48; n >=1; --n)
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
                long long ret = (mystart+myiterint);
                return ret;
                #if 0
                printf("%lu\\n", ((unsigned long)(mystart+myiterint)));
                #endif
                break;
            }}
        }}
    }}

    mystart += num_elems;
    #if 0
    if (mystart > {limit})
    #else
    if (mystart < 0)
    #endif
    {{
        break;
    }}
}}
return -1;
}}
'''))
    _update_file_using_template(fn="test.jl", template=('''

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

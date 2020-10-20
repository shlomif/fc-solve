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
    with open("test.ocl", "wt") as f:
        f.write(_myformat(
            '''kernel void sum(global unsigned int4 * restrict r,
                     global unsigned int4 * restrict i)
    {{
      const int gid = get_global_id(0);
      i[gid] = {_myrand[52]};
      i[gid] |= ({_myrand[51]} << 6);
      i[gid] |= ({_myrand[50]} << 12);
      i[gid] |= ({_myrand[49]} << 18);
    }}'''))
    with open("test.c", "wt") as f:
        f.write(_myformat('''
cl_event vecsum(cl_kernel vecsum_k, cl_command_queue que,
        cl_mem d_vsum, cl_mem d_v1, cl_mem d_v2, cl_int nels,
        cl_event init_evt)
{{
        const size_t gws[] = { round_mul_up(nels, gws_align_sum) };
        printf("sum gws: %d | %zu = %zu\n", nels, gws_align_sum, gws[0]);
        cl_event vecsum_evt;
        cl_int err;

        cl_uint i = 0;
        err = clSetKernelArg(vecsum_k, i++, sizeof(d_vsum), &d_vsum);
        ocl_check(err, "set vecsum arg", i-1);
        err = clSetKernelArg(vecsum_k, i++, sizeof(d_v1), &d_v1);
        ocl_check(err, "set vecsum arg", i-1);
        err = clSetKernelArg(vecsum_k, i++, sizeof(d_v2), &d_v2);
        ocl_check(err, "set vecsum arg", i-1);
        err = clSetKernelArg(vecsum_k, i++, sizeof(nels), &nels);
        ocl_check(err, "set vecsum arg", i-1);

        err = clEnqueueNDRangeKernel(que, vecsum_k, 1,
                NULL, gws, NULL,
                1, &init_evt, &vecsum_evt);

        ocl_check(err, "enqueue vecsum");

        return vecsum_evt;
}}

int main(int argc, char *argv[])
{{
#if 0
        if (argc <= 1) {{
                fprintf(stderr, "specify number of elements\n");
                exit(1);
        }}
        const int nels = atoi(argv[1]);
        #endif
        const size_t bufsize = {bufsize};

        cl_platform_id p = select_platform();
        cl_device_id d = select_device(p);
        cl_context ctx = create_context(p, d);
        cl_command_queue que = create_queue(ctx, d);
        cl_program prog = create_program("test.ocl", ctx, d);
        cl_int err;

        cl_kernel vecinit_k = clCreateKernel(prog, "vecinit", &err);
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
        ocl_check(err, "create buffer d_v1");
i_buff = clCreateBuffer(ctx,
        CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
                bufsize, NULL,
                        &err);
        ocl_check(err, "create buffer i_buff");
while (! is_right)
{{
    queue(k, size(r), nothing, r_buff, i_buff)
    sum_evt = vecsum(vecsum_k, que, d_vsum, d_v1, d_v2, nels, init_evt);


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

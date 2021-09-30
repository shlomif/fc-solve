#!/usr/bin/env perl

use strict;
use warnings;

use Docker::CLI::Wrapper::Container v0.0.4 ();
my $lib           = "libopencl_find_deal_idx.so";
my $board_gen_dir = "../../source/board_gen";

my $gen_ocl_py_prog = "$board_gen_dir/find-freecell-deal-index-julia-opencl.py";
my $container       = "foo";
my $sys             = "fedora33";
my $obj             = Docker::CLI::Wrapper::Container->new(
    { container => $container, sys => $sys, },
);
if ( not -d "$board_gen_dir" )
{
    die qq#wrong place! "$board_gen_dir" not found.#;
}
if ( not -f "$gen_ocl_py_prog" )
{
    die "$gen_ocl_py_prog wrong place";
}
if (   ( !-e "$lib" )
    || ( "$ENV{REBUILD}" eq "1" ) )
{
    $obj->do_system(
        {
            cmd => [
                "bash",
                "-c",
qq#python3 "$gen_ocl_py_prog" --ms <(pi-make-microsoft-freecell-board -t "24")#,
            ]
        }
    );
    $obj->do_system(
        {
            cmd => [
                "bash",
                "-c",
qq#\${CC:-clang} -shared -fPIC -O3 -march=native -flto -o "$lib" -I ~/Download/unpack/to-del/www.dmi.unict.it/bilotta/gpgpu/svolti/aa201920/opencl/ -I "$board_gen_dir" \${WCFLAGS:--Weverything} "opencl_find_deal_idx.c" -lOpenCL#
            ]
        }
    );
}
my $SEED = 124;
require Math::Random::MT;
my $gen = Math::Random::MT->new($SEED);

sub _check_deal
{
    my ($args)   = @_;
    my $deal     = $args->{deal};
    my $run_deal = $deal;
    my $captured =
`bash -c 'set -e -x ; time python3 "$board_gen_dir"/find-freecell-deal-index-using-opencl.py --ms <(pi-make-microsoft-freecell-board -t "$run_deal")'`;
    if ( not( $captured eq "Found deal = $run_deal\n" ) )
    {
        die "ok run_deal=$run_deal";
    }
    return;
}

DEAL:
while (1)
{
    my $deal = $gen->irand();
    $deal |= ( $gen->irand() << 32 );
    $deal &= ( ( 1 << 33 ) - 1 );
    next DEAL if ( $deal == 0 );
    _check_deal( { deal => $deal, } );

}

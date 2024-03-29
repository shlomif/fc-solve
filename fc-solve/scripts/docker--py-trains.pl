#! /usr/bin/env perl

use strict;
use warnings;
use 5.014;
use autodie;

use Path::Tiny qw/ path tempdir tempfile cwd /;

sub do_system
{
    my ($args) = @_;

    my $cmd = $args->{cmd};
    print "Running [@$cmd]\n";
    if ( system(@$cmd) )
    {
        die "Running [@$cmd] failed!";
    }
}

my @deps = map { /^BuildRequires:\s*(\S+)/ ? ("'$1'") : () }
    path("freecell-solver.spec.in")->lines_utf8;
my $SYS       = "fedora:31";
my $CONTAINER = "fcsfed";
do_system( { cmd => [ 'docker', 'pull', $SYS ] } );
do_system(
    { cmd => [ 'docker', 'run', "-t", "-d", "--name", $CONTAINER, $SYS, ] } );
do_system( { cmd => [ 'docker', 'cp', "../source",  "fcsfed:source", ] } );
do_system( { cmd => [ 'docker', 'cp', "../scripts", "fcsfed:scripts", ] } );
my $script = <<"EOSCRIPTTTTTTT";
set -e -x
sudo dnf -y install cmake gcc gcc-c++ git glibc-devel libcmocka-devel make perl-autodie perl-Path-Tiny python3-devel @deps
pip3 install --user pysol-cards trains
mkdir b
cd b
perl ../scripts/Tatzer -l n2t
make
export LD_LIBRARY_PATH="`pwd`:\$LD_LIBRARY_PATH"
cd ../source
PYTHONPATH="`pwd`/t/lib" python3 ../scripts/py-trains-test.py
EOSCRIPTTTTTTT

do_system(
    { cmd => [ 'docker', 'exec', $CONTAINER, 'bash', '-c', $script, ] } );
do_system( { cmd => [ 'docker', 'stop', $CONTAINER, ] } );
do_system( { cmd => [ 'docker', 'rm',   $CONTAINER, ] } );

__END__

=head1 COPYRIGHT & LICENSE

Copyright 2019 by Shlomi Fish

This program is distributed under the MIT / Expat License:
L<http://www.opensource.org/licenses/mit-license.php>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut

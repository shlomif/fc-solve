#!/usr/bin/env perl

use strict;
use warnings;
use 5.014;
use Cwd ();

BEGIN
{
    use vars qw/ $HOME /;
    $HOME = $ENV{HOME};
}
use lib "$HOME/apps/test/wml/lib64/wml";

use TheWML::Frontends::Wml::Runner ();

use lib './lib';

use Parallel::ForkManager::Segmented ();

my $UNCOND = $ENV{UNCOND} // '';

my $PWD       = Cwd::getcwd();
my @WML_FLAGS = (
    qq%
--passoption=2,-X3074 --passoption=2,-I../lib/ --passoption=3,-I../lib/ -I../lib/ $ENV{LATEMP_WML_FLAGS} -p1-3,5,7,8 --passoption=7,--skip=imgsize,summary \
 -DROOT~. -DLATEMP_THEME=sf.org1 -I $HOME/apps/wml -q
% =~ /(\S+)/g
);

my $T2_SRC_DIR = 'src';
my $T2_DEST    = shift @ARGV;

chdir($T2_SRC_DIR);

my $obj = TheWML::Frontends::Wml::Runner->new;

sub is_newer
{
    my ( $fn1, $fn2 ) = @_;
    my @stat1 = stat($fn1);
    my @stat2 = stat($fn2);
    if ( !@stat2 )
    {
        return 1;
    }
    return ( $stat1[9] >= $stat2[9] );
}

my @queue;
foreach my $lfn (@ARGV)
{
    my $dest     = "$T2_DEST/$lfn";
    my $abs_dest = "$PWD/$dest";
    my $src      = "$lfn.wml";
    if ( $UNCOND or is_newer( $src, $abs_dest ) )
    {
        push @queue,
            [
            $abs_dest,
            "-DPATH_TO_ROOT="
                . ( "../" x ( scalar( () = $lfn =~ m#/#g ) - 0 ) ),
            "-DLATEMP_FILENAME=$lfn",
            $src,
            ];
    }
}
my @FLAGS = ( @WML_FLAGS, '-o', );
my $proc  = sub {
    $obj->run_with_ARGV(
        {
            ARGV => [ @FLAGS, @{ shift(@_) } ],
        }
    ) and die "$!";
    return;
};
Parallel::ForkManager::Segmented->new->run(
    {
        items        => \@queue,
        nproc        => 4,
        batch_size   => 8,
        process_item => $proc,
    }
);

__END__

=head1 COPYRIGHT & LICENSE

Copyright 2018 by Shlomi Fish

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

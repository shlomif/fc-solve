package FC_Solve::Paths::Base;

use 5.014;
use strict;
use warnings;
use parent 'Exporter';

our @EXPORT_OK = qw($IS_WIN exe_fn );

our $IS_WIN = ( $^O eq "MSWin32" );

sub _correct_path
{
    my $p = shift;
    if ($IS_WIN)
    {
        $p =~ tr#/#\\#;
    }
    return $p;
}
my $EXE_SUF = ( $IS_WIN ? '.exe' : '' );

sub exe_fn
{
    return shift . $EXE_SUF;
}

1;

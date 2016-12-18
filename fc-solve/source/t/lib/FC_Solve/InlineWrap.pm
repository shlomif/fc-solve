package FC_Solve::InlineWrap;

use strict;
use warnings;

use Config;
use Inline;

use FC_Solve::Paths qw( $IS_WIN );

sub import
{
    my ( $self, %args ) = @_;

    my ($pkg) = caller(0);

    my $src = delete( $args{C} );
    my $libs = delete( $args{l} ) // '';

    my @workaround_for_a_heisenbug =
        ( $IS_WIN ? ( optimize => '-g' ) : () );

    my $ccflags = "$Config{ccflags} -std=gnu99";
    if ($IS_WIN)
    {
        $ccflags =~ s#(^|\s)-[Of][a-zA-Z0-9_\-]*#$1#gms;
    }

    my @inline_params = (
        C                 => $src,
        name              => $pkg,
        NAME              => $pkg,
        INC               => "-I" . $ENV{FCS_PATH} . " -I" . $ENV{FCS_SRC_PATH},
        CCFLAGS           => $ccflags,
        CLEAN_AFTER_BUILD => 0,
        LIBS              => "-L$ENV{FCS_PATH} $libs",
        @workaround_for_a_heisenbug,
        %args,
    );

    if ($IS_WIN)
    {
        use Data::Dumper;
        print STDERR "inline_params = <<<<<\n", Dumper( \@inline_params ),
            ">>>>>>\n";
    }

    eval "{ package $pkg; Inline->bind(\@inline_params); }";

    if ($@)
    {
        die $@;
    }

    return;
}

1;

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

    my @inline_params = (
        C                 => $src,
        name              => $pkg,
        NAME              => $pkg,
        INC               => "-I" . $ENV{FCS_PATH} . " -I" . $ENV{FCS_SRC_PATH},
        CCFLAGS           => "$Config{ccflags} -std=gnu99",
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

package FC_Solve::InlineWrap;

use strict;
use warnings;

use Config;
use Inline;

sub import
{
    my ( $self, %args ) = @_;

    my ($pkg) = caller(0);

    my $src     = delete( $args{C} );
    my $libs    = delete( $args{l} ) // '';
    my $ccflags = "$Config{ccflags} -std=gnu99";

    my @inline_params = (
        C                 => $src,
        name              => $pkg,
        NAME              => $pkg,
        INC               => "-I" . $ENV{FCS_PATH} . " -I" . $ENV{FCS_SRC_PATH},
        CCFLAGS           => $ccflags,
        CLEAN_AFTER_BUILD => 0,
        LIBS              => "-L$ENV{FCS_PATH} $libs",
        %args,
    );

=begin debug

    if ($IS_WIN)
    {
        require Data::Dumper;
        print STDERR "inline_params = <<<<<\n", Data::Dumper::Dumper( \@inline_params ),
            ">>>>>>\n";
    }

=end debug

=cut

    eval "{ package $pkg; Inline->bind(\@inline_params); }";

    if ($@)
    {
        die $@;
    }

    return;
}

1;

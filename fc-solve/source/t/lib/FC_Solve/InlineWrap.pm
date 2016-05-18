package FC_Solve::InlineWrap;

use strict;
use warnings;

use Config;

use Inline;

sub import
{
    my ($self, %args) = @_;

    my ($pkg) = caller(0);

    my $src = delete($args{C});
    my $libs = delete($args{l}) // '';

    my @inline_params =
    (
        C => $src,
        name => $pkg,
        NAME => $pkg,
        INC => "-I" . $ENV{FCS_PATH} . " -I" . $ENV{FCS_SRC_PATH},
        CCFLAGS => "$Config{ccflags} -std=gnu99",
        CLEAN_AFTER_BUILD => 0,
        LIBS => "-L$ENV{FCS_PATH} $libs",
        %args,
    );

    eval "{ package $pkg; Inline->bind(\@inline_params); }";

    if ($@)
    {
        die $@;
    }

    return;
}

1;

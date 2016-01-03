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

    my @inline_params =
    (
        C => $src,
        name => $pkg,
        NAME => $pkg,
        INC => "-I" . $ENV{FCS_PATH} . " -I" . $ENV{FCS_SRC_PATH},
        CCFLAGS => "$Config{ccflags} -std=gnu99",
        CLEAN_AFTER_BUILD => 0,
        %args,
    );

    eval "{ package $pkg; Inline->bind(\@inline_params); }";

    return;
}

1;

package Shlomif::Spelling::Whitelist;

use strict;
use warnings;

use MooX qw/late/;

extends('HTML::Spelling::Site::Whitelist');

has '+filename' => ( default => 'lib/hunspell/whitelist1.txt' );

sub check_word
{
    my ( $self, $args ) = @_;

    my $filename = $args->{filename};
    my $word     = $args->{word};

    return $self->SUPER::check_word($args)
        || ( $word =~
m#\A(?:0[1-9]|[1-2][0-9]|3[0-1])-(?:Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)-[0-9]{4,}\z#
        );
}

1;

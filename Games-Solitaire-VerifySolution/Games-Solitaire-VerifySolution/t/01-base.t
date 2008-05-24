#!perl -T

use strict;
use warnings;

use Test::More tests => 4;

package MyTest::Package;

use base 'Games::Solitaire::VerifySolution::Base';

__PACKAGE__->mk_accessors(qw(_bold));

sub _init
{
    my ($self, $args) = @_;

    $self->_bold($args->{bold});

    return;
}

sub render_text
{
    my ($self, $text) = @_;

    return ($self->_bold() ? "BOLD: " : "") . $text;
}

package main;

{
    my $obj1 = MyTest::Package->new({});

    # TEST
    ok ($obj1, "Testing constructor");

    # TEST
    is ($obj1->render_text("Hi"), "Hi", "Testing false bold.");
}

{
    my $obj2 = MyTest::Package->new({bold => 1});
    # TEST
    ok ($obj2, "Testing constructor with args");

    # TEST
    is ($obj2->render_text("Hi"), "BOLD: Hi", "Testing true bold.");
}


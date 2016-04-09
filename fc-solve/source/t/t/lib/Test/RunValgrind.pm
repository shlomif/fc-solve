package Test::RunValgrind;

use strict;
use warnings;

use Path::Tiny qw/path/;

use Carp;

sub new
{
    my $class = shift;

    my $self = bless {}, $class;

    $self->_init(@_);

    return $self;
}

sub _init
{
    my ($self, $args) = @_;

    return;
}

sub run
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ($self, $args) = @_;

    my $blurb = $args->{blurb}
        or Carp::confess("blurb not specified.");

    my $log_fn = $args->{log_fn}
        or Carp::confess("log_fn not specified.");

    my $prog = $args->{prog}
        or Carp::confess("prog not specified.");

    my $argv = $args->{argv}
        or Carp::confess("argv not specified.");

    system(
        "valgrind",
        "--track-origins=yes",
        "--leak-check=yes",
        "--log-file=$log_fn",
        $prog,
        @$argv,
    );

    my $out_text = path( $log_fn )->slurp_utf8;
    my $ret = Test::More::ok (
        (index($out_text, q{ERROR SUMMARY: 0 errors from 0 contexts}) >= 0)
        &&
        (index($out_text, q{in use at exit: 0 bytes}) >= 0)
        , $blurb
    );
    if ($ret)
    {
        unlink($log_fn);
    }
    return $ret;
}

1;

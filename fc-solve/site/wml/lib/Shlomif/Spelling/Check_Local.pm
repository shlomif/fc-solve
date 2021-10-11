package Shlomif::Spelling::Check_Local;

use strict;
use warnings;
use autodie;
use utf8;

use Moo;

use Text::Hunspell                ();
use Shlomif::Spelling::Whitelist  ();
use HTML::Spelling::Site::Checker ();

has obj => (
    is      => 'ro',
    default => sub {
        my ($self) = @_;
        my $speller;

        use List::Util qw/all/;
        my @basenames = (qw# en_GB.aff en_GB.dic #);
    DIRS:
        foreach my $dir ( "/usr/share/hunspell", "/usr/share/myspell" )
        {
            my $dn    = $dir;
            my @check = ( map { "$dn/$_" } @basenames );
            if ( all { -e } @check )
            {
                eval {
                    $speller =
                        Text::Hunspell->new( "$dir/en_GB.aff",
                        "$dir/en_GB.dic", );
                };
                next DIRS if $@;
                last DIRS
                    if $speller;
            }
        }

        if ( not $speller )
        {
            die "Could not initialize speller!";
        }

        return HTML::Spelling::Site::Checker->new(
            {
                timestamp_cache_fn => (
                    ( $ENV{LATEMP_SPELL_CACHE_DIR} // './Tests/data/cache' )
                    . '/spelling-timestamp.json'
                ),
                whitelist_parser =>
                    scalar( Shlomif::Spelling::Whitelist->new() ),
                check_word_cb => sub {
                    my ($word) = @_;
                    return $speller->check($word);
                },
            }
        );
    }
);

sub spell_check
{
    my ( $self, $args ) = @_;

    return $self->obj->spell_check(
        {
            files => $args->{files}
        }
    );
}

1;

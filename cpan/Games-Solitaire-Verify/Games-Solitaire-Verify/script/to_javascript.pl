#!/usr/bin/perl

use strict;
use warnings;

use PPI;

foreach my $mod (qw(
lib/Games/Solitaire/Verify/Card.pm
lib/Games/Solitaire/Verify/Column.pm
lib/Games/Solitaire/Verify/Foundations.pm
lib/Games/Solitaire/Verify/Freecells.pm
lib/Games/Solitaire/Verify/KlondikeTalon.pm
lib/Games/Solitaire/Verify/Move.pm
lib/Games/Solitaire/Verify/Solution/ExpandMultiCardMoves/Lax.pm
lib/Games/Solitaire/Verify/Solution/ExpandMultiCardMoves.pm
lib/Games/Solitaire/Verify/Solution.pm
lib/Games/Solitaire/Verify/State.pm
lib/Games/Solitaire/Verify/VariantParams.pm
lib/Games/Solitaire/Verify/VariantsMap.pm
))
{
    my $Document = PPI::Document->new($mod);

    my $acc_ref = $Document->find_first(sub {
            my ($top, $elem) = @_;
            my $ret =
            eval {
                (
                    $elem->can('find_any')
                    &&
                    ($elem->find_any(sub {
                                my ($top, $elem) = @_;
                                return ((
                                        $elem->isa('PPI::Token::Word') and
                                        $elem->literal eq '__PACKAGE__'
                                    ) ? 1 : 0)
                            }
                        ) ? 1 : 0)
                    &&
                    ($elem->find_any(sub {
                                my ($top, $elem) = @_;

                                return
                                (
                                    (
                                        $elem->isa('PPI::Token::Word') and
                                        $elem->method_call and
                                        $elem->literal eq 'mk_acc_ref'
                                    ) ? 1 : 0
                                );
                            }
                        )
                        ? 1 : 0)
                );
            };

            if ($@)
            {
                print "Err=$@\n";
                exit(-1);
            }

            return $ret;
        });

    if (!defined ($acc_ref))
    {
        die "Failure";
    }

    print $acc_ref, "\n";
}

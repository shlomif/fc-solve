#!/usr/bin/perl

use strict;

use FreeCell::Cards;
use FreeCell::State;

my $aisleriot1_state_user;

if (0)
{
$aisleriot1_state_user =
{
    'stacks' =>
    [
        [ qw(AC 8H 2C AH 9H 5S JH) ],
        [ qw(QC 4S 6D 10S AS 10H 4D) ],
        [ qw(4H KS QD 3H 3S JD 8C) ],
        [ qw(9S 5D 3C 2S 5H 7C 7H) ],
        [ qw(9D 10D 9C 7S QH 6C) ],
        [ qw(6H QS JS KD KH 10C) ],
        [ qw(8D 3D JC KC 2D 4C) ],
        [ qw(7D AD 5C 6S 2H 8S) ],
    ],
};
}
elsif(1)
{
$aisleriot1_state_user =
{
    'stacks' =>
    [
        [ qw(JH 8H 2C AH 9H 5S AC) ],
        [ qw(QC 4S 6D 10S AS 10H 4D) ],
        [ qw(4H KS QD 3H 3S JD 8C) ],
        [ qw(9S 5D 3C 2S 5H 7C 7H) ],
        [ qw(9D 10D 9C 7S QH 6C) ],
        [ qw(6H QS JS KD KH 10C) ],
        [ qw(8D 3D JC KC 2D 4C) ],
        [ qw(7D AD 5C 6S 2H 8S) ],
    ],
};

}
sub initial_user_state_to_perl
{
    my $user_state = shift;

    my (@stacks, $s);

    foreach $s (@{$user_state->{'stacks'}})
    {
        push @stacks, [ map { FreeCell::Cards::user2perl($_) } @{$s} ];
    }

    return
    {
        'stacks' => \@stacks,
        'freecells' => [ "", "", "", "" ],
        'decks' => [ 0,0,0,0],
#        'restricted_cards' => [ "", "", "", "",
#                    "", "", "", "" ],
    };
}

sub calc_max_sequence_move
{
    my $fc_num = shift; # Number of free cells

    my $fs_num = shift; # Number of unoccupied stacks.

    if ($fs_num == 0)
    {
        return $fc_num+1;
    }
    elsif ($fs_num == 1)
    {
        return 2*($fc_num+1);
    }
    elsif ($fs_num == 2)
    {
        # 0 - 4 ; 1 - 7 ; 2 - 10
        return (3*($fc_num+1) + 1)
    }
    elsif ($fs_num == 3)
    {
        # 0 - 7 ; 1 - 11
        return (4*($fc_num+1)+3);
    }
    elsif ($fs_num == 4)
    {
        # 0 - 10
        return (5*($fc_num+1)+5);
    }
    elsif ($fs_num >= 5)
    {
        return 13;
    }
}

sub min
{
    my ($m, $a);

    $m = $_[0];
    for($a=1;$a<scalar(@_);$a++)
    {
        if ($m < $_[$a])
        {
            $m = $_[$a];
        }
    }

    return $m;
}

my (@prev_states, @moves);

my $num_times;
my $put_all = 1;

sub solve_for_state
{
    #my $state = shift;

    my $depth = shift;

    my $state;
    if ($put_all)
    {
        $state = $prev_states[$#prev_states];
    }
    else
    {
        $state = $prev_states[$depth];
    }

    $num_times++;

    print "Depth: " . $depth . "\n\n";

    print FreeCell::State::as_string($state), "\n------------\n\n\n";

    my ($stack, $cards_num, @card, $new_state, $ret);
    my ($fc, $dest_stack, $dest_cards_num, @dest_card, $a, $b, $check);
    # $fc is short for freecell

    # First things first, try to see if a card can be placed in the decks
    # It's irreversible so it should be checked first.

    for($stack=0;$stack<8;$stack++)
    {
        $cards_num = scalar(@{$state->{'stacks'}->[$stack]});
        if ($cards_num)
        {
            @card = split(/-/, $state->{'stacks'}->[$stack]->[$cards_num-1]);
            if ($state->{'decks'}->[$card[1]] == $card[0] - 1)
            {
                # We can put it there
                $new_state = FreeCell::State::duplicate_state($state);

                pop(@{$new_state->{'stacks'}->[$stack]});
                $new_state->{'decks'}->[$card[1]]++;
                #$new_state->{'restricted_cards'}->[$stack] = "";

                #push (@prev_states, $new_state);

                $check = 1;

                if ($put_all)
                {
                    for($a=$#prev_states;$a>=0;$a--)
                    {
                        if (FreeCell::State::compare($prev_states[$a], $new_state) == 0)
                        {
                            $check = 0;
                            last;
                        }
                    }

                    if ($check)
                    {
                        push @prev_states, $new_state;
                    }
                }
                else
                {
                    $prev_states[$depth+1] = $new_state;
                }

                if ($check)
                {
                    $ret = solve_for_state($depth+1);
                    if (!$ret)
                    {
                        push @moves, ("Stack " . $stack . " -> Deck " . $card[1]);
                        return 0;
                    }
                }
            }

        }
    }

    # Now check the same for the free cells
    for($fc=0;$fc<4;$fc++)
    {
        if ($state->{'freecells'}->[$fc] ne "")
        {
            @card = split(/-/, $state->{'freecells'}->[$fc]);
            if ($state->{'decks'}->[$card[1]] == $card[0] - 1)
            {
                # We can put it there
                $new_state = FreeCell::State::duplicate_state($state);

                $new_state->{'freecells'}->[$fc] = "";
                $new_state->{'decks'}->[$card[1]]++;

                #push (@prev_states, $new_state);

                $check = 1;
                if ($put_all)
                {
                    for($a=$#prev_states;$a>=0;$a--)
                    {
                        if (FreeCell::State::compare($prev_states[$a], $new_state) == 0)
                        {
                            $check = 0;
                            last;
                        }
                    }

                    if ($check)
                    {
                        push @prev_states, $new_state;
                    }
                }
                else
                {
                    $prev_states[$depth+1] = $new_state;
                }

                if ($check)
                {
                    $ret = solve_for_state($depth+1);
                    if (!$ret)
                    {
                        push @moves, ("Freecell " . $fc . " -> Deck " . $card[1]);
                        return 0;
                    }
                }
            }
        }
    }

    # Now let's try to move a card from one stack to the other
    # Note that it does not involve moving cards lower than king
    # to empty stacks

    # Count the free-cells
    my $num_freecells;
    for($a=0;$a<4;$a++)
    {
        if ($state->{'freecells'}->[$a] eq "")
        {
            $num_freecells++;
        }
    }

    # Count the number of unoccupied stacks
    my $num_freestacks;
    for($a=0;$a<8;$a++)
    {
        if (scalar(@{$state->{'stacks'}->[$a]}) == 0)
        {
            $num_freestacks++;
        }
    }

    my ($c, $is_seq_in_src, @this_card, @prev_card);
    my ($ds, $dc, $is_seq_in_dest, @dest_below_card);
    my ($num_cards_to_relocate, $freecells_to_fill, $freestacks_to_fill);
    for ($stack=0;$stack<8;$stack++)
    {
        $cards_num = scalar(@{$state->{'stacks'}->[$stack]});

        for ($c=0 ; $c<$cards_num ; $c++)
        {
            # Check if there is a sequence here.
            $is_seq_in_src = 1;
            for($a=$c+1 ; $a<$cards_num ; $a++)
            {
                @this_card = split(/-/, $state->{'stacks'}->[$stack]->[$a]);
                @prev_card = split(/-/, $state->{'stacks'}->[$stack]->[$a-1]);

                if (($prev_card[0] == $this_card[0]+1) &&
                    (($prev_card[1] & 0x1) != ($this_card[1] & 0x1)))
                {
                }
                else
                {
                    $is_seq_in_src = 0;
                    last;
                }
            }

            # Find a card which this card can be put on;

            @card = split(/-/, $state->{'stacks'}->[$stack]->[$c]);

            for($ds=0 ; $ds<8; $ds++)
            {
                if ($ds != $stack)
                {
                    $dest_cards_num = scalar(@{$state->{'stacks'}->[$ds]});
                    for($dc=0;$dc<$dest_cards_num;$dc++)
                    {
                        @dest_card = split(/-/, $state->{'stacks'}->[$ds]->[$dc]);

                        if (($card[0] == $dest_card[0]-1) &&
                            (($card[1] & 0x1) != ($dest_card[1] & 0x1)))
                        {
                            # Corresponding cards - see if it is feasible to move
                            # the source to the destination.

                            $is_seq_in_dest = 0;
                            if ($dest_cards_num - 1 > $dc)
                            {
                                @dest_below_card = split(/-/, $state->{'stacks'}->[$ds]->[$dc+1]);
                                if (($dest_below_card[0] == $dest_card[0]-1) &&
                                    (($dest_below_card[1] & 0x1) != ($dest_card[1] & 0x1)))
                                {
                                    $is_seq_in_dest = 1;
                                }
                            }

                            if (! $is_seq_in_dest)
                            {
                                if ($is_seq_in_src)
                                {
                                    $num_cards_to_relocate = $dest_cards_num - $dc - 1;

                                    $freecells_to_fill = min($num_cards_to_relocate, $num_freecells);

                                    $num_cards_to_relocate -= $freecells_to_fill;

                                    $freestacks_to_fill = min($num_cards_to_relocate, $num_freestacks);

                                    $num_cards_to_relocate -= $freestacks_to_fill;

                                    if (($num_cards_to_relocate == 0) &&
                                       (calc_max_sequence_move($num_freecells-$freecells_to_fill, $num_freestacks-$freestacks_to_fill) >=
                                        $cards_num - $c))
                                    {
                                        # We can move it
                                        $new_state = FreeCell::State::duplicate_state($state);

                                        # Fill the freecells with the top cards
                                        for($a=0 ; $a<$freecells_to_fill ; $a++)
                                        {
                                            # Find a vacant freecell
                                            for($b=0;$b<4;$b++)
                                            {
                                                if ($new_state->{'freecells'}->[$b] eq "")
                                                {
                                                    last;
                                                }
                                            }

                                            $new_state->{'freecells'}->[$b] =
                                                pop(@{$new_state->{'stacks'}->[$ds]});

                                        }

                                        # Fill the free stacks with the cards below them
                                        for($a=0; $a < $freestacks_to_fill ; $a++)
                                        {
                                            # Find a vacant stack
                                            for($b=0;$b<8;$b++)
                                            {
                                                if (scalar(@{$new_state->{'stacks'}->[$b]}) == 0)
                                                {
                                                    last;
                                                }
                                            }

                                            push @{$new_state->{'stacks'}->[$b]},
                                                pop(@{$new_state->{'stacks'}->[$ds]});
                                        }

                                        push @{$new_state->{'stacks'}->[$ds]},
                                            (@{$new_state->{'stacks'}->[$stack]}[$c .. $cards_num - 1 ]);

                                        for($a=0;$a<$cards_num-$c;$a++)
                                        {
                                            pop (@{$new_state->{'stacks'}->[$stack]});
                                        }

                                        $check = 1;
                                        for($a=$#prev_states;$a>=0;$a--)
                                        {
                                            if (FreeCell::State::compare($prev_states[$a], $new_state) == 0)
                                            {
                                                $check = 0;
                                                last;
                                            }
                                        }

                                        if ($check)
                                        {

                                            if ($put_all)
                                            {
                                                push @prev_states, $new_state;
                                            }
                                            else
                                            {
                                                $prev_states[$depth+1] = $new_state;
                                            }

                                            $ret = solve_for_state($depth+1);
                                            if (!$ret)
                                            {
                                                push @moves, ("Stack " . $stack . " -> Stack " . $ds);
                                                return 0;
                                            }
                                        }
                                    }
                                }
                            }



                            if (0)
                            {
                                if ($is_seq_in_src && ($dc == $dest_cards_num - 1))
                                {
                                    if (calc_max_sequence_move($num_freecells, $num_freestacks) >=
                                        $cards_num - $c)
                                    {
                                        # We can move it
                                        $new_state = FreeCell::State::duplicate_state($state);

                                        push @{$new_state->{'stacks'}->[$ds]},
                                            (@{$new_state->{'stacks'}->[$stack]}[$c .. $cards_num - 1 ]);

                                        for($a=0;$a<$cards_num-$c;$a++)
                                        {
                                            pop (@{$new_state->{'stacks'}->[$stack]});
                                        }

                                        $check = 1;
                                        for($a=$#prev_states;$a>=0;$a--)
                                        {
                                            if (FreeCell::State::compare($prev_states[$a], $new_state) == 0)
                                            {
                                                $check = 0;
                                                last;
                                            }
                                        }

                                        if ($check)
                                        {

                                            if ($put_all)
                                            {
                                                push @prev_states, $new_state;
                                            }
                                            else
                                            {
                                                $prev_states[$depth+1] = $new_state;
                                            }

                                            $ret = solve_for_state($depth+1);
                                            if (!$ret)
                                            {
                                                push @moves, ("Stack " . $stack . " -> Stack " . $ds);
                                                return 0;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 1;  # This game cannot be solved.
}

my $state = initial_user_state_to_perl($aisleriot1_state_user);

my @moves;

#my $ret = solve_for_state($state, \@moves, 0);
push @prev_states, $state;

my $ret = solve_for_state(0);

if (! $ret)
{
    print "The moves to solve this game are:\n";
    print join("\n", reverse(@moves));
}
else
{
    print "This game is not solveable.\n";
    print "num_times is " . $num_times . "\n";
}

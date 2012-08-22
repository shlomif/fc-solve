#!/usr/bin/perl

use strict;
use warnings;

package Games::Solitaire::Verify::App::From_DBM_FC_Solver;

use base 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::VariantsMap;
use Games::Solitaire::Verify::Solution;
use Games::Solitaire::Verify::State;
use Games::Solitaire::Verify::Move;

use Getopt::Long qw(GetOptionsFromArray);

__PACKAGE__->mk_acc_ref(
    [
        qw(
            _filename
            _variant_params
        )
    ]
);

sub _init
{
    my ($self, $args) = @_;

    my $argv = $args->{'argv'};

    my $variant_map = Games::Solitaire::Verify::VariantsMap->new();

    my $variant_params = $variant_map->get_variant_by_id("freecell");

    GetOptionsFromArray(
        $argv,
        'g|game|variant=s' => sub {
            my (undef, $game) = @_;

            $variant_params = $variant_map->get_variant_by_id($game);

            if (!defined($variant_params))
            {
                die "Unknown variant '$game'!\n";
            }
        },
        'freecells-num=i' => sub {
            my (undef, $n) = @_;
            $variant_params->num_freecells($n);
        },
        'stacks-num=i' => sub {
            my (undef, $n) = @_;
            $variant_params->num_columns($n);
        },
        'decks-num=i' => sub {
            my (undef, $n) = @_;

            if (! ( ($n == 1) || ($n == 2) ) )
            {
                die "Decks should be 1 or 2.";
            }

            $variant_params->num_decks($n);
        },
        'sequences-are-built-by=s' => sub {
            my (undef, $val) = @_;

            my %seqs_build_by =
            (
                (map { $_ => $_ }
                    (qw(alt_color suit rank))
                ),
                "alternate_color" => "alt_color",
            );

            my $proc_val = $seqs_build_by{$val};

            if (! defined($proc_val))
            {
                die "Unknown sequences-are-built-by '$val'!";
            }

            $variant_params->seqs_build_by($proc_val);
        },
        'empty-stacks-filled-by=s' => sub {
            my (undef, $val) = @_;

            my %empty_stacks_filled_by_map =
            (map { $_ => 1 } (qw(kings any none)));

            if (! exists($empty_stacks_filled_by_map{$val}))
            {
                die "Unknown empty stacks filled by '$val'!";
            }

            $variant_params->empty_stacks_filled_by($val);
        },
        'sequence-move=s' => sub {
            my (undef, $val) = @_;

            my %seq_moves = (map { $_ => 1 } (qw(limited unlimited)));

            if (! exists ($seq_moves{$val}) )
            {
                die "Unknown sequence move '$val'!";
            }

            $variant_params->sequence_move($val);
        },
    )
        or die "Cannot process command line arguments";

    my $filename = shift(@$argv);

    if (!defined($filename))
    {
        $filename = "-";
    }

    $self->_variant_params($variant_params);
    $self->_filename($filename);

    return;
}

sub run
{
    my $self = shift;

    my $filename = $self->_filename();
    my $variant_params = $self->_variant_params();

    my $fh;

    if ($filename eq "-")
    {
        $fh = *STDIN;
    }
    else
    {
        open $fh, "<", $filename
            or die "Cannot open '$filename' - $!";
    }

    my $found = 0;

    LINES_PREFIX:
    while (my $line = <$fh>)
    {
        chomp($line);
        if ($line eq "Success!")
        {
            $found = 1;
            last LINES_PREFIX;
        }
    }

    if (!$found)
    {
        close($fh);
        die "State was not solved successfully.";
    }

    my $read_next_state = sub {
        my $line = <$fh>;
        chomp($line);
        if ($line eq "END")
        {
            return;
        }
        elsif ($line ne "--------")
        {
            die "Incorrect format.";
        }

        my $s = <$fh>;
        LINES:
        while ($line = <$fh>)
        {
            if ($line !~ /\S/)
            {
                last LINES;
            }
            $s .= $line;
        }
        $line = <$fh>;
        chomp($line);
        if ($line ne "==")
        {
            die "Cannot find '==' terminator";
        }

        return Games::Solitaire::Verify::State->new(
            {
                variant => "custom",
                variant_params => $self->_variant_params(),
                string => $s,
            },
        );
    };

    my $initial_state = $read_next_state->();

    my $running_state = $initial_state->clone();
    my @cols_indexes = (0 .. ($running_state->num_columns() - 1));
    my @fc_indexes = (0 .. ($running_state->num_freecells() - 1));

    print "-=-=-=-=-=-=-=-=-=-=-=-\n\n";

    my $out_running_state = sub {
        print $running_state->to_string();
        print "\n\n====================\n\n";
    };

    my $calc_foundation_to_put_card_on = sub {
        my $card = shift;

        DECKS_LOOP:
        for my $deck (0 .. $running_state->num_decks() - 1)
        {
            if ($running_state->get_foundation_value($card->suit(), $deck) ==
                $card->rank() - 1)
            {
                my $other_deck_idx;

                for $other_deck_idx (0 ..
                    (($running_state->num_decks() << 2) - 1)
                )
                {
                    if ($running_state->get_foundation_value(
                            $card->get_suits_seq->[$other_deck_idx % 4],
                            ($other_deck_idx >> 2),
                        ) < $card->rank() - 2 -
                        (($card->color_for_suit(
                            $card->get_suits_seq->[$other_deck_idx % 4]
                        ) eq $card->color()) ? 1 : 0)
                    )
                    {
                        next DECKS_LOOP;
                    }
                }
                return [$card->suit(), $deck];
            }
        }
        return;
    };

    $out_running_state->();
    MOVES:
    while (my $move_line = <$fh>)
    {
        chomp($move_line);

        if ($move_line eq "END")
        {
            last MOVES;
        }

        my @rev_cols_indexes;
        @rev_cols_indexes[@cols_indexes] = (0 .. $#cols_indexes);
        my @rev_fc_indexes;
        @rev_fc_indexes[@fc_indexes] = (0 .. $#fc_indexes);

        my ($src, $dest);
        my $dest_move;

        my @tentative_fc_indexes = @fc_indexes;
        my @tentative_cols_indexes = @cols_indexes;
        if (($src, $dest) = $move_line =~ m{\AColumn (\d+) -> Freecell (\d+)\z})
        {
            $dest_move = "Move a card from stack $tentative_cols_indexes[$src] to freecell $tentative_fc_indexes[$dest]";
        }
        elsif (($src, $dest) = $move_line =~ m{\AColumn (\d+) -> Column (\d+)\z})
        {
            $dest_move = "Move 1 cards from stack $tentative_cols_indexes[$src] to stack $tentative_cols_indexes[$dest]";
        }
        elsif (($src, $dest) = $move_line =~ m{\AFreecell (\d+) -> Column (\d+)\z})
        {
            $dest_move = "Move a card from freecell $tentative_fc_indexes[$src] to stack $tentative_cols_indexes[$dest]";
        }
        elsif (($src) = $move_line =~ m{\AColumn (\d+) -> Foundation \d+\z})
        {
            $dest_move = "Move a card from stack $tentative_cols_indexes[$src] to the foundations";
        }
        elsif (($src) = $move_line =~ m{\AFreecell (\d+) -> Foundation \d+\z})
        {
            $dest_move = "Move a card from freeecell $tentative_fc_indexes[$src] to the foundations";
        }
        print "$dest_move\n\n";


        $running_state->verify_and_perform_move(
            Games::Solitaire::Verify::Move->new(
                {
                    fcs_string => $dest_move,
                    game => $running_state->_variant(),
                },
            )
        );
        $out_running_state->();

        # Now do the horne's prune.
        my $num_moved = 1; # Always iterate at least once.

        my $perform_prune_move = sub {
            my $prune_move = shift;

            $num_moved++;

            $running_state->verify_and_perform_move(
                Games::Solitaire::Verify::Move->new(
                    {
                        fcs_string => $prune_move,
                        game => $running_state->_variant(),
                    }
                )
            );
            print "$prune_move\n\n";
            $out_running_state->();
        };

        while ($num_moved)
        {
            $num_moved = 0;
            foreach my $idx (0 .. ($running_state->num_columns()-1) )
            {
                my $col = $running_state->get_column($idx);

                if ($col->len())
                {
                    my $card = $col->top();
                    my $f = $calc_foundation_to_put_card_on->($card);

                    if (defined($f))
                    {
                        $perform_prune_move->(
                            "Move a card from stack $idx to the foundations"
                        );
                    }
                }
            }

            foreach my $idx (0 .. ($running_state->num_freecells() - 1))
            {
                my $card = $running_state->get_freecell($idx);

                if (defined($card))
                {
                    my $f = $calc_foundation_to_put_card_on->($card);

                    if (defined($f))
                    {
                        $perform_prune_move->(
                            "Move a card from freecell $idx to the foundations"
                        );
                    }
                }
            }
        }

        my $new_state = $read_next_state->();

        # Calculate the new indexes.
        my @new_cols_indexes;
        my @new_fc_indexes;

        my %old_cols_map;
        my %old_fc_map;
        my %non_assigned_cols =
            (map { $_ => 1 } (0 .. $running_state->num_columns() - 1));

        my %non_assigned_fcs =
            (map { $_ => 1 } (0 .. $running_state->num_freecells() - 1));

        foreach my $idx (0 .. ($running_state->num_columns() - 1))
        {
            my $col = $running_state->get_column($idx);
            my $card = $col->len ? $col->pos(0)->to_string() : '';

            push @{$old_cols_map{$card}}, $idx;
        }

        foreach my $idx (0 .. ($running_state->num_columns() - 1))
        {
            my $col = $new_state->get_column($idx);
            my $card = $col->len ? $col->pos(0)->to_string() : '';
            # TODO: Fix edge cases.
            my $aref = $old_cols_map{$card};

            if ((!defined($aref)) or (! @$aref))
            {
                $aref = $old_cols_map{''};
            }
            my $i = shift(@$aref);

            $new_cols_indexes[$idx] = $i;
            if (defined($i))
            {
                delete($non_assigned_cols{$i});
            }
        }

        my @non_assigned_cols_list = sort { $a <=> $b } keys(%non_assigned_cols);
        foreach my $col_idx (@new_cols_indexes)
        {
            if (!defined($col_idx))
            {
                $col_idx = shift(@non_assigned_cols_list);
            }
        }

        foreach my $idx (0 .. ($running_state->num_freecells() - 1))
        {
            my $card_obj = $running_state->get_freecell($idx);
            my $card = defined($card_obj) ? $card_obj->to_string() : '';

            push @{$old_fc_map{$card}}, $idx;
        }

        foreach my $idx (0 .. ($running_state->num_freecells() - 1))
        {
            my $card_obj = $new_state->get_freecell($idx);
            my $card = defined($card_obj) ? $card_obj->to_string() : '';
            # TODO : Fix edge cases.

            my $aref = $old_fc_map{$card};

            if ((!defined($aref)) or (! @$aref))
            {
                $aref = $old_fc_map{''};
            }

            my $i = shift(@$aref);
            $new_fc_indexes[$idx] = $i;
            if (defined($i))
            {
                delete($non_assigned_fcs{$i});
            }
        }

        my @non_assigned_fcs_list = sort { $a <=> $b } keys(%non_assigned_fcs);

        foreach my $fc_idx (@new_fc_indexes)
        {
            if (!defined ($fc_idx))
            {
                $fc_idx = shift(@non_assigned_fcs_list);
            }
        }

        my $verify_state =
            Games::Solitaire::Verify::State->new(
                {
                    variant => 'custom',
                    variant_params => $self->_variant_params(),
                }
            );

        foreach my $idx (0 .. ($running_state->num_columns() - 1))
        {
            $verify_state->add_column(
                $running_state->get_column($new_cols_indexes[$idx])->clone()
            );
        }

        $verify_state->set_freecells(
            Games::Solitaire::Verify::Freecells->new(
                {
                    count => $running_state->num_freecells(),
                }
            )
        );

        foreach my $idx (0 .. ($running_state->num_freecells() - 1))
        {
            my $card_obj = $running_state->get_freecell($new_fc_indexes[$idx]);

            if (defined($card_obj))
            {
                $verify_state->set_freecell($idx, $card_obj->clone());
            }
        }

        $verify_state->set_foundations($running_state->_foundations->clone());

        {
            my $v_s = $verify_state->to_string();
            my $n_s = $new_state->to_string();
            if ($v_s ne $n_s)
            {
                die "States mismatch:\n<<\n$v_s\n>>\n vs:\n<<\n$n_s\n>>\n.";
            }
        }

        @cols_indexes = @new_cols_indexes;
        @fc_indexes = @new_fc_indexes;
    }

    print "This game is solveable.\n";

    close($fh);
}

package main;

Games::Solitaire::Verify::App::From_DBM_FC_Solver->new({ argv => [@ARGV] })->run();

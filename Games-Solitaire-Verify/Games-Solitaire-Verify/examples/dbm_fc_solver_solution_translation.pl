#!/usr/bin/perl

use strict;
use warnings;

package Games::Solitaire::Verify::App::From_DBM_FC_Solver;

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
        if (($src, $dest) = $move_line =~ m{\AColumn (\d+) -> Freecell (\d+)})
        {
        }

        my $new_state = $read_next_state->();

        # Calculate the new indexes.
        my @new_cols_indexes;
        my @new_fc_indexes;

        my %old_cols_map;
        my %old_fc_map;

        foreach my $idx (0 .. ($running_state->num_columns() - 1))
        {
            my $col = $running_state->get_column($idx);
            my $card = $col->len ? $col->pos(0)->as_string() : '';

            push @{$old_cols_map{$card}}, $idx;
        }

        foreach my $idx (0 .. ($running_state->num_columns() - 1))
        {
            my $col = $new_state->get_column($idx);
            my $card = $col->len ? $col->pos(0)->as_string() : '';
            # TODO: Fix edge cases.
            $new_cols_indexes[$idx] = $cols_indexes[shift(
                @{
                    $old_cols_map{
                        $card
                    }
                }
            )];
        }

        foreach my $idx (0 .. ($running_state->num_freecells() - 1))
        {
            my $card_obj = $running_state->get_freecell();
            my $card = defined($card_obj) ? $card_obj->as_string() : '';

            push @{$old_fc_map{$card}}, $idx;
        }

        foreach my $idx (0 .. ($running_state->num_freecells() - 1))
        {
            my $card_obj = $running_state->get_freecell();
            my $card = defined($card_obj) ? $card_obj->as_string() : '';
            # TODO : Fix edge cases.
            $new_fc_indexes[$idx] = $fc_indexes[shift(
                @{
                    $old_cols_map{
                        $card
                    }
                }
            )];
        }

    }

    close($fh);
}

package main;

Games::Solitaire::Verify::App::From_DBM_FC_Solver->new({ argv => [@ARGV] })->run();

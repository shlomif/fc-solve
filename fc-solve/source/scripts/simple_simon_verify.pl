#!/usr/bin/perl

use strict;

use FileHandle;

use FreeCell::State;
use FreeCell::Card;

my $fh = FileHandle->new("<".$ARGV[0]);

my $line = $fh->getline();
if ($line !~ /^-=-=-=-=/)
{
    die "Incorrect format for file.";
}

my ($orig_state, %move, $new_state, $file_new_state);

$orig_state = FreeCell::State::read_from_file($fh);

MAIN_LOOP: while (1)
{       
    while ($line = $fh->getline())
    {
        if ($line =~ /^Move/)
        {
            last;
        }
        if ($line =~ /^This game/)
        {
            last MAIN_LOOP;
        }
    }

    if ($line =~ /^Move (\d+) cards from stack (\d+) to stack (\d+)/)
    {
        $move{'type'} = "stack_to_stack";
        $move{'source'} = $2;
        $move{'dest'} = $3;
        $move{'num_cards'} = $1;
    }
    elsif ($line =~ /^Move the sequence on top of Stack (\d+) to the foundations/)
    {
        $move{'type'} = "seq_to_founds";
        $move{'source'} = $1;
    }
    else
    {
        die "Uknown move!";
    }

    $orig_state->verify_and_perform_simple_simon_move(\%move);
    $file_new_state = FreeCell::State::read_from_file($fh);
    if (FreeCell::State::compare($orig_state, $file_new_state))
    {
        die "Incorrect move was performed on the state!";
    }
    $orig_state = $file_new_state;
}

print "Solution is OK\n";



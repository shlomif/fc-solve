#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 4;
use Carp;
use IPC::Open2;

sub check_split
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $input_string = shift;
    my $want_argv = shift;
    my $msg = shift;

    my $split_exe = $ENV{'FCS_PATH'}."/t/out-split-cmd-line.exe";

    my ($child_out, $child_in);

    my $pid = open2($child_out , $child_in, $split_exe);

    print {$child_in} $input_string;
    close($child_in);

    my @have;
    while (my $line = <$child_out>)
    {
        chomp($line);
        if ($line !~ m{\A<<(.*)\z})
        {
            die "Invalid output from program.";
        }
        my $terminator = $1;
        
        my $item = "";
        my $found_terminator = 0;
        while ($line = <$child_out>)
        {
            if ($line eq "$terminator\n")
            {
                $found_terminator = 1;
                last;
            }
            $item .= $line;
        }
        if (!$found_terminator)
        {
            die "Could not find terminator '$terminator' in output.";
        }
        chomp($item);
        push @have, $item;
    }

    is_deeply(
        \@have,
        $want_argv,
        $msg,
    );
}

# TEST
check_split(
    "one two three",
    ["one", "two", "three",],
    "Simple barewords",
);


# TEST
check_split(
    "one two three lom-prom-tom KuTler abruptChange Come-Together",
    [
        "one", "two", "three", "lom-prom-tom", "KuTler", 
        "abruptChange", "Come-Together",
    ],
    "Simple barewords No. 2",
);

# TEST
check_split(
    q{I read    "The Adventures of Tom Sawyer" and liked it.},
    [
        "I",
        "read",
        "The Adventures of Tom Sawyer",
        "and",
        "liked",
        "it.",
    ],
    "Words in quotes + more than one space.",
);


# TEST
check_split(
    qq{I read \\"The       Adventure\\"},
    [
        "I",
        "read",
        q{"The},
        q{Adventure"},
    ],
    "Escaped quotes",
);


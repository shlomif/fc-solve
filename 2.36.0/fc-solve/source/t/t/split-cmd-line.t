#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 20;
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

# TEST
check_split(
    qq{I read \\"The       Adventure\\"\n},
    [
        "I",
        "read",
        q{"The},
        q{Adventure"},
    ],
    "Escaped quotes with newline",
);


# TEST
check_split(
    qq{I read \\"The       Adventure\\"                \n},
    [
        "I",
        "read",
        q{"The},
        q{Adventure"},
    ],
    "Escaped quotes line with trailing space",
);


# TEST
check_split(
    qq{I read \\"The       Adventure\\"      },
    [
        "I",
        "read",
        q{"The},
        q{Adventure"},
    ],
    "Escaped quotes with trailing space.",
);


# TEST
check_split(
    qq{GNU\\\\Linux and Platro\\\\Day\n},
    [
        "GNU\\Linux",
        "and",
        "Platro\\Day",
    ],
    "Backslash",
);

# TEST
check_split(
    qq{Hi "Mostly \\"Done deal\\" Plurality" There},
    [
        "Hi",
        "Mostly \"Done deal\" Plurality",
        "There",
    ],
    "Backslash-quotes inside quotes.",
);

# TEST
check_split(
    qq{Hi "Mostly \\"Done deal\\" w\\\\o Plurality" There},
    [
        "Hi",
        "Mostly \"Done deal\" w\\o Plurality",
        "There",
    ],
    "Backslash-quotes and backslash inside quotes.",
);

# TEST
check_split(
    qq{Aloha\\ Audrey and Alan\n},
    [
        "Aloha Audrey",
        "and",
        "Alan",
    ],
    "backslash-space.",
);

# TEST
check_split(
    qq{Aloha\\ Audrey and Alan},
    [
        "Aloha Audrey",
        "and",
        "Alan",
    ],
    "backslash-space without newline",
);

# TEST
check_split(
    qq{One\\ Two" Three Four Five"Six\\ Seven & Eight\n},
    [
        "One Two Three Four FiveSix Seven",
        "&",
        "Eight",
    ],
    "Mixed quotes and backslash space.",
);


# TEST
check_split(
    qq{       One\\ Two" Three Four Five"Six\\ Seven & Eight   },
    [
        "One Two Three Four FiveSix Seven",
        "&",
        "Eight",
    ],
    "Mixed quotes and backslash space with leading and trailing space",
);

# TEST
check_split(
    <<'EOF',
# This is a comment.
OneWord word2 "Word 3"  \
    Word-No-4

EOF
    [
        "OneWord",
        "word2",
        "Word 3",
        "Word-No-4",
    ],
    "Leading comment",
);

# TEST
check_split(
    <<'EOF',
# This is a comment.
OneWord word2 "Word 3"  \
   WORD_NO_444    # End of line comment.

EOF
    [
        "OneWord",
        "word2",
        "Word 3",
        "WORD_NO_444",
    ],
    "Leading comment",
);

# TEST
check_split(
    <<'EOF',
First Line
Second Line
EOF
    [
        "First",
        "Line",
        "Second",
        "Line",
    ],
    "Two lines",
);


# TEST
check_split(
    <<'EOF',
First Line   \
Second Line
EOF
    [
        "First",
        "Line",
        "Second",
        "Line",
    ],
    "Two lines with trailing backslash",
);

# TEST
check_split(
    <<'EOF',
First Line   
Second Line
"Third ""L-R"
EOF
    [
        "First",
        "Line",
        "Second",
        "Line",
        "Third L-R",
    ],
    "Three lines.",
);

# TEST
check_split(
    <<'EOF',
First Line   
Second Line                 # A comment
"Third ""L-R"
EOF
    [
        "First",
        "Line",
        "Second",
        "Line",
        "Third L-R",
    ],
    "Three lines with a comment",
);

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2009 Shlomi Fish

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut


package FreecellSolver::ExtractGames;

use strict;
use warnings;
use IO::All qw/ io /;

use MooX qw/ late /;

has 'games' => (is => 'ro', default => sub {
        my @lines = io->file('../../source/USAGE.txt')->chomp->getlines;
        my @ret;
        foreach my $l (@lines)
        {
            if ($l =~ /\A--game \[game\]/ .. $l =~ /\AExamples/)
            {
                if (my ($id, $name) = ($l =~ m#\A\|\+([a-zA-Z_]+)\+\s+\|([A-Za-z' ]*)\z#))
                {
                    push @ret, {id => $id, name => $name};
                }
            }
        }
        return \@ret;
    });
1;

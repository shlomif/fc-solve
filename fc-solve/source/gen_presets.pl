#!/usr/bin/perl -w

use strict;

my %presets = 
(
    'bakers_game' => [ qw(i freecell sbb suit) ],
    'bakers_dozen' => [ qw(s 13 f 0 d 1 sbb rank sm limited esf none to 0123456789 at 0123456789) ],
    'beleaguered_castle' => [ qw(i freecell sbb rank f 0) ],
    'cruel' => [ qw(i bakers_dozen s 12 sbb suit) ],
    'der_katzenschwanz' => [ qw(i die_schlange sm unlimited) ],
    'die_schlange' => [ qw(i freecell f 8 s 9 d 2 esf none) ],
    'eight_off' => [ qw(i kings_only_bakers_game f 8) ],
    'fan' => [ qw(i freecell s 18 sbb suit sm limited esf kings_only f 0) ],
    'forecell' => [ qw(i freecell esf kings_only) ],
    'freecell' => [ qw(s 8 f 4 d 1 sbb ac sm limited esf any_card to [01][23456789] at 0123456789) ],
    'good_measure' => [ qw(i bakers_dozen s 10) ],
    'kings_only_bakers_game' => [ qw(i bakers_game esf kings_only) ],
    'relaxed_freecell' => [ qw(i freecell sm unlimited) ],
    'relaxed_seahaven_towers' => [ qw(i seahaven_towers sm unlimited) ],
    'seahaven_towers' => [ qw(i bakers_game esf kings_only s 10) ],
    'simple_simon' => [ qw(i bakers_game s 10 f 0 to abcdefgh at abcdefgh) ],
    # 'yukon' => [ qw(i freecell s 7 f 0 to ABCDEFG) ],
);

my @strings;

sub compile_preset
{
    my $preset_name = shift;

    my $preset = $presets{$preset_name};

    # For inheritance
    my $compiled = shift || {};

    my @params = @{$preset};

    my ($cmd, $arg);

    eval {

    while ($cmd = shift(@params))
    {
        $arg = shift(@params);
        
        if ($cmd =~ /^(i|inherits?)$/)
        {
            if (!exists($presets{$arg}))
            {
                die "Unknown inheritor!\n";
            }
            compile_preset($arg, $compiled);
        }
        elsif ($cmd =~ /^(s|stacks?)$/)
        {
            if ($arg !~ /^(\d+)$/)
            {
                die "Argument to stacks is not an integer!\n";
            }
            $compiled->{'stacks'} = $arg;            
        }
        elsif ($cmd =~ /^(f|freecells?)$/)
        {
            if ($arg !~ /^(\d+)$/)
            {
                die "Argument to freecells is not an integer!\n";
            }
            $compiled->{'freecells'} = $arg;
        }
        elsif ($cmd =~ /^(d|decks?)$/)
        {
            if ($arg !~ /^(1|2)$/)
            {
                die "Argument to decks is not 1 or 2!\n";
            }
            $compiled->{'decks'} = $arg;
        }
        elsif ($cmd =~ /^(sbb|seqs_build_by)$/)
        {
            if ($arg !~ /^(ac|suit|rank)$/)
            {
                die "Argument to stacks_build_by is improper!\n";
            }
            $compiled->{'seqs_build_by'} = $arg;
        }
        elsif ($cmd =~ /^(sm|(seq|sequence)_move)$/)
        {
            if ($arg !~ /^(limited|unlimited)$/)
            {
                die "Argument to sequence move is not limited/unlimited!\n";
            }
            $compiled->{'sequence_move'} = (($arg eq "unlimited")?1:0);
        }
        elsif ($cmd =~ /^(esf|empty_stacks_fill(ed(_by)?)?)$/)
        {
            if ($arg !~ /^(any_card|kings_only|none)$/)
            {
                die "Improper parameter to Empty Stacks Filled By!\n";
            }
            $compiled->{'empty_stacks_fill'} = $arg;
        }
        elsif ($cmd =~ /^(to|tests_order)$/)
        {
            if ($arg =~ /[^0-9a-hA-G\[\(\)\]]/)
            {
                die "Unrecognized character in Tests order!\n";
            }
            $compiled->{'tests_order'} = $arg;
        }
        elsif ($cmd =~ /^(at|allowed_tests)$/)
        {
            if ($arg =~ /[^0-9a-hA-G]/)
            {
                die "Unrecognized character in Allowed Tests!\n";
            }
            $compiled->{'allowed_tests'} = $arg;
        }
        else
        {
            die "Unknown Command $cmd\n";
        }
    }
    
    };

    if ($@)
    {
        die "Preset Name: $preset_name\n$@";
    }

    return $compiled;
}

sub preset_to_string
{
    my $preset_name = shift;
    my $pc = shift;
    
    my @lines = ();

    my $arg;

    eval
    {
    
    push @lines, ("FCS_PRESET_" . uc($preset_name));
    
    if (!exists($pc->{'freecells'}))
    {
        die "Freecells were not defined!\n";
    }
    push @lines, $pc->{'freecells'};
    
    if (!exists($pc->{'stacks'}))
    {
        die "Stacks were not defined!\n";
    }
    push @lines,  $pc->{'stacks'};

    if (!exists($pc->{'decks'}))
    {
        die "Decks Number was not defined!\n";
    }
    push @lines,  $pc->{'decks'};

    if (!exists($pc->{'seqs_build_by'}))
    {
        die "Seqs Build by was not defined!\n";
    }
    $arg = $pc->{'seqs_build_by'};
    push @lines, ("FCS_SEQ_BUILT_BY_". (($arg eq "ac") ? "ALTERNATE_COLOR" : ($arg eq "suit") ? "SUIT" : "RANK"));

    if (!exists($pc->{'sequence_move'}))
    {
        die "Sequence move was not defined!\n";
    }
    push @lines, $pc->{'sequence_move'};

    if (!exists($pc->{'empty_stacks_fill'}))
    {
        die "Empty Stacks Fill is undefined!\n";        
    }
    $arg = $pc->{'empty_stacks_fill'};
    push @lines, "FCS_ES_FILLED_BY_" . (($arg eq "none") ? "NONE" : ($arg eq "any_card") ? "ANY_CARD" : "KINGS_ONLY");

    if (!exists($pc->{'tests_order'}))
    {
        die "Tests order is undefined!\n";
    }
    push @lines, "\"" . $pc->{'tests_order'} . "\"";

    if (!exists($pc->{'allowed_tests'}))
    {
        die "Allowed Tests' is undefined!\n";
    }
    push @lines, "\"" . $pc->{'allowed_tests'} . "\"";

    };

    if ($@)
    {
        die "Preset name: $preset_name\n$@\n";
    }

    my @lines_p = (map { "        $_," } @lines);
    my @lines_with_spaces = (@lines_p[0 .. 3], "", @lines_p[4 .. 6], "", @lines_p[7 .. $#lines_p]);

    return "    {\n" . join("\n", @lines_with_spaces) . "\n" . "    },\n";
}

foreach my $preset_name (sort {$a cmp $b } keys(%presets))
{
    my $preset_compiled = compile_preset($preset_name);
    push @strings, preset_to_string($preset_name, $preset_compiled);
}

print "static const fcs_preset_t fcs_presets[" . scalar(@strings) . "] = \n";
print "{\n";
print join("", @strings);
print "};\n";


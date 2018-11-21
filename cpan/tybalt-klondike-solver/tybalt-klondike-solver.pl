#!/usr/bin/perl

# widthsolitaire.pl - width first solitaire search

use File::Slurp;
use List::Util qw(shuffle);
use strict;

my $eachfile = $ARGV[0] eq '-e' && 'd.eachfile';
$eachfile and write_file $eachfile, '';

my @cards = shuffle glob '{A,2,3,4,5,6,7,8,9,T,J,Q,K}{S,H,D,C}';

my $layout = '';
for ( 1 .. 7 )
{
    $layout .= lc shift @cards for 2 .. $_;
    $layout .= shift(@cards) . "\n";
}

$layout = ( lc join '', @cards ) . " -- -- -- -- --\n$layout";

my ( $deck, $over, $spades, $hearts, $diamonds, $clubs, @stacks );
my %found = ( S => \$spades, H => \$hearts, D => \$diamonds, C => \$clubs );
my %wantsuit = ( S => '[HD]', H => '[SC]', D => '[SC]', C => '[HD]' );
my ( %rank, %state, $moves );
@rank{qw(-A A2 23 34 45 56 67 78 89 9T TJ JQ QK)} = ();
my $rank  = '-A23456789TJQK';
my $cycle = 0;
my ( $low, $high ) = ( 1, 1 );
my @new = ($layout);
$state{$layout} = '';

while ( my $n = @new )
{
    my $k = keys %state;
    $low > $n  and $low  = $n;
    $high < $n and $high = $n;
    if ( not ++$cycle % 1000 )
    {
        print "n: $low/$n/$high keys $k cycle $cycle\n";
        $low = $high = $n;
    }
    $layout = shift @new;
    ( $deck, $over, $spades, $hearts, $diamonds, $clubs, @stacks ) =
        split ' ', $layout;
    $eachfile and append_file $eachfile, "\n$layout";

    if ( $layout =~ /^-- -- KS KH KD KC/ )
    {
        print "\n*** WIN !!!\n\n";
        showwin();
    }
    move();
}

if ($eachfile)
{
    for my $key ( sort keys %state )
    {
        ( my $tmp = $key ) =~ s/\s+/ /g;
        append_file $eachfile, "state: $tmp\n";
    }
}
my $n = keys %state;
die "cycle: $cycle ** failed at state: $n **\n$layout";

sub addmove
{
    my $new = join "\n",
        "$deck $over $spades $hearts $diamonds $clubs", @stacks, '';
    if ( not exists $state{$new} )
    {
        $state{$new} = $layout;
        push @new, $new;
        if ($eachfile)
        {
            ( my $tmp = $new ) =~ s/\s+/ /g;
            append_file $eachfile, "push: $tmp\n";
        }
    }
    $moves++;
    ( $deck, $over, $spades, $hearts, $diamonds, $clubs, @stacks ) =
        split ' ', $layout;
}

sub playup    # does a card play up to the foundation
{
    my ( $number, $suit ) = $_[0] =~ /(.)(\w)$/ or return 0;
    my $found = $found{$suit} or die "$layout\nnumber $number suit <$suit>\n";
    return exists $rank{ substr( $$found, 0, 1 ) . $number } && $found;
}

sub showwin
{
    my @answer;
    while ($layout)
    {
        push @answer, $layout;
        $layout = $state{$layout};
    }
    my $n = @answer;

    #print reverse @answer;
    write_file 'd.width', join "\n", reverse @answer;
    print "cycles: $cycle steps: $n\n";
    exit;
}

sub move
{
    $moves = 0;

    if ( my $found = playup($over) )    # play over to foundation
    {
        $over =~ s/(..)?(..)$/uc $1 or '--'/e;
        $$found = $2;
        addmove();
    }

    if ( my ( $fn, $fs ) = $over =~ /([2-9TJQ])(\w)$/ )    # over to stack
    {
        my ($number) = $rank =~ /$fn(.)/ or die "fn $fn fs $fs\n$layout ";
        my $pat = qr/$number$wantsuit{$fs}$/;
        for my $stack ( 0 .. $#stacks )
        {
            if ( $stacks[$stack] =~ /$pat/ )
            {
                $over =~ s/(..)?(.\w)$/uc $1 or '--'/e
                    or die "over <$over>\n$layout ";
                $stacks[$stack] .= $2;
                addmove();
            }
        }
    }

    if ( my @which = grep $stacks[$_] eq '--',
        0 .. $#stacks )    # when empty stack
    {
        if ( $over =~ s/(..)?(K.)$/uc $1 or '--'/e )    # over K to stack
        {
            $stacks[ $which[0] ] = $2;
            addmove();
        }
        for my $stack ( 0 .. $#stacks )    # move stack K to empty stack
        {
            if ( $stacks[$stack] =~ s/(..)(K.*)/\U$1/ )
            {
                $stacks[ $which[0] ] = $2;
                addmove();
            }
        }
    }

    for my $from ( 0 .. $#stacks )         # play from stack to foundation
    {
        if ( my $found = playup( $stacks[$from] ) )
        {
            $stacks[$from] =~ s/(..)?(..)$/uc $1 or '--'/e or die "$layout ";
            $$found = $2;
            addmove();
        }
    }

    for my $to ( 0 .. $#stacks )           # stack to stack
    {
        my ( $tn, $ts ) = $stacks[$to] =~ /(\w)(\w)$/ or next;
        my ($before) = $rank =~ /(.)$tn/ or die "tn $tn ts $ts\n$layout ";
        $before or die "no before in\n$layout ";
        my $wantsuit = $wantsuit{$ts} or die "tn $tn ts $ts";
        for my $from ( 0 .. $#stacks )
        {
            if ( $stacks[$from] =~
                s/(^|.[shdc])($before$wantsuit.*)/uc $1 or '--'/e )
            {
                $stacks[$to] .= $2;
                addmove();
            }
            elsif ( $stacks[$from] =~ /(..)$before$wantsuit/ and playup($1) )
            {
                $stacks[$from] =~ s/(..)($before$wantsuit.*)/uc $1 or '--'/e;
                $stacks[$to] .= $2;
                addmove();
            }
        }
    }

    if ($moves)
    {
        $eachfile and append_file $eachfile, "moves: $moves\n";
    }
    elsif ( $deck =~ s/(..)?(\w\w)$/$1 or '--'/e )    # deck has cards
    {
        $over = ( $over ne '--' && lc($over) ) . uc $2;
        addmove();
    }
    elsif ( $over =~ s/(\w\w)(.+)/\U$1/ )             # restart deck from over
    {
        $deck = lc join '', reverse $2 =~ /../g;      # reverse by twos
        addmove();
    }
}

package FreeCell::State;

use strict;

use FreeCell::Object;
use FreeCell::Card;

use vars qw(@ISA);
@ISA=qw(FreeCell::Object);

sub initialize
{
    my $self = shift;

    $self->{'stacks'} = (map { [] } (1 .. 8));
    $self->{'founds'} = [ 0, 0, 0, 0];
    $self->{'fc'} = (map { FreeCell::Card->new(); } (1 .. 4));

    return 0;
}

sub read_from_file
{
    my $self = FreeCell::State->new();
    
    my $fh = shift;
    my $line;
    
    while($line = $fh->getline())
    {
        if ($line =~ /^Foundations/)
        {
            last;
        }
    }    
    $line =~ s/^Foundations: //;
    $line =~ /H-(.) C-(.) D-(.) S-(.)/;
    $self->{'founds'} = [ map { FreeCell::Card::get_card_value_from_string($_) } ($1,$2,$3,$4) ];
    $line = $fh->getline();
    if ($line !~ /^Freecells/)
    {
        die "Incorrect state format";
    }
    $line =~ s/^Freecells: *//;
    $line =~ s/ *$//;
    $self->{'fc'} = [ map { ($_ eq "-") ? 0 : Freecell::Card::from_string($_) } split(/\s+/, $line) ];

    my (@stacks);
    while ($line = $fh->getline())
    {
        if ($line !~ /^:/)
        {
            last;
        }
        $line =~ s/^: *//;
        $line =~ s/ *$//;
        push @stacks, [ map { FreeCell::Card::from_string($_) } split(/\s+/, $line) ];
    }

    $self->{'stacks'} = [ @stacks ];

    return $self;    
}

sub verify_and_perform_simple_simon_move
{
    my $self = shift;

    my $move = shift;

    if ($move->{'type'} eq "stack_to_stack")
    {
        my $src = $move->{'source'};
        my $dest = $move->{'dest'};
        my $num_cards = $move->{'num_cards'};

        my $stacks = $self->{'stacks'};

        my $ss = $stacks->[$src];

        my $stack_len = scalar(@$ss);

        if ($stack_len < $num_cards)
        {
            die "Not enough cards in stack";
        }

        my $num_true_seqs = 1;
        
        for my $i (($stack_len-$num_cards+1) .. ($stack_len-1))
        {
            if ($ss->[$i]->{'v'}+1 != $ss->[$i-1]->{'v'})
            {
                die "Not a proper sequence at position $i!";
            }
            $num_true_seqs += (($ss->[$i]->{'s'} != $ss->[$i-1]->{'s'}) ? 1 : 0);
        }

        my $ds = $stacks->[$dest];

        if ((scalar(@$ds) == 0) || $ss->[$stack_len-$num_cards]->{'v'} != $ds->[$#$ds]->{'v'}+1)
        {
            # Do nothing.
        }
        else
        {
            die "Putting a sequence on top of an invalid parent!";
        }

        my $num_free_stacks = scalar(grep { (scalar(@{$stacks->[$_]}) == 0) && ($_ ne $src) && ($_ ne $dest) } (0 .. $#$stacks));
        if ($num_true_seqs > (1 << $num_free_stacks))
        {
            die "Cannot move sequence - not enough free stacks.";
        }

        push @$ds, @$ss[($stack_len - $num_cards) .. ($stack_len-1)];
        @$ss = @$ss[0 .. ($stack_len - $num_cards-1)];
    }
    elsif ($move->{'type'} eq "seq_to_founds")
    {
        my $src = $move->{'source'};
        my $stacks = $self->{'stacks'};
        my $ss = $stacks->[$src];
        my $stack_len = scalar(@$ss);
        if ($stack_len < 13)
        {
            die "Improper sequence to foundations move. Not enough cards.";
        }
        my $suit = $ss->[$stack_len-13]->{'s'};
        for my $i (0 .. 12)
        {
            my $card = $ss->[$stack_len-13+$i];
            if (($card->{'s'} != $suit) ||
                ($card->{'v'} != 13-$i))
            {
                die "Improper sequence in a seq->found move. ($i)";
            }
        }
        for my $i (1 .. 13)
        {
            pop(@$ss);
        }
        $self->{'founds'}->[$suit] = 13;
    }
}

sub compare
{
    my $state1 = shift;
    my $state2 = shift;

    my $stacks1 = $state1->{'stacks'};
    my $stacks2 = $state2->{'stacks'};
    for my $i (0 .. $#$stacks1)
    {
        my $s1 = $stacks1->[$i];
        my $s2 = $stacks2->[$i];
        for my $j (0 .. $#$s1)
        {
            if ($s1->[$j]->{'s'} != $s2->[$j]->{'s'})
            {
                die "State Comparison: Incorrect Card Suit ($i,$j)";
            }
            if ($s1->[$j]->{'v'} != $s2->[$j]->{'v'})
            {
                die "State Comparison: Incorrect Card Value ($i,$j)";
            }
        }
    }

    my $f1 = $state1->{'founds'};
    my $f2 = $state1->{'founds'};
    for my $i (0 .. $#$f1)
    {
        if ($f1->[$i] != $f2->[$i])
        {
            die "State comparison: Incorrect value in Foundations ($i)";
        }
    }

    return 0;
}

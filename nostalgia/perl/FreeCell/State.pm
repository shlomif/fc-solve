package FreeCell::State;

use FreeCell::Cards;

# Card Format:
# 1,2,3,4,5,6,7,8,9,10, 11(J), 12(Q), 13(K)
# 0 (h), 2 (s), 3 (d), 4 (c) - Hearts, spades, diamonds, clubs

sub pack_stack
{
	my $stack = shift;

	return join(",", @{$stack});
}

sub pack_state
{
	my $state = shift;

	my ($packed, $stack);

	$packed .= join(";",
	(map
		{ &pack_stack($_); }
		@{$state->{'stacks'}}
	)
	);

	$packed .= "|";

	$packed .= join(";", @{$state->{'freecells'}});

	$packed .= "|";

	$packed .= join(";", @{$state->{'decks'}});

	#$packed .= "|";

	#$packed .= join(";", @{$state->{'restricted_cards'}});

	return $packed;
}

sub unpack_state
{
	my $packed = shift;

	my ($stacks_str, $freecells_str, $decks_str, $restricted_str) = split(/\|/, $packed);

	return
	{
		'stacks' => [ (map { [ split(/,/, $_) ] } (split(/;/, $stacks_str,20))) ] ,
		'freecells' => [ split(/;/, $freecells_str, 20) ],
		'decks' => [ split(/;/, $decks_str) ],
	#	'restricted_cards' => [ split(/;/, $restricted_str, 20) ],
	};
}

sub duplicate_state
{
	my $state = shift;

	return unpack_state(pack_state($state));
}

sub compare
{
	my $state1 = shift;

	my $state2 = shift;

	my $s;

	foreach $s ($state1,$state2)
	{
		$s = duplicate_state($s);

		$s->{'stacks'} = [ sort { $a->[0] cmp $b->[0] } @{$s->{'stacks'}} ];

		$s->{'freecells'} = [ sort {$a cmp $b } @{$s->{'freecells'}} ] ;
	}

	return (pack_state($state1) cmp pack_state($state2));
}

sub as_string
{
	my $state = shift;

	my $string = "";

	$string .=
	sprintf("%3s %3s %3s %3s        H-%1s S-%1s D-%1s C-%1s\n\n",
		(map { &FreeCell::Cards::perl2user($_); } @{$state->{'freecells'}}),
		(map { &FreeCell::Cards::p2u_card_number($_); } @{$state->{'decks'}})
	);

	my ($card_num, $s, $max_num_cards);

	$max_num_cards = 0;
	for($s=0;$s<8;$s++)
	{
		if (scalar(@{$state->{'stacks'}->[$s]}) > $max_num_cards)
		{
			$max_num_cards = scalar(@{$state->{'stacks'}->[$s]});
		}
	}

	for($card_num = 0 ; $card_num < $max_num_cards ; $card_num++)
	{
		$string .= "  ";
		for ($s = 0; $s < 8 ; $s++)
		{
			if ($card_num >= scalar(@{$state->{'stacks'}->[$s]}))
			{
				$string .= "    ";
			}
			else
			{
				$string .= sprintf("%3s", FreeCell::Cards::perl2user($state->{'stacks'}->[$s]->[$card_num])) . " ";
			}
		}
		$string .= "\n";
	}

	return $string;
}

1;

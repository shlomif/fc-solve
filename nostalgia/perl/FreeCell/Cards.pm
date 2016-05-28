package FreeCell::Cards;

use strict;

{
	my %card_map_1 = (qw(A 1 J 11 Q 12 K 13));
	my %card_map_2 = (qw(H 0 S 1 D 2 C 3));

	my %card_map_3 = reverse(%card_map_1);
	my %card_map_4 = reverse(%card_map_2);

	sub u2p_card_number
	{
		my $rest = shift;

		my $card;

        	if (exists($card_map_1{$rest}))
		{
			$card = $card_map_1{$rest};
		}
		else
		{
			$card = $rest;
		}
		return $card
	}

	sub u2p_deck
	{
		my $deck = shift;

		return $card_map_2{$deck};
	}

sub user2perl
{
	my $user = shift;

	$user = uc($user);

	my $deck = substr($user, length($user)-1);

	my $card;

	my $rest = substr($user, 0, length($user)-1);

	$card = u2p_card_number($rest);

        return $card . '-' . u2p_deck($deck);
}

	sub p2u_card_number
	{
		my $card = shift;

		if (exists($card_map_3{$card}))
		{
			return $card_map_3{$card};
		}

		return $card;
	}

	sub p2u_deck
	{
		my $deck = shift;

		return $card_map_4{$deck};
	}

sub perl2user
{
	my $perlcard = shift;

	my ($card, $deck) = split(/-/, $perlcard);

	return (p2u_card_number($card) . p2u_deck($deck));

}
}

1;

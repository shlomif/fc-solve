package FreeCell::Card;

use strict;

use FreeCell::Object;

use vars qw(@ISA);

@ISA=qw(FreeCell::Object);

sub initialize
{
    my $self = shift;

    $self->{'s'} = 0;
    $self->{'v'} = 0;
    
    return 0;
}

use vars qw(%suit_u2p_map %suit_p2u_map %value_u2p_map %value_p2u_map);

%suit_u2p_map = ('H' => 0, 'C' => 1, 'D' => 2, 'S' => 3);
%suit_p2u_map = reverse(%suit_u2p_map);
%value_u2p_map = 
    (
        '0' => 0,
        (map { $_ => $_ } (2 .. 9)), 
        'A' => 1, 
        'J' => 11, 
        'Q' => 12, 
        'K' => 13, 
        'T' => 10,
    );

%value_p2u_map = reverse(%value_u2p_map);

sub from_string
{
    my $string = shift;

    my $card = FreeCell::Card->new();

    $string =~ s/^\s+//;
    
    if ($string eq "")
    {
        return $card;
    }
    
    $card->{'v'} = $value_u2p_map{substr($string,0,1)};
    $card->{'s'} = $suit_u2p_map{substr($string,1,1)};

    return $card;
}

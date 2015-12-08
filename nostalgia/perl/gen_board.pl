use FreeCell::Cards;

srand();

my @cards;

for($a=0;$a<52;$a++)
{
    $cards[$a] = $a;
}

for($a=0;$a<$ARGV[0];$a++)
{
    $one = int(rand(52));
    $two = int(rand(52));

    $temp = $cards[$one];
    $cards[$one] = $cards[$two];
    $cards[$two] = $temp;
}

for($a=0;$a<4;$a++)
{
    print join(" ",
    (map
        { FreeCell::Cards::perl2user((($_ % 13)+1) . '-' . int($_ / 13));}
        @cards[$a*7 .. ($a*7+6)])), "\n";
}

for($a=0;$a<4;$a++)
{
    print join(" ",
    (map
        { FreeCell::Cards::perl2user((($_ % 13)+1) . '-' . int($_ / 13));}
        @cards[28+$a*6 .. (28+($a*6)+5)])), "\n";
}



#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 59;
use Games::Solitaire::Verify::Move;

{
    my $move = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from stack 3 to the foundations",
            game => "freecell",
        }
    );

    # TEST
    ok ($move, "Checking that ::Move->new works");

    # TEST
    is ($move->source_type(), "stack", "source_type() is stack");

    # TEST
    is ($move->dest_type(), "foundation", "dest_type() is foundation");

    # TEST
    is ($move->source(), 3, "source() is 3");
}

{
    my $move = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from stack 0 to the foundations",
            game => "freecell",
        }
    );

    # TEST
    ok ($move, "Checking that ::Move->new works");

    # TEST
    is ($move->source_type(), "stack", "source_type() is stack");

    # TEST
    is ($move->dest_type(), "foundation", "dest_type() is foundation");

    # TEST
    is ($move->source(), 0, "source() is 0");
}

{
    my $name = "FC2->Found";

    my $move = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from freecell 2 to the foundations",
            game => "freecell",
        }
    );

    # TEST
    ok ($move, "$name : checking that ::Move->new works");

    # TEST
    is ($move->source_type(), "freecell", "$name : source_type() is frecell");

    # TEST
    is ($move->dest_type(), "foundation", "$name : dest_type() is foundation");

    # TEST
    is ($move->source(), 2, "$name : source() is 2");
}

{
    my $move;
    eval { 
        $move = Games::Solitaire::Verify::Move->new(
            {
                fcs_string => "This is not a legal move. QRXTOK0JH=%OVK",
                game => "freecell",
            }
        );
    };
    
    my $e = Exception::Class->caught();

    # TEST
    isa_ok ($e, "Games::Solitaire::Verify::Exception::Parse::FCS",
        "Caught an exception that's 'FCS'"
    );
}

{
    my $name = "FC0->Found";

    my $move = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from freecell 0 to the foundations",
            game => "freecell",
        }
    );

    # TEST
    ok ($move, "$name : checking that ::Move->new works");

    # TEST
    is ($move->source_type(), "freecell", "$name : source_type() is frecell");

    # TEST
    is ($move->dest_type(), "foundation", "$name : dest_type() is foundation");

    # TEST
    is ($move->source(), 0, "$name : source() is 0");
}



{
    my $name = "FC3->S2";

    my $move = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from freecell 3 to stack 2",
            game => "freecell",
        }
    );

    # TEST
    ok ($move, "$name : checking that ::Move->new works");

    # TEST
    is ($move->source_type(), "freecell", "$name : source_type() is frecell");

    # TEST
    is ($move->dest_type(), "stack", "$name : dest_type() is stack");

    # TEST
    is ($move->source(), 3, "$name : source() is 3");

    # TEST
    is ($move->dest(), 2, "$name : dest() is 2");
}


{
    my $name = "FC0->S2";

    my $move = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from freecell 0 to stack 2",
            game => "freecell",
        }
    );

    # TEST
    ok ($move, "$name : checking that ::Move->new works");

    # TEST
    is ($move->source_type(), "freecell", "$name : source_type() is frecell");

    # TEST
    is ($move->dest_type(), "stack", "$name : dest_type() is stack");

    # TEST
    is ($move->source(), 0, "$name : source() is 0");

    # TEST
    is ($move->dest(), 2, "$name : dest() is 2");
}

{
    my $name = "FC0->S0";

    my $move = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from freecell 0 to stack 0",
            game => "freecell",
        }
    );

    # TEST
    ok ($move, "$name : checking that ::Move->new works");

    # TEST
    is ($move->source_type(), "freecell", "$name : source_type() is frecell");

    # TEST
    is ($move->dest_type(), "stack", "$name : dest_type() is stack");

    # TEST
    is ($move->source(), 0, "$name : source() is 0");

    # TEST
    is ($move->dest(), 0, "$name : dest() is 0");
}

{
    my $name = "FC3->S0";

    my $move = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from freecell 3 to stack 0",
            game => "freecell",
        }
    );

    # TEST
    ok ($move, "$name : checking that ::Move->new works");

    # TEST
    is ($move->source_type(), "freecell", "$name : source_type() is frecell");

    # TEST
    is ($move->dest_type(), "stack", "$name : dest_type() is stack");

    # TEST
    is ($move->source(), 3, "$name : source() is 3");

    # TEST
    is ($move->dest(), 0, "$name : dest() is 0");
}



{
    my $name = "S4->FC3";

    my $move = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from stack 4 to freecell 3",
            game => "freecell",
        }
    );

    # TEST
    ok ($move, "$name : checking that ::Move->new works");

    # TEST
    is ($move->source_type(), "stack", "$name : source_type() is stack");

    # TEST
    is ($move->dest_type(), "freecell", "$name : dest_type() is freecell");

    # TEST
    is ($move->source(), 4, "$name : source() is 4");

    # TEST
    is ($move->dest(), 3, "$name : dest() is 3");
}

{
    my $name = "S4->FC0";

    my $move = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from stack 4 to freecell 0",
            game => "freecell",
        }
    );

    # TEST
    ok ($move, "$name : checking that ::Move->new works");

    # TEST
    is ($move->source_type(), "stack", "$name : source_type() is stack");

    # TEST
    is ($move->dest_type(), "freecell", "$name : dest_type() is freecell");

    # TEST
    is ($move->source(), 4, "$name : source() is 4");

    # TEST
    is ($move->dest(), 0, "$name : dest() is 0");
}



{
    my $name = "Stack->Stack[num==1]";

    my $move = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move 1 cards from stack 7 to stack 2",
            game => "freecell",
        }
    );

    # TEST
    ok ($move, "$name : checking that ::Move->new works");

    # TEST
    is ($move->source_type(), "stack", "$name : source_type() is stack");

    # TEST
    is ($move->dest_type(), "stack", "$name : dest_type() is stack");

    # TEST
    is ($move->source(), 7, "$name : source() is 7");

    # TEST
    is ($move->dest(), 2, "$name : dest() is 2");

    # TEST
    is ($move->num_cards(), 1, "$name : num_cards == 1");
}

{
    my $name = "Stack0->Stack3[num==1]";

    my $move = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move 1 cards from stack 0 to stack 3",
            game => "freecell",
        }
    );

    # TEST
    ok ($move, "$name : checking that ::Move->new works");

    # TEST
    is ($move->source_type(), "stack", "$name : source_type() is stack");

    # TEST
    is ($move->dest_type(), "stack", "$name : dest_type() is stack");

    # TEST
    is ($move->source(), 0, "$name : source() is 0");

    # TEST
    is ($move->dest(), 3, "$name : dest() is 3");

    # TEST
    is ($move->num_cards(), 1, "$name : num_cards == 1");
}

#!/usr/bin/perl

use strict;
use warnings;

my $prefix = "";
PREFIX_LOOP:
while (<>)
{
    if (/\A {4}tests_define_accessors\(\);/)
    {
        last PREFIX_LOOP;
    }
    $prefix .= $_;
}
my $rest_of_func = $_;

my $extra_var_declares = "";

my %var_map =
(
    'state,stack_idx' => "col",
    'state,ds' => "dest_col",
    'state,clear_junk_dest_stack' => "clear_junk_dest_col",
);


while (<>)
{
    if (my ($inner_shift, $len_var, $state_var, $index_var) =
        /\A( +)(\w+) *= *fcs_stack_len *\( *(\w+) *, *(\w+) *\) *; *\Z/
       )
    {
        my $indent = $inner_shift;
        $indent =~ s/ {4}//;
        my $col_var = $var_map{"$state_var,$index_var"} or
            die "Unknown index var '$state_var,$index_var'";
        $extra_var_declares .= "    fcs_cards_column_t $col_var;\n";
        $rest_of_func .=
            "$inner_shift$col_var = "
            . "fcs_state_get_col($state_var, $index_var);\n"
            ;
        $rest_of_func .=
            "$inner_shift$len_var = fcs_cards_column_len($col_var);\n";

        TO_END_OF_BLOCK:
        while (<>)
        {
            s{(fcs_stack_card(?:_num)?)\(\Q$state_var\E *, *\Q$index_var\E *, *([^\),]+)\)}{
                (($1 eq "fcs_stack_card") ? "fcs_cards_column_get_card" : "fcs_cards_column_get_card_num")
                . "($col_var, $2)"
            }ge;
            $rest_of_func .= $_;
            if (/\A$indent\}/)
            {
                last TO_END_OF_BLOCK;
            }
        }
    }
    else
    {
        $rest_of_func .= $_;
    }

}

print $prefix, $extra_var_declares, $rest_of_func;

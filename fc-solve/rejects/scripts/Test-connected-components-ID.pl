#!/usr/bin/perl

use strict;
use warnings;

use Games::Solitaire::Verify::Solution;

open my $dump_fh, '<', '982.dump'
    or die "Cannot open 982.dump";

my $initial_state_str;
my $initial_board;

my $two_fc_variant = Games::Solitaire::Verify::VariantsMap->new->get_variant_by_id('freecell');

$two_fc_variant->num_freecells(2);

my %components = ();
my %states_to_components = ();

while (my $line = <$dump_fh>)
{
    if (($line =~ m{^Foundations}) .. ($line !~ /\S/))
    {
        my $state_str = '';
        while ($line !~ /\S/)
        {
            $state_str .= $line;
            $line = <$dump_fh>;
        }
        # $state_str is now ready.
        if (!defined($initial_state_str))
        {
            $initial_state_str = $state_str;
            $initial_board = Games::Solitaire::Verify::State->new(
                {
                    string => $initial_state_str,
                    variant => "custom",
                    variant_params => $two_fc_variant,
                }
            );
        }

        my $board = Games::Solitaire::Verify::State->new(
            {
                string => $initial_state_str,
                variant => "custom",
                variant_params => $two_fc_variant,
            }
        );

        my $found_str = join(',',
            map { $board->get_foundation_value($_, 0) } (0 .. 3)
        );

        my @columns_non_free_lens;

        foreach my $col_idx (0 .. ($board->num_columns - 1))
        {
            my $col = $board->get_column($col_idx);

            my $get_non_free_len = sub {
                my $non_free_len = $col->len();

                while ($non_free_len > 1)
                {
                    my $child = $col->pos($non_free_len-1);
                    my $parent = $col->pos($non_free_len-2);

                    if (not (($child->color() ne $parent->color())
                        &&
                        ($child->rank()+1 == $parent->rank())))
                    {
                        return $non_free_len;
                    }
                }
                continue
                {
                    $non_free_len--;
                }
                return 0;
            };

            push @columns_non_free_lens, $get_non_free_len->();
        }

        my $component_id = $found_str . ';' . join(',', @columns_non_free_lens);

        if (exists($states_to_components{$state_str}))
        {
            if ($states_to_components{$state_str} ne $component_id)
            {
                die "MisMATCH! <<<$state_str>>> ; <<<$component_id>>> ; <<<$states_to_components>>>";
            }
        }
        elsif (exists($components{$component_id}))
        {
            die "Two component IDs with different components - <<<$component_id>>>!";
        }
        else
        {
            $components{$component_id} = 1;
            # Do a BrFS scan on the fully connected component.

        }
    }
}

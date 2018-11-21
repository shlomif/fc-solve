package FC_Solve::SplitTests;

use strict;
use warnings;
use autodie;
use Data::Dumper      ();
use Test::Data::Split ();

sub gen
{
    my ( $self, $args ) = @_;

    my $module      = $args->{module};
    my $data_module = $args->{data_module};
    my $fn_prefix   = $args->{prefix};

    if ( $module !~ /\A[A-Z][a-zA-Z_0-9:]+\z/ )
    {
        die "module should be the name of a module.";
    }
    if ( $data_module !~ /\A[A-Z][a-zA-Z_0-9:]+\z/ )
    {
        die "data_module should be the name of a module.";
    }
    if ( $fn_prefix !~ /\A[a-z]+\z/ )
    {
        die "prefix is wrong.";
    }

    eval "use $data_module;";
    if ( my $error = $@ )
    {
        die $error;
    }

    my $data_obj = $data_module->new;

    Test::Data::Split->new(
        {
            target_dir  => 't',
            filename_cb => sub {
                my ( $self, $args ) = @_;

                return "${fn_prefix}--$args->{id}.t";
            },
            contents_cb => sub {
                my ( $self, $args ) = @_;
                my $data = $args->{data};
                my $dump = sub {
                    return Data::Dumper->new( [shift] )->Terse(1)->Indent(0)
                        ->Dump;
                };
                return <<"EOF";
#!/usr/bin/perl
use Test::More tests => 1;
use $module;
${module}::r(@{[$dump->(+{id => $args->{id},%{$data->{args}}}) . "," . $dump->($data->{msg})]});
EOF
            },
            data_obj => $data_obj,
        }
    )->run;

}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2009 Shlomi Fish

=cut

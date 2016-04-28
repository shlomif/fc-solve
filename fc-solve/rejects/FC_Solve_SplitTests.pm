package ..::rejects::FC_Solve_SplitTests;

use strict;
use warnings;
sub {
                my ($self, $args) = @_;
                my $id = $args->{id};
                my $id_quoted = quotemeta($id);
                my $data = Data::Dumper->new([$data_obj->lookup_data($id)])->Terse(1)->Indent(0)->Dump;
                return <<"EOF";
#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;

use $module;

# TEST
$module->run_id({ id => qq/$id_quoted/, data => $data, });

EOF
        }
        1;

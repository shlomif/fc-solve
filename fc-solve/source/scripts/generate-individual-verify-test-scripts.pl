#!/usr/bin/perl

# TODO :
# Merge with generate-individual-valgrind-test-scripts.pl
#
use strict;
use warnings;
use autodie;

use FindBin;
use lib "$FindBin::Bin/../t/t/lib";

use Test::Data::Split;
use FC_Solve::Test::Verify;

Test::Data::Split->new(
    {
        target_dir => 't',
        filename_cb => sub {
            my ($self, $args) = @_;

            return "verify--$args->{id}.t";
        },
        contents_cb => sub {
            my ($self, $args) = @_;
            my $id_quoted = quotemeta($args->{id});
            return <<"EOF";
#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;

use FC_Solve::Test::Verify;

# TEST
FC_Solve::Test::Verify->new->run_verify_id_test({ id => qq/$id_quoted/, });

EOF
        },
        data_obj => FC_Solve::Test::Verify->new,
    }
)->run;

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2009 Shlomi Fish

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut


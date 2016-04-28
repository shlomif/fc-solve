package FC_Solve::SplitTests;

use strict;
use warnings;
use autodie;

use Data::Dumper ();

use Test::Data::Split;

sub gen
{
    my ($self, $args) = @_;

    my $module = $args->{module};
    my $data_module = $args->{data_module};
    my $fn_prefix = $args->{prefix};

    if ($module !~ /\A[A-Z][a-zA-Z_0-9:]+\z/)
    {
        die "module should be the name of a module.";
    }
    if ($data_module !~ /\A[A-Z][a-zA-Z_0-9:]+\z/)
    {
        die "data_module should be the name of a module.";
    }
    if ($fn_prefix !~ /\A[a-z]+\z/)
    {
        die "prefix is wrong.";
    }

    eval "use $data_module;";

    my $data_obj = $data_module->new;

    Test::Data::Split->new(
        {
            target_dir => 't',
            filename_cb => sub {
                my ($self, $args) = @_;

                return "${fn_prefix}--$args->{id}.t";
            },
            contents_cb => ($args->{content_cb} // sub {
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
        }),
        data_obj => $data_obj,
    }
)->run;

}

1;

__END__

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


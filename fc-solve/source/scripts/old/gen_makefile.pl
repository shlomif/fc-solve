#!/usr/bin/perl

use strict;
use warnings;
use autodie;

sub min
{
    my $ret = shift;
    foreach (@_)
    {
        if ($_ < $ret)
        {
            $ret = $_;
        }
    }

    return $ret;
}

my @objects=(
    qw(alloc app_str check_and_add_state card cl_chop cmd_line fcs_dm fcs_hash fcs_isa),
    qw(freecell intrface lib lookup2 move pqueue preset rand),
    qw(scans simpsim state)
    );

my @targets = (
    {
        'exe' => "fc-solve",
        'objs' => [ qw(main) ],
    },
    {
        'exe' => "freecell-solver-range-parallel-solve",
        'objs' => [ qw(test_multi_parallel) ],
    },
);

my @headers_proto=
    (
        qw(alloc app_str check_and_add_state card cl_chop), { 'name' => "config", 'gen' => 1},
            qw(fcs_cl fcs fcs_dm fcs_enums),
        qw(fcs_hash fcs_isa fcs_move fcs_user inline jhjtypes lookup2),
        qw(move ms_ca), {'name' => "prefix", 'gen' => 1},
            qw(pqueue preset rand state check_limits tests)
    );

my @headers = (map { ref($_) eq "HASH" ? $_->{'name'} : $_ } @headers_proto);

my @defines=(qw(WIN32));

my @debug_defines = (qw(DEBUG));

print "all: fc-solve.exe freecell-solver-range-parallel-solve.exe\n\n";

print "OFLAGS=" . "/Og2 " . join(" ", (map { "/D".$_ } @defines)) . "\n";
print "DFLAGS=\$(OFLAGS) " . join(" ", (map { "/D".$_ } @debug_defines)) . "\n";

print "INCLUDES=" . join(" ", (map { $_.".h" } @headers)). "\n";
print "CC=cl\n";
print "LIB32=link.exe\n";


print "\n\n";

print "OBJECTS = " . join(" ", (map { $_.".obj" } @objects)) . "\n";

print "\n\n";

foreach my $o (@objects, (map { @{$_->{'objs'}} } @targets))
{
    print "$o.obj: $o.c \$(INCLUDES)\n";
    print "\t\$(CC) /c /Fo$o.obj \$(OFLAGS) $o.c\n";
    print "\n";
}

print "\n\n###\n### Final Targets\n###\n\n\n";

foreach my $t (@targets)
{
    my $exe = $t->{'exe'};
    my @objs = @{$t->{'objs'}};

    #my $obj_line = "\$(OBJECTS) " . join(" ", (map { "$_.obj" } @objs));
    my $obj_line = "freecell-solver-static.lib " . join(" ", (map { "$_.obj" } @objs));

    print "$exe.exe: $obj_line\n";
    print "\t\$(CC) /Fe$exe.exe /F0x2000000 $obj_line\n";
    print "\n";
}

print "freecell-solver-static.lib: \$(OBJECTS)\n";
print "\t\$(LIB32) -lib \$(OBJECTS) /out:freecell-solver-static.lib\n";
print "\n";

print "freecell-solver.dll: \$(OBJECTS) freecell-solver.def\n";
print "\t\$(LIB32) kernel32.lib user32.lib gdi32.lib /dll /out:freecell-solver.dll /implib:freeecell-solver.lib /DEF:freecell-solver.def \$(OBJECTS) \n";
print "\n";

#print "fc-solve.exe: \$(OBJECTS)\n";
#print "\t\$(CC) /Fefc-solve.exe /F0x2000000 \$(OBJECTS)\n";
#print "\n";

print "clean:\n";
print "\tdel *.obj *.exe *.lib *.dll *.exp\n";

sub my_edit_file
{
    my ($filename, $sub_ref) = @_;

    local $_;
    my $new_fn = "$filename.new";
    open my $in_fh, '<', $filename;
    open my $out_fh, '>', $new_fn;

    while (<$in_fh>)
    {
        $sub_ref->($in_fh, $out_fh, $_);
    }
    close ($in_fh);
    close ($out_fh);

    rename ($new_fn, $filename);
}

my_edit_file('Makefile.am',
    sub {
        my ($in_fh, $out_fh) = @_;
        if (/^libfreecell_solver_la_SOURCES *=/)
        {
            print {$out_fh} "libfreecell_solver_la_SOURCES = " . join(" ", (map { "$_.c" } @objects)) . "\n";
        }
        elsif (/^#<<<HEADERS\.START/)
        {
            while(! /^#>>>HEADERS\.END/)
            {
                $_ = <$in_fh>;
            }
            print {$out_fh} "#<<<HEADERS.START\n";
            my @headers_no_gen = map { (ref($_) eq "HASH") ? $_->{'name'} : $_ } (grep { (ref($_) eq "HASH") ? (! $_->{'gen'}) : 1 } @headers_proto);
            for my $i (0 .. ((int(scalar(@headers_no_gen)/5)+((scalar(@headers_no_gen)%5) > 0))-1))
            {
                print {$out_fh} "EXTRA_DIST += " . join(" ", map { "$_.h" } @headers_no_gen[($i*5) .. min($i*5+4, $#headers_no_gen)]) . "\n";
            }
            print {$out_fh} "#>>>HEADERS.END\n";
        }
        else
        {
            print {$out_fh} $_;
        }

        return;
    }
);

my_edit_file('Makefile.gnu',
    sub {
        my ($in_fh, $out_fh) = @_;
        if (/^#<<<OBJECTS\.START/)
        {
            while(! /^#>>>OBJECTS\.END/)
            {
                $_ = <$in_fh>;
            }
            print {$out_fh} "#<<<OBJECTS.START\n";
            print {$out_fh} "OBJECTS = " . (" " x 20) . "\\\n";
            print {$out_fh} join("", (map { sprintf((" " x 10) . "%-20s\\\n", ($_.".o")) } @objects));
            print {$out_fh} "\n";
            print {$out_fh} "#>>>OBJECTS.END\n";
        }
        else
        {
            print {$out_fh} $_;
        }

        return;
    }
);

my_edit_file('Makefile.lite',
    sub {
        my ($in_fh, $out_fh) = @_;
        if (/^INCLUDES *=/)
        {
            print {$out_fh} "INCLUDES = " . join(" ", (map { "$_.h" } @headers)) . "\n";
        }
        elsif (/^#<<<OBJECTS\.START/)
        {
            while(! /^#>>>OBJECTS\.END/)
            {
                $_ = <$in_fh>;
            }
            my @ext_objects = (@objects, "main");
            print {$out_fh} "#<<<OBJECTS.START\n";
            print {$out_fh} join("\n\n", (map { "$_.o: $_.c \$(INCLUDES)\n\t\$(CC) -c \$(OFLAGS) -o \$@ \$<" } @ext_objects));
            print {$out_fh} "\n\nOBJECTS = " . join(" ", (map { "$_.o" } @ext_objects)) . "\n";
            print {$out_fh} "#>>>OBJECTS.END\n";
        }
        else
        {
            print {$out_fh} $_;
        }
    }
);



=head1 COPYRIGHT AND LICENSE

Copyright (c) 2000 Shlomi Fish

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


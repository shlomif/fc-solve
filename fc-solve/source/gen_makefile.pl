#!/usr/bin/perl

use strict;

my @objects=(
    qw(alloc app_str caas card cmd_line fcs_dm fcs_hash fcs_isa freecell), 
    qw(intrface lib lookup2 move pqueue preset rand scans simpsim state)
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

my @headers=
    (
        qw(alloc app_str caas card config fcs_cl fcs fcs_dm fcs_enums),
        qw(fcs_hash fcs_isa fcs_move fcs_user jhjtypes), 
        qw(move ms_ca pqueue preset rand state test_arr tests)
    );

my @defines=(qw(FCS_STATE_STORAGE=FCS_STATE_STORAGE_INTERNAL_HASH FCS_STACK_STORAGE=FCS_STACK_STORAGE_INTERNAL_HASH WIN32));

my @debug_defines = (qw(DEBUG));

print "all: fc-solve.exe\n\n";

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


#!/usr/bin/perl

use strict;
use warnings;

# use File::Which;
# use File::Basename;
use Cwd;
use FindBin;
use File::Spec;
use File::Copy;
use File::Path;
use Getopt::Long;

my $tests_glob = "*.{exe,t}";

GetOptions(
    '--glob=s' => \$tests_glob,
);

{
    local $ENV{FCS_PATH} = Cwd::getcwd();

    my $testing_preset_rc;
    {
        my $preset_dest = File::Spec->catdir(
            File::Spec->curdir(), "t", "Presets"
        );
        my $preset_src = File::Spec->catfile(
            File::Spec->curdir(), "Presets"
        );
        my $pset_files_dest = File::Spec->catdir($preset_dest, "presets");
        my $pset_files_src  = File::Spec->catdir($preset_src, "presets");
        mkpath($pset_files_dest);
        opendir my $d, $pset_files_src
            or die "Cannot open $pset_files_src";
        while (defined(my $f = readdir($d)))
        {
            if ($f =~ m{\.sh\z})
            {
                copy(
                    File::Spec->catfile($pset_files_src, $f),
                    File::Spec->catfile($pset_files_dest, $f)
                );
            }
        }
        closedir($d);

        my $files_dest_abs = File::Spec->rel2abs(File::Spec->catfile($pset_files_dest, "STUB.TXT"));

        $files_dest_abs =~ s{STUB\.TXT\z}{};

        $testing_preset_rc = File::Spec->rel2abs(File::Spec->catfile($preset_dest, "presetrc"));

        open my $in, "<", File::Spec->catfile($preset_src, "presetrc");
        open my $out, ">", $testing_preset_rc;
        while (my $line = <$in>)
        {
            $line =~ s{\A(dir=)(.*)}{$1$files_dest_abs\n}ms;
            print {$out} $line;
        }
        close($in);
        close($out);
    }


    local $ENV{FREECELL_SOLVER_PRESETRC} = $testing_preset_rc;
    chdir("$FindBin::Bin/t");
    my @tests = glob("t/$tests_glob");

    {
        # local $ENV{FCS_PATH} = dirname(which("fc-solve"));
        print STDERR "FCS_PATH = $ENV{FCS_PATH}\n";
        exec("runprove", @tests);
    }
}

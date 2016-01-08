#!/usr/bin/ruby

$log = File.new("Binary-Size.log", "w")

def config(args)
    system("./Tatzer #{args}");
end

def measure_once(preset, stage)
    s = File.size("libfreecell-solver.so")
    $log.puts("#{preset} - #{stage} - #{s}")
    $log.flush()
end

def measure_size(preset)
    measure_once(preset, "before strip")
    system("strip * > /dev/null 2>&1")
    measure_once(preset, "after strip")
end

def config_and_measure(preset, args)
    config(args)
    system("make clean");
    system("make");
    measure_size(preset)
end

def mk_gnu_and_measure(preset, args)
    system("make -f Makefile.gnu clean");
    system("make -f Makefile.gnu all #{args} DEBUG=0 OPT_FOR_SIZE=1");
    measure_size(preset);
end

# This is in order to reset all the flags in config.h.
config("--nowith-context-var");

mk_gnu_and_measure("gcc-Os", "FREECELL_ONLY=0 DISABLE_SIMPLE_SIMON=0")
mk_gnu_and_measure("gcc-Os-fc-only", "FREECELL_ONLY=1")
mk_gnu_and_measure("gcc-Os-no-simple-simon",
                   "FREECELL_ONLY=0 DISABLE_SIMPLE_SIMON=1"
                  )
mk_gnu_and_measure("gcc-Os-fc-only-no-flips",
                   "FREECELL_ONLY=1 WITHOUT_CARD_FLIPS=1"
                  )
mk_gnu_and_measure("gcc-Os-fc-only-no-flips-no-ctx",
                   "FREECELL_ONLY=1 WITHOUT_CARD_FLIPS=1 WITHOUT_CONTEXT_VAR=1"
                  )

config_and_measure("default", "")
config_and_measure("release", "-r")
config_and_measure("r-fc-only", "-r --fc-only")
config_and_measure("r-no-simple-simon", "-r --disable-simple-simon")
config_and_measure("r-fc-only-arch-omit-frame",
                   "-r --fc-only --omit-frame --arch=pentium4"
                  )
config_and_measure("r-fc-only-omit-frame",
                   "-r --fc-only --omit-frame"
                  )
config_and_measure("r-no-simple-simon-omit-frame",
                   "-r --disable-simple-simon --omit-frame"
                  )


$log.close()

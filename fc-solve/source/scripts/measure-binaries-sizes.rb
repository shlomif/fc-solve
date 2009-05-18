#!/usr/bin/ruby

$log = File.new("Binary-Size.log", "w")

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
    system("./Tatzer #{args}");
    system("make clean");
    system("make");
    measure_size(preset)
end

def mk_gnu_and_measure(preset, args)
    system("make -f Makefile.gnu clean");
    system("make -f Makefile.gnu all #{args}");
    measure_size(preset);
end

config_and_measure("default", "")
config_and_measure("release", "-r")
config_and_measure("r-fc-only", "-r --fc-only")
config_and_measure("r-fc-only-arch-omit-frame", 
                   "-r --fc-only --omit-frame --arch=pentium4"
                  )
config_and_measure("r-fc-only-omit-frame",
                   "-r --fc-only --omit-frame"
                  )
mk_gnu_and_measure("gcc-Os", "FREECELL_ONLY=0 DEBUG=0")
mk_gnu_and_measure("gcc-Os-fc-only", "FREECELL_ONLY=1 DEBUG=0")

$log.close()

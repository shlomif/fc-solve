#!/bin/bash
d="FATPACK"
s="$(pwd)"
rm -fr "$d"
mkdir "$d"
cxsa="${s}/../Class-XSAccessor--replacement/lib/"
export PERL5LIB="${s}/lib:$cxsa"
(
    cd "$d"
    cp -a "$s/lib" ./lib
    rm -f lib/Games/Solitaire/Verify/App/CmdLine.pm
    rm -f lib/Games/Solitaire/Verify/App/CmdLine/Expand.pm
    mkdir lib/Class
    bn='Class/XSAccessor.pm'
    cp -f "$cxsa/$bn" lib/"$bn"
    e="$s/expand-moves.fatpacked.pl"
    fatpack pack "$s/../test-scripts/e1.pl" > "$e"
    perl -p -0777 -MSQ -E 's#\A#eval <<${sq}MY_JACK_SOPHIE_END${sq};\n#;s#\z#\nMY_JACK_SOPHIE_END\nprint \$\@;\n#;' < "$e" > "$s/with-wrap.pl"
)

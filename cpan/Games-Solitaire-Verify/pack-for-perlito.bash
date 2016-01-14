#!/bin/bash

git_repo="$HOME/progs/freecell/git"
gsv="$git_repo/fc-solve/cpan/Games-Solitaire-Verify"

inc()
{
    local path="$1"
    shift
    echo "-I" "$gsv/$path/lib"
}

BASE="MyDir"
BASE_LIB="MyDir/lib"

for bn in "Games-Solitaire-Verify" "POSIX--replacement" "Exception-Class--replacement" "Getopt-Long--replacement" "main-open--replacement" "List-Util--replacement" "Class-XSAccessor--replacement" ; do
    p="$gsv/$bn/lib"
    find "$p" -name '*.pm' | (while read fn ; do
        g="${fn##$p/}"
        mkdir -p "$BASE_LIB/$(dirname "$g")"
        perl "$gsv/process-perl-code-for-perlito.pl" < "$fn" > "$BASE_LIB/$g"
    done)
done
cat <<'EOF' > 'mk_acc_ref.pl'
sub mk_acc_ref
{
    my $pkg = shift;
    my $names = shift;

    my $mapping = +{ map { $_ => $_ } @$names };

    {
        foreach my $meth_name (keys %$mapping)
        {
            my $slot = $mapping->{$meth_name};

            {
                no strict;
                *{"${pkg}::$meth_name"} = sub {
                    my $self = shift;

                    if (@_)
                    {
                        $self->{$slot} = shift;
                    }
                    return $self->{$slot};
                };
            }
        }
    }
}
EOF
perl -lp -i -0777 -MSQ -MIO::All -E 's#^sub mk_acc_ref\n(.*?)\n}\n#io()->file("mk_acc_ref.pl")->all#ems' "$BASE_LIB"/Games/Solitaire/Verify/Base.pm

# perl -MCarp::Always perlito5.pl -Cjs -I src5/lib/ $(inc "Games-Solitaire-Verify") $(inc "POSIX--replacement") $(inc "Exception-Class--replacement") $(inc "Getopt-Long--replacement") $(inc "List-Util--replacement") $(inc "Class-XSAccessor--replacement")  "$gsv"/Games-Solitaire-Verify/bin/expand-solitaire-multi-card-moves
perl -MCarp::Always "$gsv/process-expand-code-for-perlito.pl" < "$gsv"/Games-Solitaire-Verify/bin/expand-solitaire-multi-card-moves > "$BASE_LIB"/expand-solitaire-multi-card-moves
cp -f "$gsv"/Games-Solitaire-Verify/bin/verify-solitaire-solution "$BASE_LIB"/verify-solitaire-solution
perl -MCarp::Always perlito5.pl --bootstrapping -Cjs -I src5/lib/ -I "$BASE_LIB" "$BASE_LIB"/expand-solitaire-multi-card-moves > foo.js
perl -MCarp::Always perlito5.pl --bootstrapping -Cjs -I src5/lib/ -I "$BASE_LIB" "$BASE_LIB"/verify-solitaire-solution > bar.js

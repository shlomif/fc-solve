common='-s -i -p -t -sam -sel'
filt()
{
    perl -lanE 's/[ \t]+$//; say if !/^Stored-/';
}
_gdb()
{
    gdb -args "$@"
    exit
}
_nogdb()
{
    "$@" | filt
}
board1="`pwd`/1.board"
board="`pwd`/24.board"
f()
{
    "$1" -l cpb $common "$board" | filt
}
f()
{
    _nogdb \
        "$1" \
--method a-star -asw 0.2,0.8,0,0,0 -step 500 --st-name 11 --flare-name 11 -nf \
--method a-star -to 0123467 -asw 0.5,0,0.3,0,0 -step 500 --st-name 18 --flare-name 18 -nf \
--flares-plan Run:200@18,Run:200@11,Run:200@18 \
$common "$board1"
}
f()
{
    "$1" --flare-name dfs \
        --next-flare --method a-star --flare-name befs \
        --flares-plan 'Run:300@dfs,Run:1000@befs,CP:,Run:100@dfs' \
        $common  <(pi-make-microsoft-freecell-board -t 6) | filt
}
# f ./fc-solve
(cd /usr && f fc-solve) > good
f ./fc-solve > bad
cmp good bad
read
nvim -O good bad
# ; gvimdiff <(f fc-solve ) <(f ./fc-solve)

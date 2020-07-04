function! Foo()
    let @e='</'.submatch(1).'_section>'
    return '<'.submatch(1).'_section id="'.submatch(2).'" title="'.submatch(3)."\">"
endfunction

function! Foo()
    let @e='{% endcall %}'
    return '{% call '.submatch(1).'_section( id="'.submatch(2).'", title="'.submatch(3).'" ) %}'
endfunction

map <F2> :s!^<\(h[0-9]\) id="\([^"]\+\)">\(.\+\)</\1>$!\=Foo()!<CR><CR>
fun RefactForEach()
    s/\(\S\+\)\.forEach( *(\(\w\+\) *) *=> *{/for (const \2 of \1) {/
    normal f{%
    s/});/}/
endf
command! RefactForEach call RefactForEach()
if 0
    map <F2> Oconsole.log("<<line no. <C-r>=line(".")<cr> at <c-r>% : trace>>");<esc>
endif

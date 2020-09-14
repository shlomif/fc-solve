function! Substitute_With_H2_H3_etc_HTML_Header_Section()
    let @e='</'.submatch(1).'_section>'
    return '<'.submatch(1).'_section id="'.submatch(2).'" title="'.submatch(3)."\">"
endfunction

function! Substitute_With_H2_H3_etc_HTML_Header_Section()
    let @e='{% endcall %}'
    return '{% call '.submatch(1).'_section( id="'.submatch(2).'", title="'.submatch(3).'" ) %}'
endfunction

map <F2> :s!^<\(h[0-9]\) id="\([^"]\+\)">\(.\+\)</\1>$!\=Substitute_With_H2_H3_etc_HTML_Header_Section()!<CR><CR>
fun RefactForEach()
    s/\(\S\+\)\.forEach( *(\(\w\+\) *) *=> *{/for (const \2 of \1) {/
    normal f{%
    s/});/}/
endf
command! RefactForEach call RefactForEach()
if 0
    map <F2> Oconsole.log("<<line no. <C-r>=line(".")<cr> at <c-r>% : trace>>");<esc>
endif
fun TsReplace()
    let pat = @/
    let funcname = substitute(pat, "\\v^\\\\\\<(.*)\\\\\\>$", "\\1", "")
    let basefunc = substitute(funcname, "\\v^freecell_solver_user", "user", "")
    echo basefunc
    /^export interface ModuleWrapper /
    exe "normal o\<c-r>=basefunc\<cr>: (...args: any) => any;\<esc>vi{:sort\<cr>"
    exe "/\\v^let " . funcname . " \\=/"
    .d
    /^export function FC_Solve_init_wrappers_with_module/
    let @/ = pat
    normal n
    exe "s/" . pat . "/ret." . basefunc . "/"
    exe "normal V%\"ad"
    /^    return ret;/
    normal k
    put a
    /\v^\}/
    normal 3j
    let @/ = pat
    exe ".,$s/" . pat . "/that.module_wrapper." . basefunc . "/g"
    normal gg
endf

map <F3> :call TsReplace()<cr>

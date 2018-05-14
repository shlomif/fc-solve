"use strict";
function mydef(x, y) {
    if (typeof define !== 'function') {
        return require('amdefine')(module)(x, y);
    } else {
        return define(x, y);
    }
}
mydef([((typeof WebAssembly) ? "libfreecell-solver.min" :
    "libfreecell-solver-asm")], function(M) {
    return M;
});

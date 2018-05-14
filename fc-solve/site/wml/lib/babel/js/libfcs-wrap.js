"use strict";
function mydef(x, y) {
    if (typeof define !== 'function') {
        return require('amdefine')(module)(x, y);
    } else {
        return define(x, y);
    }
}
// Taken from https://stackoverflow.com/questions/47879864/
// thanks!
const supported = (() => {
    try {
        if (typeof WebAssembly === "object"
            && typeof WebAssembly.instantiate === "function") {
            const module = new WebAssembly.Module(
                Uint8Array.of(0x0, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00));
            if (module instanceof WebAssembly.Module) {
                return (new WebAssembly.Instance(module) instanceof
                    WebAssembly.Instance);
            }
        }
    } catch (e) {
    }
    return false;
})();

mydef([(supported ? "libfreecell-solver.min" :
//mydef([(false ? "libfreecell-solver.min" :
    "libfreecell-solver-asm")], function(M) {
    return M;
});

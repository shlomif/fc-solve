export interface ModuleWrapper {
    fc_solve__hll_ms_rand__get_singleton: (...args: any) => any;
    fc_solve__hll_ms_rand__init: (...args: any) => any;
    fc_solve__hll_ms_rand__mod_rand: (...args: any) => any;
    Module: any;
}

export function base_calc_module_wrapper(Module: any): ModuleWrapper {
    const fc_solve__hll_ms_rand__get_singleton = Module.cwrap(
        "fc_solve__hll_ms_rand__get_singleton",
        "number",
        [],
    );
    const fc_solve__hll_ms_rand__init = Module.cwrap(
        "fc_solve__hll_ms_rand__init",
        "number",
        ["number", "string"],
    );
    const fc_solve__hll_ms_rand__mod_rand = Module.cwrap(
        "fc_solve__hll_ms_rand__mod_rand",
        "number",
        ["number", "number"],
    );
    return {
        fc_solve__hll_ms_rand__get_singleton,
        fc_solve__hll_ms_rand__init,
        fc_solve__hll_ms_rand__mod_rand,
        Module,
    };
}

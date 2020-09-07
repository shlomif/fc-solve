import bigInt from "big-integer";
import * as BaseApi from "./web-fcs-api-base";

let fc_solve_user__find_deal__alloc = null;
let fc_solve_user__find_deal__fill = null;
let fc_solve_user__find_deal__free = null;
let fc_solve_user__find_deal__run = null;

export function FC_Solve_init_wrappers_with_module(
    Module,
): BaseApi.ModuleWrapper {
    fc_solve_user__find_deal__alloc = Module.cwrap(
        "fc_solve_user__find_deal__alloc",
        "number",
        [],
    );
    fc_solve_user__find_deal__fill = Module.cwrap(
        "fc_solve_user__find_deal__fill",
        "number",
        ["number", "string"],
    );
    fc_solve_user__find_deal__free = Module.cwrap(
        "fc_solve_user__find_deal__free",
        "number",
        ["number"],
    );
    fc_solve_user__find_deal__run = Module.cwrap(
        "fc_solve_user__find_deal__run",
        "string",
        ["number", "string", "string"],
    );

    return BaseApi.base_calc_module_wrapper(Module);
}

export class Freecell_Deal_Finder {
    private obj: number;
    private abs_end: bigInt.BigInteger;
    private CHUNKM: bigInt.BigInteger;
    private start: bigInt.BigInteger;
    private update_cb: any;
    constructor(args) {
        const that = this;
        that.obj = fc_solve_user__find_deal__alloc();
    }

    public fill(str) {
        const that = this;
        fc_solve_user__find_deal__fill(that.obj, str);
        return;
    }

    public release() {
        fc_solve_user__find_deal__free(this.obj);
        return;
    }

    public run(abs_start, abs_end_param, update_cb) {
        const that = this;
        const CHUNK = bigInt(1000000);
        that.CHUNKM = CHUNK.add(bigInt.minusOne);
        const start = bigInt(abs_start);
        const abs_end = bigInt(abs_end_param);
        that.abs_end = abs_end;
        that.start = start;
        that.update_cb = update_cb;

        return;
    }

    public cont() {
        const that = this;
        const abs_end = that.abs_end;
        if (that.start.lesser(abs_end)) {
            that.update_cb({ start: that.start });
            let end = that.start.add(that.CHUNKM);
            if (end.gt(abs_end)) {
                end = abs_end;
            }
            const result = fc_solve_user__find_deal__run(
                that.obj,
                that.start.toString(),
                end.toString(),
            );
            if (result !== "-1") {
                return { found: true, result };
            }
            that.start = end.add(bigInt.one);
            return { found: false, cont: true };
        } else {
            return { found: false, cont: false };
        }
    }
}

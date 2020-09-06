import bigInt from "big-integer";
import * as BaseApi from "./web-fcs-api-base";
import { perl_range } from "./prange";

let fc_solve__hll_ms_rand__get_singleton = null;
let fc_solve__hll_ms_rand__init = null;
let fc_solve__hll_ms_rand__mod_rand = null;
let fc_solve_user__find_deal__alloc = null;
let fc_solve_user__find_deal__fill = null;
let fc_solve_user__find_deal__free = null;
let fc_solve_user__find_deal__run = null;

export function FC_Solve_init_wrappers_with_module(
    Module,
): BaseApi.ModuleWrapper {
    fc_solve__hll_ms_rand__get_singleton = Module.cwrap(
        "fc_solve__hll_ms_rand__get_singleton",
        "number",
        [],
    );
    fc_solve__hll_ms_rand__init = Module.cwrap(
        "fc_solve__hll_ms_rand__init",
        "number",
        ["number", "string"],
    );
    fc_solve__hll_ms_rand__mod_rand = Module.cwrap(
        "fc_solve__hll_ms_rand__mod_rand",
        "number",
        ["number", "number"],
    );
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

    return {
        fc_solve__hll_ms_rand__get_singleton,
        fc_solve__hll_ms_rand__init,
        fc_solve__hll_ms_rand__mod_rand,
        Module,
    };
}

/*
 * Microsoft Windows Freecell / Freecell Pro boards generation.
 *
 * See:
 *
 * - http://rosettacode.org/wiki/Deal_cards_for_FreeCell
 *
 * - http://www.solitairelaboratory.com/mshuffle.txt
 *
 * Under MIT/Expat Licence.
 *
 * */

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
/*
 * Microsoft C Run-time-Library-compatible Random Number Generator
 * Copyright by Shlomi Fish, 2011.
 * Released under the MIT/Expat License
 * ( http://en.wikipedia.org/wiki/MIT_License ).
 * */
class MSRand {
    private gamenumber: string;
    private rander: number;
    private module_wrapper: BaseApi.ModuleWrapper;
    constructor(args) {
        const that = this;
        that.module_wrapper = args.module_wrapper;
        that.gamenumber = args.gamenumber;
        that.rander = that.module_wrapper.fc_solve__hll_ms_rand__get_singleton();
        that.module_wrapper.fc_solve__hll_ms_rand__init(
            that.rander,
            "" + that.gamenumber,
        );
        return;
    }
    public max_rand(mymax) {
        const that = this;
        return that.module_wrapper.fc_solve__hll_ms_rand__mod_rand(
            that.rander,
            mymax,
        );
    }
    public shuffle(deck) {
        if (deck.length) {
            let i = deck.length;
            while (--i) {
                const j = this.max_rand(i + 1);
                const tmp = deck[i];
                deck[i] = deck[j];
                deck[j] = tmp;
            }
        }
        return deck;
    }
}
/*
 * Microsoft Windows Freecell / Freecell Pro boards generation.
 *
 * See:
 *
 * - http://rosettacode.org/wiki/Deal_cards_for_FreeCell
 *
 * - http://www.solitairelaboratory.com/mshuffle.txt
 *
 * Under MIT/Expat Licence.
 *
 * */

export function deal_ms_fc_board(module_wrapper: BaseApi.ModuleWrapper, seed) {
    const randomizer = new MSRand({
        module_wrapper: module_wrapper,
        gamenumber: seed,
    });
    const num_cols = 8;

    const columns = perl_range(0, num_cols - 1).map(() => {
        return [];
    });
    let deck = perl_range(0, 4 * 13 - 1);

    randomizer.shuffle(deck);

    deck = deck.reverse();

    for (let i = 0; i < 52; i++) {
        columns[i % num_cols].push(deck[i]);
    }

    function render_card(card) {
        const suit = card % 4;
        const rank = Math.floor(card / 4);

        return "A23456789TJQK".charAt(rank) + "CDHS".charAt(suit);
    }

    function render_column(col) {
        return ": " + col.map(render_card).join(" ") + "\n";
    }

    return columns.map(render_column).join("");
}

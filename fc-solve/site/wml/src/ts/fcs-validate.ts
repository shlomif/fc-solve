import { perl_range } from "./prange";
import { rank_re, suits__int_to_str, suit_re } from "./french-cards";
import { BoardTextLine, capitalize_cards } from "./capitalize-cards";

// Adapted from http://www.inventpartners.com/javascript_is_int - thanks.
function is_int(input: number): boolean {
    const value: string = "" + input;
    if (parseFloat(value) === parseInt(value, 10) && !isNaN(input)) {
        return true;
    } else {
        return false;
    }
}

const _ranks__int_to_str: string = "0A23456789TJQK";
export const ranks__str_to_int = {};
export const NUM_SUITS: number = 4;
const _suits: number[] = perl_range(0, NUM_SUITS - 1);
export const MIN_RANK: number = 1;
export const MAX_RANK: number = 13;
const _ranks: number[] = perl_range(MIN_RANK, MAX_RANK);

for (const rank of _ranks) {
    ranks__str_to_int[_ranks__int_to_str.substring(rank, rank + 1)] = rank;
}
export let suits__str_to_int = new Map<string, number>();
for (const suit of _suits) {
    suits__str_to_int.set(suits__int_to_str.substring(suit, suit + 1), suit);
}

class Card {
    constructor(
        private rank: number,
        private suit: number,
    ) {
        if (!is_int(rank)) {
            throw "rank is not an integer.";
        }
        if (!is_int(suit)) {
            throw "suit is not an integer.";
        }
        if (rank < 1) {
            throw "rank is too low.";
        }
        if (rank > MAX_RANK) {
            throw "rank is too high.";
        }
        if (suit < 0) {
            throw "suit is negative.";
        }
        if (suit >= NUM_SUITS) {
            throw "suit is too high.";
        }
    }

    public getRank(): number {
        return this.rank;
    }

    public getSuit(): number {
        return this.suit;
    }

    public toString(): string {
        return (
            _ranks__int_to_str.substring(this.rank, this.rank + 1) +
            suits__int_to_str.substring(this.suit, this.suit + 1)
        );
    }
}

class Column {
    constructor(private cards: Card[]) {}

    public getLen(): number {
        return this.cards.length;
    }

    public getCard(idx: number) {
        const that = this;
        if (idx < 0) {
            throw "idx is below zero.";
        }
        if (idx >= that.getLen()) {
            throw "idx exceeds the length of the column.";
        }
        return that.cards[idx];
    }

    public getArrOfStrs(): string[] {
        const that = this;
        return perl_range(0, that.getLen() - 1).map((i) => {
            return that.getCard(i).toString();
        });
    }
    public toString(): string {
        const that = this;
        return (
            Array.prototype.concat
                .apply([], [[":"], that.getArrOfStrs()])
                .join(" ") + "\n"
        );
    }
}

const card_re: string = "(" + rank_re + ")(" + suit_re + ")";
export function fcs_js__card_from_string(s: string): Card {
    const m = s.match("^" + card_re + "$");
    if (!m) {
        throw 'Invalid format for a card - "' + s + '"';
    }
    return new Card(ranks__str_to_int[m[1]], suits__str_to_int.get(m[2]));
}

class BaseResult {
    constructor(
        public is_correct: boolean,
        public start_char_idx: number,
        public num_consumed_chars: number,
        public error: string,
    ) {}

    public getEnd(): number {
        return this.start_char_idx + this.num_consumed_chars;
    }
}

class ColumnParseResult extends BaseResult {
    public col: Column;

    constructor(
        is_correct: boolean,
        start_char_idx: number,
        num_consumed_chars: number,
        error: string,
        cards: Card[],
    ) {
        super(is_correct, start_char_idx, num_consumed_chars, error);
        this.col = new Column(cards);
    }

    public getLen(): number {
        return this.col.getLen();
    }
    public toString(): string {
        return this.col.toString();
    }
}

class StringParser {
    private consumed: number = 0;

    constructor(private s: string) {}

    public consume(m: RegExpMatchArray): void {
        const that = this;
        const len_match: number = m[1].length;
        that.consumed += len_match;
        that.s = that.s.substring(len_match);

        return;
    }

    public getConsumed(): number {
        return this.consumed;
    }

    public isNotEmpty(): boolean {
        return this.s.length > 0;
    }

    public match(re: any): RegExpMatchArray {
        return this.s.match(re);
    }

    public consume_match(re: any): RegExpMatchArray {
        const that = this;
        const m = that.match(re);
        if (m) {
            that.consume(m);
        }
        return m;
    }
    public skipComments(): void {
        const that = this;
        that.consume_match(/^((?:[ \t]*#[^\n\r]*\r?\n)*)/);
        return;
    }
}

class CardsStringParser<CardType> extends StringParser {
    public cards: CardType[] = [];
    private is_start: boolean = true;

    constructor(
        s: string,
        private card_mapper: (string) => CardType,
    ) {
        super(s);
    }

    public afterStart(): void {
        this.is_start = false;

        return;
    }

    public getStartSpace(): string {
        return this.is_start ? "" : " +";
    }

    public should_loop(): boolean {
        const that = this;
        return (
            that.isNotEmpty() && !that.consume_match(/^(\s*(?:#[^\n]*)?\n?)$/)
        );
    }

    public add(m: RegExpMatchArray): void {
        this.cards.push(this.card_mapper(m[2]));

        this.afterStart();
        return;
    }

    public loop(re: any, callback: () => any): any {
        const p = this;

        while (p.should_loop()) {
            const m = p.consume_match(
                "^(" + p.getStartSpace() + "(" + re + ")" + ")",
            );
            if (!m) {
                p.consume_match("^( *)");

                return callback();
            }
            p.add(m);
        }
        return null;
    }
}

function calc_1H_error_string(suit: string): string {
    return 'Wrong rank specifier "1" (followed by "[R]"). Perhaps you meant either "A[R]" (for ace) or "T[R]" (for rank ten).'.replace(
        /\[R\]/g,
        suit,
    );
}

export function fcs_js__column_from_string(
    start_char_idx: number,
    orig_s: string,
    force_leading_colon: boolean,
): ColumnParseResult {
    const p = new CardsStringParser<Card>(orig_s, fcs_js__card_from_string);

    const match = p.consume_match("^((?:: +|:(?:$|(?=\\n)))?)");

    if (force_leading_colon && !match[1].length) {
        return new ColumnParseResult(
            false,
            start_char_idx,
            p.getConsumed(),
            'Columns must start with a ":" in strict mode.',
            [],
        );
    }

    const ret = p.loop(card_re, () => {
        const card_str = p.match(/^(\S+)/)[1];
        const m = card_str.match("^1(" + suit_re + ")");
        if (m) {
            return new ColumnParseResult(
                false,
                start_char_idx,
                p.getConsumed(),
                calc_1H_error_string(m[1]),
                [],
            );
        }
        return new ColumnParseResult(
            false,
            start_char_idx,
            p.getConsumed(),
            "Wrong card format - should be [Rank][Suit]",
            [],
        );
    });
    if (ret) {
        return ret;
    }

    return new ColumnParseResult(
        true,
        start_char_idx,
        p.getConsumed(),
        "",
        p.cards,
    );
}

type MaybeCard = Card | null;

class Freecells {
    constructor(
        private num_freecells: number,
        private cards: MaybeCard[],
    ) {
        if (!is_int(num_freecells)) {
            throw "num_freecells is not an integer.";
        }
        if (cards.length !== num_freecells) {
            throw "cards length mismatch.";
        }
    }

    public getNum(): number {
        return this.num_freecells;
    }

    public getCard(idx: number) {
        const that = this;
        if (idx < 0) {
            throw "idx is below zero.";
        }
        if (idx >= that.getNum()) {
            throw "idx exceeds the length of the column.";
        }
        return that.cards[idx];
    }

    public getArrOfStrs(): string[] {
        const that = this;
        return perl_range(0, that.getNum() - 1).map((i) => {
            const card = that.getCard(i);
            return card !== null ? card.toString() : "-";
        });
    }

    public toString(): string {
        const that = this;
        return (
            Array.prototype.concat
                .apply([], [["Freecells:"], that.getArrOfStrs()])
                .join(" ") + "\n"
        );
    }
}

class FreecellsParseResult extends BaseResult {
    public freecells: Freecells;

    constructor(
        is_correct: boolean,
        start_char_idx: number,
        num_consumed_chars: number,
        error: string,
        num_freecells: number,
        fc: MaybeCard[],
    ) {
        super(is_correct, start_char_idx, num_consumed_chars, error);
        if (is_correct) {
            this.freecells = new Freecells(num_freecells, fc);
        }
    }
}

export function fcs_js__freecells_from_string(
    num_freecells: number,
    start_char_idx: number,
    orig_s: string,
): FreecellsParseResult {
    const p = new CardsStringParser<MaybeCard>(orig_s, (card_str) => {
        return card_str === "-" ? null : fcs_js__card_from_string(card_str);
    });

    function make_ret(verdict: boolean, err_str: string) {
        return new FreecellsParseResult(
            verdict,
            start_char_idx,
            p.getConsumed(),
            err_str,
            num_freecells,
            verdict ? p.cards : [],
        );
    }

    if (
        !p.consume_match(
            new RegExp("^(" + freecells_prefix_re + ":(?: +|$|(?=\\n)))"),
        )
    ) {
        return make_ret(
            false,
            'Wrong line prefix for freecells - should be "Freecells:"',
        );
    }

    const ret = p.loop("\\-|(?:" + card_re + ")", () => {
        const card_str = p.match(/^(\S+)/)[1];
        const m = card_str.match("^1(" + suit_re + ")");
        if (m) {
            return make_ret(false, calc_1H_error_string(m[1]));
        }
        return make_ret(false, "Wrong card format - should be [Rank][Suit]");
    });

    if (ret) {
        return ret;
    }

    while (p.cards.length < num_freecells) {
        p.cards.push(null);
    }

    if (p.cards.length !== num_freecells) {
        return make_ret(false, "Too many cards specified in Freecells line.");
    }

    return make_ret(true, "");
}

export class Foundations {
    private ranks: number[] = [-1, -1, -1, -1];

    constructor() {}

    public getByIdx(deck: number, suit: number) {
        this._validateDeckSuit(deck, suit);
        return this.ranks[suit];
    }

    public setByIdx(deck: number, suit: number, rank: number): boolean {
        this._validateDeckSuit(deck, suit);

        if (!is_int(rank)) {
            throw "Rank must be an integer.";
        }

        if (!(rank >= 0 && rank <= MAX_RANK)) {
            throw "rank is out of range.";
        }

        if (this.ranks[suit] >= 0) {
            return false;
        }

        this.ranks[suit] = rank;

        return true;
    }

    public finalize(): void {
        const that = this;
        for (let i = 0; i < NUM_SUITS; i++) {
            if (that.getByIdx(0, i) < 0) {
                that.setByIdx(0, i, 0);
            }
        }
        return;
    }

    public toString(): string {
        const that = this;
        const arr: string[] = [];
        for (const suit of _suits) {
            const val = that.getByIdx(0, suit);
            if (val > 0) {
                arr.push(
                    suits__int_to_str[suit] + "-" + _ranks__int_to_str[val],
                );
            }
        }
        return (
            Array.prototype.concat
                .apply([], [["Foundations:"], arr])
                .join(" ") + "\n"
        );
    }

    private _validateDeckSuit(deck: number, suit: number) {
        if (deck !== 0) {
            throw "multiple decks are not supported.";
        }
        if (!is_int(suit)) {
            throw "suit is not an integer.";
        }
        if (!(suit >= 0 && suit < NUM_SUITS)) {
            throw "suit is out of range.";
        }

        return;
    }
}

class FoundationsParseResult extends BaseResult {
    public foundations: Foundations;

    constructor(
        is_correct: boolean,
        start_char_idx: number,
        num_consumed_chars: number,
        error: string,
        foundations: Foundations,
    ) {
        super(is_correct, start_char_idx, num_consumed_chars, error);
        if (is_correct) {
            this.foundations = foundations;
        }
    }
}

const foundations_prefix_re = /^((?:Foundations|Founds|FOUNDS|founds)\:)/;
const freecells_prefix_re = "(?:Freecells|FC|Fc|fc|freecells)";
export function fcs_js__foundations_from_string(
    num_decks: number,
    start_char_idx: number,
    orig_s: string,
): FoundationsParseResult {
    if (num_decks !== 1) {
        throw "Can only handle 1 decks.";
    }

    const p = new StringParser(orig_s);
    const founds = new Foundations();

    function make_ret(verdict: boolean, err_str: string) {
        if (verdict) {
            founds.finalize();
        }
        return new FoundationsParseResult(
            verdict,
            start_char_idx,
            p.getConsumed(),
            err_str,
            founds,
        );
    }

    if (!p.consume_match(foundations_prefix_re)) {
        return make_ret(
            false,
            'Wrong line prefix for the foundations - should be "Foundations:"',
        );
    }

    while (p.isNotEmpty()) {
        if (p.consume_match(/^( *\n?)$/)) {
            break;
        }
        const m = p.consume_match("^( +(" + suit_re + ")-(" + rank_re + "))");
        if (!m) {
            return make_ret(
                false,
                "Could not match a foundation string " +
                    suit_re +
                    "-" +
                    rank_re,
            );
        }
        const suit = m[2];
        if (
            !founds.setByIdx(
                0,
                suits__str_to_int.get(suit),
                ranks__str_to_int[m[3]],
            )
        ) {
            return make_ret(false, 'Suit "' + suit + '" was already set.');
        }
    }
    return make_ret(true, "");
}

export enum ErrorLocationType {
    Foundations,
    Freecells,
    Column,
}

class ErrorLocation {
    constructor(
        public type_: ErrorLocationType,
        public idx: number,
        public start: number,
        public end: number,
    ) {}
}

export enum ParseErrorType {
    VALID,
    TOO_MUCH_OF_CARD,
    NOT_ENOUGH_OF_CARD,
    FOUNDATIONS_NOT_AT_START,
    FREECELLS_NOT_AT_START,
    LINE_PARSE_ERROR,
    LOWERCASE_LETTERS,
    HAS_10_STRINGS,
}

class ParseError {
    public problem_strings: string[] = [];
    constructor(
        public type_: ParseErrorType,
        public locs: ErrorLocation[],
        public card: Card,
    ) {}
}

class ParseLocation {
    constructor(
        public type_: ErrorLocationType,
        public row: number,
        public col: number,
    ) {}
}

export class BoardParseResult {
    public errors: ParseError[] = [];
    public is_valid: boolean = true;
    public foundations: FoundationsParseResult;
    public freecells: FreecellsParseResult;
    public columns: ColumnParseResult[];
    constructor(
        public num_stacks: number,
        public num_freecells: number,
        orig_s: string,
    ) {
        const that = this;

        const lines = orig_s.match(/[^\n]*\n?/g).map((l) => {
            return new BoardTextLine(l);
        });
        for (const l of lines) {
            {
                const matches = l.getContent().match(/[a-z]+/g);
                if (matches && matches.length > 0) {
                    const err = new ParseError(
                        ParseErrorType.LOWERCASE_LETTERS,
                        [],
                        fcs_js__card_from_string("AH"),
                    );
                    err.problem_strings = matches;
                    that.errors.push(err);
                    that.is_valid = false;
                }
            }
            {
                const matches = l.getContent().match(/10/g);
                if (matches && matches.length > 0) {
                    const err = new ParseError(
                        ParseErrorType.HAS_10_STRINGS,
                        [],
                        fcs_js__card_from_string("AH"),
                    );
                    err.problem_strings = matches;
                    that.errors.push(err);
                    that.is_valid = false;
                }
            }
        }
        that.columns = [];
        const counter: ParseLocation[][][] = _suits.map((i) => {
            return perl_range(0, MAX_RANK).map((i) => {
                return [];
            });
        });

        const p = new StringParser(orig_s);
        for (let i = 0; i < 2; ++i) {
            p.skipComments();
            that._try_to_parse_foundations(p);
            if (!that.is_valid) {
                return;
            }
            p.skipComments();
            that._try_to_parse_freecells(p);
            if (!that.is_valid) {
                return;
            }
        }
        for (let i = 0; i < num_stacks; ++i) {
            p.skipComments();
            const start_char_idx = p.getConsumed();
            const l = p.consume_match(/^([^\n]*(?:\n|$))/)[1];
            const col = fcs_js__column_from_string(start_char_idx, l, false);
            that.columns.push(col);
            if (!col.is_correct) {
                that.errors.push(
                    new ParseError(
                        ParseErrorType.LINE_PARSE_ERROR,
                        [
                            new ErrorLocation(
                                ErrorLocationType.Column,
                                i,
                                start_char_idx,
                                p.getConsumed(),
                            ),
                        ],
                        fcs_js__card_from_string("AH"),
                    ),
                );
                that.is_valid = false;
                return;
            }
        }
        if (that.foundations) {
            for (const suit of _suits) {
                for (const rank of perl_range(
                    1,
                    that.foundations.foundations.getByIdx(0, suit),
                )) {
                    counter[suit][rank].push(
                        new ParseLocation(ErrorLocationType.Foundations, 0, 0),
                    );
                }
            }
        }
        if (that.freecells) {
            for (const i of perl_range(
                0,
                that.freecells.freecells.getNum() - 1,
            )) {
                const card = that.freecells.freecells.getCard(i);
                if (card) {
                    counter[card.getSuit()][card.getRank()].push(
                        new ParseLocation(ErrorLocationType.Freecells, i, 0),
                    );
                }
            }
        }
        that.columns.forEach((col_res, idx) => {
            const col = col_res.col;
            for (const h of perl_range(0, col.getLen() - 1)) {
                const card = col.getCard(h);

                counter[card.getSuit()][card.getRank()].push(
                    new ParseLocation(ErrorLocationType.Column, idx, h),
                );
            }
        });
        const NUM_WANTED_CARDS: number = 1;
        const too_many_cards__errors: ParseError[] = [];
        const not_enough_cards__errors: ParseError[] = [];
        for (const suit of _suits) {
            for (const rank of _ranks) {
                const count = counter[suit][rank];
                function add_error(arr, type_, locs) {
                    arr.push(new ParseError(type_, locs, new Card(rank, suit)));
                    that.is_valid = false;

                    return;
                }
                if (count.length > NUM_WANTED_CARDS) {
                    const locs: ErrorLocation[] = count.map((v) => {
                        return new ErrorLocation(v.type_, v.row, 0, 0);
                    });

                    add_error(
                        too_many_cards__errors,
                        ParseErrorType.TOO_MUCH_OF_CARD,
                        locs,
                    );
                } else if (count.length < NUM_WANTED_CARDS) {
                    add_error(
                        not_enough_cards__errors,
                        ParseErrorType.NOT_ENOUGH_OF_CARD,
                        [],
                    );
                }
            }
        }
        that.errors.push(...too_many_cards__errors);
        that.errors.push(...not_enough_cards__errors);

        return;
    }
    public getBoardString(): string {
        const that = this;
        let ret: string = "";
        if (that.foundations) {
            ret += that.foundations.foundations.toString();
        }
        if (that.freecells) {
            ret += that.freecells.freecells.toString();
        }
        for (const col of that.columns) {
            ret += col.toString();
        }
        return ret;
    }
    private _calc_filled(): ColumnParseResult[] {
        const that = this;
        return that.columns.filter((c) => {
            return c.getLen() > 0;
        });
    }
    private _try_to_parse_foundations(p: StringParser): void {
        const that = this;
        if (p.match(foundations_prefix_re)) {
            const start_char_idx = p.getConsumed();
            const l = p.consume_match(/^([^\n]*(?:\n|$))/)[1];
            const fo = fcs_js__foundations_from_string(1, start_char_idx, l);
            that.foundations = fo;
            if (!fo.is_correct) {
                that.errors.push(
                    new ParseError(
                        ParseErrorType.LINE_PARSE_ERROR,
                        [
                            new ErrorLocation(
                                ErrorLocationType.Foundations,
                                0,
                                start_char_idx,
                                p.getConsumed(),
                            ),
                        ],
                        fcs_js__card_from_string("AH"),
                    ),
                );
                that.is_valid = false;
                return;
            }
        }
    }
    private _try_to_parse_freecells(p: StringParser): void {
        const that = this;
        const num_freecells = that.num_freecells;
        if (p.match(new RegExp("^" + freecells_prefix_re + ":"))) {
            const start_char_idx = p.getConsumed();
            const l = p.consume_match(/^([^\n]*(?:\n|$))/)[1];
            const fc = fcs_js__freecells_from_string(
                num_freecells,
                start_char_idx,
                l,
            );
            that.freecells = fc;
            if (!fc.is_correct) {
                that.errors.push(
                    new ParseError(
                        ParseErrorType.LINE_PARSE_ERROR,
                        [
                            new ErrorLocation(
                                ErrorLocationType.Freecells,
                                0,
                                start_char_idx,
                                p.getConsumed(),
                            ),
                        ],
                        fcs_js__card_from_string("AH"),
                    ),
                );
                that.is_valid = false;
                return;
            }
        }
    }
    public checkIfFlipped(): boolean {
        const that = this;
        let i = 0;
        const my_filled_columns = that._calc_filled();
        for (; i < 6; ++i) {
            if (i >= my_filled_columns.length) {
                return false;
            }
            if (my_filled_columns[i].getLen() != 8) {
                return false;
            }
        }
        for (; i < 7; ++i) {
            if (i >= my_filled_columns.length) {
                return false;
            }
            if (my_filled_columns[i].getLen() != 4) {
                return false;
            }
        }
        for (; i < my_filled_columns.length; ++i) {
            if (my_filled_columns[i].getLen() != 0) {
                return false;
            }
        }
        return true;
    }
    public flip(): BoardParseResult {
        const that = this;
        if (!that.checkIfFlipped()) {
            throw "not flipped";
        }
        const my_filled_columns = that._calc_filled();
        let new_columns: ColumnParseResult[] = [];
        for (let i = 0; i < 8; ++i) {
            new_columns.push(
                fcs_js__column_from_string(
                    0,
                    ": " +
                        perl_range(0, i < 4 ? 6 : 5)
                            .map((c) => {
                                return my_filled_columns[c].col
                                    .getCard(i)
                                    .toString();
                            })
                            .join(" ") +
                        "\n",
                    false,
                ),
            );
        }
        return new BoardParseResult(
            8,
            4,
            new_columns.map((col) => col.toString()).join(""),
        );
    }
}
const lax_card_rank_re = "(?:(?:" + rank_re + ")|10|[01])";
const lax_card_re =
    "(?:(?:" +
    lax_card_rank_re +
    suit_re +
    ")|(?:" +
    suit_re +
    lax_card_rank_re +
    "))";
const lax_card_with_spaces_re = "(?:(?:\\s|^)" + lax_card_re + "(?=(?:\\s|$)))";
const lax_card_three_matches = new RegExp(
    lax_card_with_spaces_re +
        ".*?" +
        lax_card_with_spaces_re +
        ".*?" +
        lax_card_with_spaces_re,
    "ims",
);
export function determine_if_string_is_board_like(s: string): boolean {
    return lax_card_three_matches.test(s);
}



// Adapted from http://www.inventpartners.com/javascript_is_int - thanks.
function is_int(input: number): boolean {
    var value:string = "" + input;
    if ((parseFloat(value) == parseInt(value)) && !isNaN(input)) {
        return true;
    } else {
        return false;
    }
}

var _ranks__int_to_str:string = "0A23456789TJQK";
var _ranks__str_to_int = {};
function _perl_range(start: number, end: number): Array<number> {
    var ret: Array<number> = [];

    for (var i = start; i <= end; i++) {
        ret.push(i);
    }

    return ret;
}

_perl_range(1,13).forEach(function (rank) {
    _ranks__str_to_int[_ranks__int_to_str.substring(rank, rank+1)] = rank;
});
var _suits__int_to_str:string = "HCDS";
var _suits__str_to_int = {};
_perl_range(0,3).forEach(function (suit) {
    _suits__str_to_int[_suits__int_to_str.substring(suit, suit+1)] = suit;
});

class Card {
    private rank: number;
    private suit: number;

    constructor(rank: number, suit: number) {
        if (! is_int(rank)) {
            throw "rank is not an integer.";
        }
        if (! is_int(suit)) {
            throw "suit is not an integer.";
        }
        if (rank < 1) {
            throw "rank is too low.";
        }
        if (rank > 13) {
            throw "rank is too high.";
        }
        if (suit < 0) {
            throw "suit is negative.";
        }
        if (suit > 3) {
            throw "suit is too high.";
        }
        this.rank = rank;
        this.suit = suit;
    }

    getRank(): number {
        return this.rank;
    }

    getSuit(): number {
        return this.suit;
    }

    toString(): string {
        return _ranks__int_to_str.substring(this.rank, this.rank+1) + _suits__int_to_str.substring(this.suit, this.suit+1);
    }
}

class Column {
    private cards: Array<Card>;

    constructor(cards: Array<Card>) {
        this.cards = cards;
    }

    getLen(): number {
        return this.cards.length;
    }

    getCard(idx: number) {
        var that = this;
        if (idx < 0) {
            throw "idx is below zero.";
        }
        if (idx >= that.getLen()) {
            throw "idx exceeds the length of the column.";
        }
        return that.cards[idx];
    }

    getArrOfStrs(): Array<string> {
        var that = this;
        return _perl_range(0, that.getLen()-1).map(function (i) {
            return that.getCard(i).toString();
        });
    }
}


const suit_re: string = '[HCDS]';
const rank_re: string = '[A23456789TJQK]';
const card_re: string = '(' + rank_re + ')(' + suit_re + ')';
export function fcs_js__card_from_string(s: string): Card {
    var m = s.match('^' + card_re + '$');
    if (! m) {
        throw "Invalid format for a card - \"" + s + "\"";
    }
    return new Card(_ranks__str_to_int[m[1]], _suits__str_to_int[m[2]]);
}

class BaseResult {
    public is_correct: boolean;
    public start_char_idx: number;
    public num_consumed_chars: number;
    public error: string;

    constructor(is_correct: boolean, start_char_idx: number, num_consumed_chars: number, error: string) {
        this.is_correct = is_correct;
        this.num_consumed_chars = num_consumed_chars;
        this.error = error;
        this.start_char_idx = start_char_idx;
    }
    getEnd(): number {
        return (this.start_char_idx + this.num_consumed_chars);
    }
}

class ColumnParseResult extends BaseResult {
    public col: Column;

    constructor(is_correct: boolean, start_char_idx: number, num_consumed_chars: number, error: string, cards: Array<Card>) {
        super(is_correct, start_char_idx, num_consumed_chars, error);
        this.col = new Column(cards);
    }
}

class StringParser {
    private s: string;
    private consumed: number;

    constructor(s: string) {
        this.s = s;
        this.consumed = 0;
    }

    consume(m: RegExpMatchArray): void {
        var that = this;
        var len_match:number = m[1].length;
        that.consumed += len_match;
        that.s = that.s.substring(len_match);

        return;
    }

    getConsumed(): number {
        return this.consumed;
    }

    isNotEmpty(): boolean {
        return (this.s.length > 0);
    }

    match(re: any): RegExpMatchArray {
        return this.s.match(re);
    }

    consume_match(re: any): RegExpMatchArray {
        var that = this;
        var m = that.match(re);
        if (m) {
            that.consume(m);
        }
        return m;
    }
}

class CardsStringParser<CardType> extends StringParser {
    private is_start: boolean;
    public cards: Array<CardType>;
    private card_mapper: ((string) => CardType);

    constructor(s: string, card_mapper) {
        super(s)
        this.is_start = true;
        this.cards = [];
        this.card_mapper = card_mapper;
    }

    afterStart(): void {
        this.is_start = false;

        return;
    }

    getStartSpace(): string {
        return (this.is_start ? '' : ' +');
    }

    should_loop(): boolean {
        var that = this;
        return (that.isNotEmpty() && (!that.consume_match(/^(\s*(?:#[^\n]*)?\n?)$/)));
    }

    add(m: RegExpMatchArray): void {
        this.cards.push(this.card_mapper(m[2]));

        this.afterStart();
        return;
    }

    loop(re: any, callback: (() => any)): any {
        var p = this;

        while (p.should_loop()) {
            var m = p.consume_match('^(' + p.getStartSpace() + '(' + re + ')' + ')');
            if (! m) {
                p.consume_match('^( *)');

                return callback();
            }
            p.add(m);
        }
        return null;
    }
}

export function fcs_js__column_from_string(start_char_idx: number, orig_s: string): ColumnParseResult {
    var p = new CardsStringParser<Card>(orig_s, fcs_js__card_from_string);

    p.consume_match('^((?:\: +)?)');

    var ret = p.loop(card_re, () => { return new ColumnParseResult(false, start_char_idx, p.getConsumed(), 'Wrong card format - should be [Rank][Suit]', []); });
    if (ret) {
        return ret;
    }

    return new ColumnParseResult(true, start_char_idx, p.getConsumed(), '', p.cards);
}

type MaybeCard = Card | null;

class Freecells {
    private num_freecells: number;
    private cards: Array<MaybeCard>;

    constructor(num_freecells: number, cards: Array<MaybeCard>) {
        if (!is_int(num_freecells)) {
            throw "num_freecells is not an integer.";
        }
        this.num_freecells = num_freecells;

        if (cards.length != num_freecells) {
            throw "cards length mismatch.";
        }
        this.cards = cards;
    }

    getNum(): number {
        return this.num_freecells;
    }

    getCard(idx: number) {
        var that = this;
        if (idx < 0) {
            throw "idx is below zero.";
        }
        if (idx >= that.getNum()) {
            throw "idx exceeds the length of the column.";
        }
        return that.cards[idx];
    }

    getArrOfStrs(): Array<string> {
        var that = this;
        return _perl_range(0, that.getNum()-1).map(function (i) {
            var card = that.getCard(i);
            return ((card !== null) ? card.toString() : '-');
        });
    }
}

// TODO : Merge common functionality with ColumnParseResult into a base class.
class FreecellsParseResult extends BaseResult {
    public freecells: Freecells;

    constructor(is_correct: boolean, start_char_idx: number, num_consumed_chars: number, error: string, num_freecells: number, fc: Array<MaybeCard>) {
        super(is_correct, start_char_idx, num_consumed_chars, error);
        if (is_correct) {
            this.freecells = new Freecells(num_freecells, fc);
        }
    }
}

export function fcs_js__freecells_from_string(num_freecells: number, start_char_idx: number, orig_s: string): FreecellsParseResult {
    var p = new CardsStringParser<MaybeCard>(orig_s, (card_str) => { return ((card_str == '-') ? null : fcs_js__card_from_string(card_str)); });

    function make_ret (verdict: boolean, err_str: string) {
        return new FreecellsParseResult(verdict, start_char_idx, p.getConsumed(), err_str, num_freecells, verdict ? p.cards : []);
    }

    if (!p.consume_match(/^(Freecells\: +)/)) {
        return make_ret(false, 'Wrong line prefix for freecells - should be "Freecells:"');
    }

    var ret = p.loop("\\-|(?:" + card_re + ')', () => { return make_ret(false, 'Wrong card format - should be [Rank][Suit]'); });

    if (ret) {
        return ret;
    }

    while (p.cards.length < num_freecells) {
        p.cards.push(null);
    }

    if (p.cards.length != num_freecells) {
        return make_ret(false, 'Too many cards specified in Freecells line.');
    }

    return make_ret(true, '');
}

export class Foundations {
    private ranks: Array<number>;

    constructor() {
        this.ranks = [-1,-1,-1,-1];
    }

    private _validateDeckSuit(deck: number, suit: number) {
        if (deck != 0) {
            throw "multiple decks are not supported.";
        }
        if (!is_int(suit)) {
            throw "suit is not an integer.";
        }
        if (! ((suit >= 0) && (suit < 4))) {
            throw "suit is out of range.";
        }

        return;
    }


    getByIdx(deck: number, suit: number) {
        this._validateDeckSuit(deck, suit);
        return this.ranks[suit];
    }

    setByIdx(deck: number, suit: number, rank: number): boolean {
        this._validateDeckSuit(deck, suit);

        if (!is_int(rank)) {
            throw "Rank must be an integer.";
        }

        if (! ((rank >=0 ) && (rank <= 13))) {
            throw "rank is out of range.";
        }

        if (this.ranks[suit] >= 0) {
            return false;
        }

        this.ranks[suit] = rank;

        return true;
    }

    finalize(): void {
        var that = this;
        for (var i = 0; i < 4; i++) {
            if (that.getByIdx(0,i) < 0) {
                that.setByIdx(0,i,0);
            }
        }
        return;
    }
};

class FoundationsParseResult extends BaseResult {
    public foundations: Foundations;

    constructor(is_correct: boolean, start_char_idx: number, num_consumed_chars: number, error: string, foundations: Foundations) {
        super(is_correct, start_char_idx, num_consumed_chars, error);
        if (is_correct) {
            this.foundations = foundations;
        }
    }
}

export function fcs_js__foundations_from_string(num_decks: number, start_char_idx: number, orig_s: string): FoundationsParseResult {

    if (num_decks != 1) {
        throw "Can only handle 1 decks.";
    }

    var p = new StringParser(orig_s);
    var founds = new Foundations;

    function make_ret (verdict: boolean, err_str: string) {
        if (verdict) {
            founds.finalize();
        }
        return new FoundationsParseResult(verdict, start_char_idx, p.getConsumed(), err_str, founds);
    }

    if (!p.consume_match(/^(Foundations\:)/)) {
        return make_ret(false, 'Wrong line prefix for freecells - should be "Freecells:"');
    }

    while (p.isNotEmpty()) {
        if (p.consume_match(/^( *\n?)$/)) {
            break;
        }
        var m = p.consume_match("^( +(" + suit_re + ")-(" + rank_re + "))");
        if (! m) {
            return make_ret(false, "Could not match a foundation string [HCDS]-[A23456789TJQK]");
        }
        const suit = m[2];
        if (!founds.setByIdx(0, _suits__str_to_int[suit], _ranks__str_to_int[m[3]])) {
            return make_ret(false, "Suit \"" + suit + "\" was already set.");
        }
    }
    return make_ret(true, '');
}

export enum ErrorLocationType {
    ErrorLocationType_Foundations,
    ErrorLocationType_Freecells,
    ErrorLocationType_Column,
};

class ErrorLocation {
    public type_: ErrorLocationType;
    public idx: number;
    public start: number;
    public end: number;

    constructor(t: ErrorLocationType, idx: number, start: number, end:number) {
        this.type_ = t;
        this.idx = idx;
        this.start = start;
        this.end = end;
        return;
    }
}

export enum ParseErrorType {
    VALID,
    TOO_MUCH_OF_CARD,
    NOT_ENOUGH_OF_CARD,
    FOUNDATIONS_NOT_AT_START,
    FREECELLS_NOT_AT_START,
    LINE_PARSE_ERROR,
};

class ParseError {
    public type_: ParseErrorType;
    public locs: Array<ErrorLocation>;
    public card: Card;

    constructor(t: ParseErrorType, locs: Array<ErrorLocation>, c: Card) {
        this.type_ = t;
        this.locs = locs;
        this.card = c;
        return;
    }
}

export class BoardParseResult {
    public errors: Array<ParseError>;
    public is_valid: boolean;
    public foundations: FoundationsParseResult;
    public freecells: FreecellsParseResult;
    public columns: Array<ColumnParseResult>;
    public num_stacks: number;
    public num_freecells: number;
    constructor(num_stacks: number, num_freecells: number, orig_s: string) {
        var that = this;
        that.num_stacks = num_stacks;
        that.num_freecells = num_freecells;

        that.errors = [];
        that.columns = [];
        var p = new StringParser(orig_s);
        for (var i=0; i < num_stacks; i++) {
            const start_char_idx = p.getConsumed();
            var l = p.consume_match(/^([^\n]*(?:\n|$))/)[1];
            var col = fcs_js__column_from_string(start_char_idx, l);
            if (! col.is_correct) {
                that.errors.push(new ParseError(
                    ParseErrorType.LINE_PARSE_ERROR,
                    [new ErrorLocation(
                        ErrorLocationType.ErrorLocationType_Column,
                        i,
                        start_char_idx,
                        p.getConsumed()
                    )
                    ],
                    fcs_js__card_from_string('AH')
                    )
                );
                that.is_valid = false;
                return;
            }
            that.columns.push(col);
        }
        // TODO : Implement duplicate/missing cards.
        that.is_valid = true;
        return;
    }
}



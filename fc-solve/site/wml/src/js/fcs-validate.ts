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
}



var card_re:string = '([A23456789TJQK])([HCDS])';
function fcs_js__card_from_string(s: string): Card {
    var m = s.match('^' + card_re + '$');
    if (! m) {
        throw "Invalid format for a card - \"" + s + "\"";
    }
    return new Card(_ranks__str_to_int[m[1]], _suits__str_to_int[m[2]]);
}

class ColumnParseResult {
    public col: Column;
    public is_correct: boolean;
    public problem_found_at_char_idx: number;
    public error: string;

    constructor(is_correct: boolean, problem_found_at_char_idx: number, error: string, cards: Array<Card>) {
        this.is_correct = is_correct;
        this.problem_found_at_char_idx = problem_found_at_char_idx;
        this.error = error;
        this.col = new Column(cards);
    }
}

function fcs_js__column_from_string(s: string): ColumnParseResult {
    var cards:Array<Card> = [];
    var is_start:boolean = true;
    var consumed:number = 0;

    function consume_match(m: RegExpMatchArray): void {
        var len_match:number = m[1].length;
        consumed += len_match;
        s = s.substring(len_match);

        return;
    }

    consume_match(s.match('^((?:\: +)?)'));
    while (s.length > 0) {
        if (s.match('^ +$')) {
            break;
        }

        var m = s.match('^(' + (is_start ? '' : ' +') + '(' + card_re + ')' + ')');
        if (! m) {
            m = s.match('^( *)');
            consume_match(m);

            return new ColumnParseResult(false, consumed, 'Wrong card format - should be [Rank][Suit]', []);
        }

        consume_match(m);
        cards.push(fcs_js__card_from_string(m[2]));
        is_start = false;
    }
    return new ColumnParseResult(true, consumed, '', cards);
}

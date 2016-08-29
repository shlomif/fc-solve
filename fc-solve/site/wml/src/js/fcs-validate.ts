// Adapted from http://www.inventpartners.com/javascript_is_int - thanks.
function is_int(input: number): boolean {
    var value:string = "" + input;
    if((parseFloat(value) == parseInt(value)) && !isNaN(input)){
        return true;
    } else {
        return false;
    }
}

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


function fcs_js__card_from_string(s: string): Card {
    var m = s.match(/^([A23456789TJQK])([HCDS])$/);
    if (! m) {
        throw "Invalid format for a card - \"" + s + "\"";
    }
    return new Card(_ranks__str_to_int[m[1]], _suits__str_to_int[m[2]]);
}

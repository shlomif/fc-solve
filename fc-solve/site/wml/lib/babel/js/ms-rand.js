"use strict";
/*
 * Microsoft C Run-time-Library-compatible Random Number Generator
 * Copyright by Shlomi Fish, 2011.
 * Released under the MIT/Expat License
 * ( http://en.wikipedia.org/wiki/MIT_License ).
 * */
class MSRand {
    constructor(args) {
        var that = this;
        that.gamenumber = args.gamenumber;
        that.rander = fc_solve__hll_ms_rand__get_singleton();
        fc_solve__hll_ms_rand__init(that.rander, "" + that.gamenumber);
        return;
    }
    max_rand(mymax) {
        return fc_solve__hll_ms_rand__mod_rand(this.rander, mymax);
    }
    shuffle(deck) {
        if (deck.length) {
            var i = deck.length;
            while (--i) {
                var j = this.max_rand(i+1);
                var tmp = deck[i];
                deck[i] = deck[j];
                deck[j] = tmp;
            }
        }
        return deck;
    }
}

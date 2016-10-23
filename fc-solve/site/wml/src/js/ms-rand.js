"use strict";
/*
 * Microsoft C Run-time-Library-compatible Random Number Generator
 * Copyright by Shlomi Fish, 2011.
 * Released under the MIT/X11 License
 * ( http://en.wikipedia.org/wiki/MIT_License ).
 * */
Class('MSRand', {
    has: {
        gamenumber: { is: ro, },
        rander: {is: rw, init: function() {
            var that = this;
            var ret = fc_solve__hll_ms_rand__get_singleton();
            fc_solve__hll_ms_rand__init(ret, "" + that.getGamenumber());
            return ret;
        }},
    },
    methods: {
        max_rand: function(mymax) {
            return fc_solve__hll_ms_rand__mod_rand(this.getRander(), mymax);
        },
        shuffle: function(deck) {
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
        },
    },
});

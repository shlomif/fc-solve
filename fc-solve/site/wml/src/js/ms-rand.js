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
        seed: { is: rw, init: function() { return this.gamenumber; } },
    },
    methods: {
        rand: function() {
            this.setSeed((this.getSeed() * 214013 + 2531011) & 0x7FFFFFFF);
            return ((this.getSeed() >> 16) & 0x7fff);
        },
        randp: function() {
            this.setSeed((this.getSeed() * 214013 + 2531011) & 0x7FFFFFFF);
            return ((this.getSeed() >> 16) & 0xffff);
        },
        game_num_rand: function () {
            var that = this;
            var gnGameNumber = that.getGamenumber();
            if (gnGameNumber < 0x100000000)
            {
                const ret = that.rand();
                return ((gnGameNumber < 0x80000000) ? ret : (ret | 0x8000));
            }
            else
            {
                return that.randp() + 1;
            }
        },
        max_rand: function(mymax) {
            return this.game_num_rand() % mymax;
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

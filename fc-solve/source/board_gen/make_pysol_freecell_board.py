#!/usr/bin/python
#
# make_pysol_freecell_board.py - Program to generate the boards of
# PySol for input into Freecell Solver.
# 
# Usage: make_pysol_freecell_board.py [board number] | fc-solve
# 
# Or on non-UNIXes:
#
# python make_pysol_freecell_board.py [board number] | fc-solve
#
# This program is platform independant and will generate the same results
# on all architectures and operating systems.
#
# Based on the code by Markus Franz Xaver Johannes Oberhumer.
# Modified by Shlomi Fish, 2000
#
# Since much of the code here is ripped from the actual PySol code, this 
# program is distributed under the GNU General Public License.
#
#
#
## vim:ts=4:et:nowrap
##
##---------------------------------------------------------------------------##
##
## PySol -- a Python Solitaire game
##
## Copyright (C) 2000 Markus Franz Xaver Johannes Oberhumer
## Copyright (C) 1999 Markus Franz Xaver Johannes Oberhumer
## Copyright (C) 1998 Markus Franz Xaver Johannes Oberhumer
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; see the file COPYING.
## If not, write to the Free Software Foundation, Inc.,
## 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
##
## Markus F.X.J. Oberhumer
## <markus.oberhumer@jk.uni-linz.ac.at>
## http://wildsau.idv.uni-linz.ac.at/mfx/pysol.html
##
##---------------------------------------------------------------------------##


# imports
import sys, os, re, string, time, types

# PySol imports

# /***********************************************************************
# // Abstract PySol Random number generator.
# //
# // We use a seed of type long in the range [0, MAX_SEED].
# ************************************************************************/

class PysolRandom:
    MAX_SEED = 0L

    ORIGIN_UNKNOWN  = 0
    ORIGIN_RANDOM   = 1
    ORIGIN_PREVIEW  = 2         # random from preview
    ORIGIN_SELECTED = 3         # manually entered
    ORIGIN_NEXT_GAME = 4        # "Next game number"

    def __init__(self, seed=None):
        if seed is None:
            seed = self._getRandomSeed()
        self.initial_seed = self.setSeed(seed)
        self.origin = self.ORIGIN_UNKNOWN

    def __str__(self):
        return self.str(self.initial_seed)

    def reset(self):
        self.seed = self.initial_seed

    def getSeed(self):
        return self.seed

    def setSeed(self, seed):
        seed = self._convertSeed(seed)
        if type(seed) is not types.LongType:
            raise TypeError, "seeds must be longs"
        if not (0L <= seed <= self.MAX_SEED):
            raise ValueError, "seed out of range"
        self.seed = seed
        return seed

    def copy(self):
        random = PysolRandom(0L)
        random.__class__ = self.__class__
        random.__dict__.update(self.__dict__)
        return random

    #
    # implementation
    #

    def choice(self, seq):
        return seq[int(self.random() * len(seq))]

    # Get a random integer in the range [a, b] including both end points.
    def randint(self, a, b):
        return a + int(self.random() * (b+1-a))

    #
    # subclass responsibility
    #

    # Get the next random number in the range [0.0, 1.0).
    def random(self):
        raise SubclassResponsibility

    #
    # subclass overrideable
    #

    def _convertSeed(self, seed):
        return long(seed)

    def increaseSeed(self, seed):
        if seed < self.MAX_SEED:
            return seed + 1L
        return 0L

    def _getRandomSeed(self):
        t = long(time.time() * 256.0)
        t = (t ^ (t >> 24)) % (self.MAX_SEED + 1L)
        return t

    #
    # shuffle
    #   see: Knuth, Vol. 2, Chapter 3.4.2, Algorithm P
    #   see: FAQ of sci.crypt: "How do I shuffle cards ?"
    #

    def shuffle(self, seq):
        n = len(seq) - 1
        while n > 0:
            j = self.randint(0, n)
            seq[n], seq[j] = seq[j], seq[n]
            n = n - 1


# /***********************************************************************
# // Linear Congruential random generator
# //
# // Knuth, Donald.E., "The Art of Computer Programming,", Vol 2,
# // Seminumerical Algorithms, Third Edition, Addison-Wesley, 1998,
# // p. 106 (line 26) & p. 108
# ************************************************************************/

class LCRandom64(PysolRandom):
    MAX_SEED = 0xffffffffffffffffL  # 64 bits

    def str(self, seed):
        s = repr(long(seed))[:-1]
        s = "0"*(20-len(s)) + s
        return s

    def random(self):
        self.seed = (self.seed*6364136223846793005L + 1L) & self.MAX_SEED
        return ((self.seed >> 21) & 0x7fffffffL) / 2147483648.0
        


# /***********************************************************************
# // Linear Congruential random generator
# // In PySol this is only used for 0 <= seed <= 32000.
# ************************************************************************/

class LCRandom31(PysolRandom):
    MAX_SEED = 0x7fffffffL          # 31 bits

    def str(self, seed):
        return "%05d" % int(seed)

    def random(self):
        self.seed = (self.seed*214013L + 2531011L) & self.MAX_SEED
        return (self.seed >> 16) / 32768.0

    def randint(self, a, b):
        self.seed = (self.seed*214013L + 2531011L) & self.MAX_SEED
        return a + (int(self.seed >> 16) % (b+1-a))

def createCards(num_decks=1):
    cards = []
    id = 0
    for deck in range(num_decks):
        for suit in range(4):
            for rank in range(13):
                card = (suit << 4) + (rank+1)
                #if card is None:
                #    continue
                cards.append(card)
                id = id + 1
    return cards

def get_card_suit(suit):
    if suit == 0:
        return "C"
    elif suit == 1:
        return "S"
    elif suit == 2:
        return "H"
    elif suit == 3:
        return "D"

def get_card_num(rank,print_ts):
    ret = ""
    if rank == 1:
        ret = ret + "A"
    elif rank < 10:
        ret = ret + str(rank)
    elif rank == 10:
        if print_ts:
            ret = ret + "T"
        else:
            ret = ret + "10"
    elif rank == 11:
        ret = ret + "J"
    elif rank == 12:
        ret = ret + "Q"
    elif rank == 13:
        ret = ret + "K"
    return ret;

def card_to_string(card,not_append_ws,print_ts,flipped=0):
    suit = card >> 4
    rank = card & 0x0F
    ret = ""
    if flipped:
        ret = ret + "<"

    ret = ret + get_card_num(rank,print_ts)
    ret = ret + get_card_suit(suit) 

    if flipped:
        ret = ret + ">"
    if (not(not_append_ws)):
        ret = ret + " "
    
    return ret

def flip_card(card_str, flip):
    if flip:
        return "<" + card_str + ">"
    else:
        return card_str

def shlomif_main(args):
    print_ts = 0
    if (args[1] == "-t"):
        print_ts = 1
        args.pop(0)
    game_num = long(args[1])
    if (len(args) >= 3):
        which_game = args[2]
    else:
        which_game = "freecell"
    if ((which_game == "der_katz") or (which_game == "der_katzenschwantz") or (which_game == "die_schlange") or (which_game == "gypsy")):
        orig_cards = createCards(2)
    else:
        orig_cards = createCards()
    if game_num <= 32000:
        r = LCRandom31()
        r.setSeed(game_num)
        fcards = []
        if (len(orig_cards) == 52):
            for i in range(13):
                for j in (0, 39, 26, 13):
                    fcards.append(orig_cards[i + j])
            orig_cards = fcards
        r.shuffle(orig_cards)
    else:
        r = LCRandom64()
        r.setSeed(game_num)
        r.shuffle(orig_cards)

    
    cards = []
    for i in range(len(orig_cards)):
        cards.append(orig_cards[len(orig_cards)-i-1])

    if ((which_game == "der_katz") or (which_game == "der_katzenschwantz") or (which_game == "die_schlange")):
        if (which_game == "die_schlange"):
            print "Foundations: H-A S-A D-A C-A H-A S-A D-A C-A"
        output = ""
        for card in cards:
            if ((card & 0x0F) == 13):
                print output
                output = ""
            if (not((which_game == "die_schlange") and ((card & 0x0F) == 1))):
                if (output != ""):
                    output = output + " "        
                output = output + card_to_string(card, 1, print_ts)
        print output
    elif ((which_game == "freecell") or (which_game == "forecell") or (which_game == "bakers_game") or (which_game == "ko_bakers_game") or (which_game == "kings_only_bakers_game") or (which_game == "relaxed_freecell") or (which_game == "eight_off")):
        output = range(9)
        for i in range(9):
            output[i] = ""

        if ((which_game == "forecell") or (which_game == "eight_off")):
            output[8] = "FC: ";
            for i in range(52):
                if (i < 48):
                    output[i%8] = output[i%8] + card_to_string(cards[i], (i>=48-8), print_ts)
                else:
                    if (which_game == "forecell"):
                        output[8] = output[8] + card_to_string(cards[i], (i == 52), print_ts)
                    else:
                        output[8] = output[8] + card_to_string(cards[i], 0, print_ts) + "-"
                        if (i != 52):
                            output[8] = output[8] + " "
        else:
            for i in range(52):
                output[i%8] = output[i%8] + card_to_string(cards[i], (i>=52-8), print_ts)
        
        if ((which_game == "forecell") or (which_game == "eight_off")):
            print output[8]
        for i in range(8):
            print output[i]
    elif ((which_game == "seahaven_towers") or (which_game == "seahaven") or (which_game == "relaxed_seahaven") or (which_game == "relaxed_seahaven_towers")):
        output = range(11);
        for i in range(11):
            output[i] = ""
        output[10] = "FC: - "

        for i in range(52):
            if (i < 50):
                output[i%10] = output[i%10] + card_to_string(cards[i], (i>=50-10), print_ts)
            else:
                output[10] = output[10] + card_to_string(cards[i], (i == 52-1), print_ts)

        print output[10]
        for i in range(10):
            print output[i]
    elif (which_game == "bakers_dozen"):
        i, n = 0, 13 
        kings = []
        for c in cards:
            if ((c & 0x0F) == 13):
                kings.append(i)
            i = i + 1
        for i in kings:
            j = i % n
            while j < i:
                if ((cards[j] & 0x0F) != 13):
                    print "Performed Swap"
                    cards[i], cards[j] = cards[j], cards[i]
                    break
                j = j + n
        cards.reverse()

        output = range(13);
        for i in range(13):
            output[i] = ""

        for i in range(52):
            output[i%13] = output[i%13] + card_to_string(cards[i], (i>=52-13), print_ts)

        for i in range(13):
            print output[i]
    elif (which_game == "gypsy"):
        output = range(8);
        for i in range(8):
            output[i] = ""
        for i in range(24):
            output[i%8] = output[i%8] + flip_card(card_to_string(cards[i], 1, print_ts), (i<16))
            if (i < 16):
                output[i%8] = output[i%8] + " "

        talon = "Talon:"
        for i in range(24,8*13):
            talon = talon + " " + card_to_string(cards[i], 1, print_ts)
                
        print talon
        for i in range(8):
            print output[i]
    elif ((which_game == "klondike") or (which_game == "klondike_by_threes") or (which_game == "casino_klondike") or (which_game == "small_harp") or (which_game == "thumb_and_pouch") or (which_game == "vegas_klondike") or (which_game == "whitehead")):
        #o = ""
        #for i in cards:
        #    o = o + " " + card_to_string(i, 1)
        #print o
        output = range(7);
        for i in range(7):
            output[i] = ""
        card_num = 0
        for r in range(1,7):
            for s in range(7-r):
                output[s] = output[s] + card_to_string(cards[card_num], 0, print_ts, flipped=(which_game != "whitehead"))
                card_num = card_num + 1
        for s in range(7):
            output[s] = output[s] + card_to_string(cards[card_num], 1, print_ts)
            card_num = card_num + 1

        talon = "Talon: "
        while card_num < 52:
            talon = talon + card_to_string(cards[card_num], (card_num == 52-1), print_ts)
            card_num = card_num + 1
            
        
        print talon
        if (not (which_game == "small_harp")):
            output.reverse();
        for i in output:
            print i
    elif (which_game == "simple_simon"):
        card_num = 0
        output = range(10)
        for i in range(10):
            output[i] = ""
        num_cards = 9
        while num_cards >= 3:
            for s in range(num_cards):
                output[s] = output[s] + card_to_string(cards[card_num], 0, print_ts)
                card_num = card_num + 1
            num_cards = num_cards - 1
        for s in range(10):
            output[s] = output[s] + card_to_string(cards[card_num], 1, print_ts)
            card_num = card_num + 1
        for i in output:
            print i
    elif (which_game == "yukon"):
        card_num = 0
        output = range(7)
        for i in range(7):
            output[i] = ""
        for i in range(1, 7):
            for j in range(i, 7):
                output[j] = output[j] + card_to_string(cards[card_num], 0, print_ts, 1)
                card_num = card_num + 1
        for i in range(4):
            for j in range(1,7):
                output[j] = output[j] + card_to_string(cards[card_num], 0, print_ts, 0)
                card_num = card_num + 1
        for i in range(7):
            output[i] = output[i] + card_to_string(cards[card_num], 1, print_ts, 0)
            card_num = card_num + 1
        for i in output:
            print i
    elif ((which_game == "beleaguered_castle") or (which_game == "streets_and_alleys") or (which_game == "citadel")):
        if (which_game == "beleaguered_castle") or (which_game == "citadel"):
            new_cards = []
            for c in cards:
                if (c & 0x0F != 1):
                    new_cards.append(c)
            cards = new_cards;
        output = range(8)
        for i in range(8):
            output[i] = ""

        card_num = 0
        if (which_game == "beleaguered_castle") or (which_game == "citadel"):
            foundations = [1,1,1,1]
        else:
            foundations = [0,0,0,0]
        for i in range(6):
            for s in range(8):
                if (which_game == "citadel"): 
                    if (foundations[cards[card_num] >> 4]+1 == (cards[card_num] & 0x0F)):
                        foundations[cards[card_num] >> 4] = foundations[cards[card_num] >> 4] + 1;
                        card_num = card_num + 1
                        continue
                output[s] = output[s] + card_to_string(cards[card_num], (i == 6-1), print_ts)
                card_num = card_num + 1
            if (card_num == len(cards)):
                break
                
        if (which_game == "streets_and_alleys"):
            for s in range(4):
                output[s] = output[s] + " " + card_to_string(cards[card_num], 1, print_ts)
                card_num = card_num + 1
        
        f_str = "Foundations:"
        for f in [2,0,3,1]:
            if (foundations[f] != 0):
                f_str = f_str + " " + get_card_suit(f) + "-" + get_card_num(foundations[f], print_ts)
        
        if (f_str != "Foundations:"):
            print f_str

        for i in output:
            print i
    elif (which_game == "fan"):
        output = [""] * 18
        for i in range(52-1):
            output[i%17] = output[i%17] + card_to_string(cards[i], (i >= 2*17), print_ts)
        output[17] = output[17] + card_to_string(cards[i+1], 1, print_ts);
        for i in output:
            print i
    else:
        print "Unknown game type " + which_game + "\n"
        
            
if __name__ == "__main__":
    sys.exit(shlomif_main(sys.argv))



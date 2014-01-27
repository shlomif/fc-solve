"use strict";

var ms_deal_24 = "4C 2C 9C 8C QS 4S 2H\n" +
"5H QH 3C AC 3H 4H QD\n" +
"QC 9S 6H 9H 3S KS 3D\n" +
"5D 2S JC 5C JH 6D AS\n" +
"2D KD TH TC TD 8D\n" +
"7H JS KH TS KC 7C\n" +
"AH 5S 6S AD 8H JD\n" +
"7S 6C 7D 4D 8S 9D\n" ;

var solution_for_deal_24__default = "-=-=-=-=-=-=-=-=-=-=-=-\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C 2C 9C 8C QS 4S 2H\n" +
": 5H QH 3C AC 3H 4H QD\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D AS\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
": AH 5S 6S AD 8H JD\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-A\n" +
"Freecells:\n" +
": 4C 2C 9C 8C QS 4S 2H\n" +
": 5H QH 3C AC 3H 4H QD\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
": AH 5S 6S AD 8H JD\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to freecell 0\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-A\n" +
"Freecells:  JD\n" +
": 4C 2C 9C 8C QS 4S 2H\n" +
": 5H QH 3C AC 3H 4H QD\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
": AH 5S 6S AD 8H\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to freecell 1\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-A\n" +
"Freecells:  JD  8H\n" +
": 4C 2C 9C 8C QS 4S 2H\n" +
": 5H QH 3C AC 3H 4H QD\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
": AH 5S 6S AD\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: H-0 C-0 D-A S-A\n" +
"Freecells:  JD  8H\n" +
": 4C 2C 9C 8C QS 4S 2H\n" +
": 5H QH 3C AC 3H 4H QD\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
": AH 5S 6S\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to freecell 2\n" +
"\n" +
"Foundations: H-0 C-0 D-A S-A\n" +
"Freecells:  JD  8H  2H\n" +
": 4C 2C 9C 8C QS 4S\n" +
": 5H QH 3C AC 3H 4H QD\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
": AH 5S 6S\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to freecell 3\n" +
"\n" +
"Foundations: H-0 C-0 D-A S-A\n" +
"Freecells:  JD  8H  2H  4S\n" +
": 4C 2C 9C 8C QS\n" +
": 5H QH 3C AC 3H 4H QD\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
": AH 5S 6S\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 0\n" +
"\n" +
"Foundations: H-0 C-0 D-A S-A\n" +
"Freecells:      8H  2H  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H QH 3C AC 3H 4H QD\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
": AH 5S 6S\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to freecell 0\n" +
"\n" +
"Foundations: H-0 C-0 D-A S-A\n" +
"Freecells:  6S  8H  2H  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H QH 3C AC 3H 4H QD\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
": AH 5S\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 3\n" +
"\n" +
"Foundations: H-0 C-0 D-A S-A\n" +
"Freecells:  6S  8H  2H  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H QH 3C AC 3H 4H QD\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
": AH\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: H-A C-0 D-A S-A\n" +
"Freecells:  6S  8H  2H  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H QH 3C AC 3H 4H QD\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
":\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to the foundations\n" +
"\n" +
"Foundations: H-2 C-0 D-A S-A\n" +
"Freecells:  6S  8H      4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H QH 3C AC 3H 4H QD\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
":\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to freecell 2\n" +
"\n" +
"Foundations: H-2 C-0 D-A S-A\n" +
"Freecells:  6S  8H  QD  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H QH 3C AC 3H 4H\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
":\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 1 to stack 6\n" +
"\n" +
"Foundations: H-2 C-0 D-A S-A\n" +
"Freecells:  6S  8H  QD  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H QH 3C AC 3H\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
": 4H\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: H-3 C-0 D-A S-A\n" +
"Freecells:  6S  8H  QD  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H QH 3C AC\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
": 4H\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: H-4 C-0 D-A S-A\n" +
"Freecells:  6S  8H  QD  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H QH 3C AC\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
":\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: H-4 C-A D-A S-A\n" +
"Freecells:  6S  8H  QD  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H QH 3C\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC 7C\n" +
":\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 5 to stack 6\n" +
"\n" +
"Foundations: H-4 C-A D-A S-A\n" +
"Freecells:  6S  8H  QD  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H QH 3C\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC\n" +
": 7C\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to stack 5\n" +
"\n" +
"Foundations: H-4 C-A D-A S-A\n" +
"Freecells:  6S  8H      4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H QH 3C\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS KC QD\n" +
": 7C\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 4\n" +
"\n" +
"Foundations: H-4 C-A D-A S-A\n" +
"Freecells:  6S  8H      4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H QH 3C\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D 7C\n" +
": 7H JS KH TS KC QD\n" +
":\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to freecell 2\n" +
"\n" +
"Foundations: H-4 C-A D-A S-A\n" +
"Freecells:  6S  8H  3C  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H QH\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D 7C\n" +
": 7H JS KH TS KC QD\n" +
":\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 1 to stack 6\n" +
"\n" +
"Foundations: H-4 C-A D-A S-A\n" +
"Freecells:  6S  8H  3C  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D 7C\n" +
": 7H JS KH TS KC QD\n" +
": QH\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 1\n" +
"\n" +
"Foundations: H-4 C-A D-A S-A\n" +
"Freecells:  6S  8H  3C\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H 4S\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D 7C\n" +
": 7H JS KH TS KC QD\n" +
": QH\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to freecell 3\n" +
"\n" +
"Foundations: H-4 C-A D-A S-A\n" +
"Freecells:  6S  8H  3C  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 5H\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D 7C\n" +
": 7H JS KH TS KC QD\n" +
": QH\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: H-5 C-A D-A S-A\n" +
"Freecells:  6S  8H  3C  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
":\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH 6D 5S\n" +
": 2D KD TH TC TD 8D 7C\n" +
": 7H JS KH TS KC QD\n" +
": QH\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 3 to stack 4\n" +
"\n" +
"Foundations: H-5 C-A D-A S-A\n" +
"Freecells:  6S  8H  3C  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
":\n" +
": QC 9S 6H 9H 3S KS 3D\n" +
": 5D 2S JC 5C JH\n" +
": 2D KD TH TC TD 8D 7C 6D 5S\n" +
": 7H JS KH TS KC QD\n" +
": QH\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 2 to stack 1\n" +
"\n" +
"Foundations: H-5 C-A D-A S-A\n" +
"Freecells:  6S  8H  3C  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 3D\n" +
": QC 9S 6H 9H 3S KS\n" +
": 5D 2S JC 5C JH\n" +
": 2D KD TH TC TD 8D 7C 6D 5S\n" +
": 7H JS KH TS KC QD\n" +
": QH\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 2\n" +
"\n" +
"Foundations: H-5 C-A D-A S-A\n" +
"Freecells:  6S  8H  3C  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 3D\n" +
": QC 9S 6H 9H 3S KS QH\n" +
": 5D 2S JC 5C JH\n" +
": 2D KD TH TC TD 8D 7C 6D 5S\n" +
": 7H JS KH TS KC QD\n" +
":\n" +
": 7S 6C 7D 4D 8S 9D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 7 to stack 6\n" +
"\n" +
"Foundations: H-5 C-A D-A S-A\n" +
"Freecells:  6S  8H  3C  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 3D\n" +
": QC 9S 6H 9H 3S KS QH\n" +
": 5D 2S JC 5C JH\n" +
": 2D KD TH TC TD 8D 7C 6D 5S\n" +
": 7H JS KH TS KC QD\n" +
": 9D\n" +
": 7S 6C 7D 4D 8S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 7 to stack 6\n" +
"\n" +
"Foundations: H-5 C-A D-A S-A\n" +
"Freecells:  6S  8H  3C  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 3D\n" +
": QC 9S 6H 9H 3S KS QH\n" +
": 5D 2S JC 5C JH\n" +
": 2D KD TH TC TD 8D 7C 6D 5S\n" +
": 7H JS KH TS KC QD\n" +
": 9D 8S\n" +
": 7S 6C 7D 4D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to stack 7\n" +
"\n" +
"Foundations: H-5 C-A D-A S-A\n" +
"Freecells:  6S  8H      4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 3D\n" +
": QC 9S 6H 9H 3S KS QH\n" +
": 5D 2S JC 5C JH\n" +
": 2D KD TH TC TD 8D 7C 6D 5S\n" +
": 7H JS KH TS KC QD\n" +
": 9D 8S\n" +
": 7S 6C 7D 4D 3C\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 7 to stack 4\n" +
"\n" +
"Foundations: H-5 C-A D-A S-A\n" +
"Freecells:  6S  8H      4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 3D\n" +
": QC 9S 6H 9H 3S KS QH\n" +
": 5D 2S JC 5C JH\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD\n" +
": 9D 8S\n" +
": 7S 6C 7D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 7\n" +
"\n" +
"Foundations: H-5 C-A D-A S-A\n" +
"Freecells:      8H      4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 3D\n" +
": QC 9S 6H 9H 3S KS QH\n" +
": 5D 2S JC 5C JH\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to freecell 2\n" +
"\n" +
"Foundations: H-5 C-A D-A S-A\n" +
"Freecells:      8H  JH  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 3D\n" +
": QC 9S 6H 9H 3S KS QH\n" +
": 5D 2S JC 5C\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to freecell 0\n" +
"\n" +
"Foundations: H-5 C-A D-A S-A\n" +
"Freecells:  5C  8H  JH  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 3D\n" +
": QC 9S 6H 9H 3S KS QH\n" +
": 5D 2S JC\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 3 to stack 5\n" +
"\n" +
"Foundations: H-5 C-A D-A S-A\n" +
"Freecells:  5C  8H  JH  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 3D\n" +
": QC 9S 6H 9H 3S KS QH\n" +
": 5D 2S\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: H-5 C-A D-A S-2\n" +
"Freecells:  5C  8H  JH  4S\n" +
": 4C 2C 9C 8C QS JD\n" +
": 3D\n" +
": QC 9S 6H 9H 3S KS QH\n" +
": 5D\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 3\n" +
"\n" +
"Foundations: H-5 C-A D-A S-2\n" +
"Freecells:  5C  8H  JH\n" +
": 4C 2C 9C 8C QS JD\n" +
": 3D\n" +
": QC 9S 6H 9H 3S KS QH\n" +
": 5D 4S\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 1 to stack 3\n" +
"\n" +
"Foundations: H-5 C-A D-A S-2\n" +
"Freecells:  5C  8H  JH\n" +
": 4C 2C 9C 8C QS JD\n" +
":\n" +
": QC 9S 6H 9H 3S KS QH\n" +
": 5D 4S 3D\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to freecell 3\n" +
"\n" +
"Foundations: H-5 C-A D-A S-2\n" +
"Freecells:  5C  8H  JH  QH\n" +
": 4C 2C 9C 8C QS JD\n" +
":\n" +
": QC 9S 6H 9H 3S KS\n" +
": 5D 4S 3D\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 2 to stack 1\n" +
"\n" +
"Foundations: H-5 C-A D-A S-2\n" +
"Freecells:  5C  8H  JH  QH\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS\n" +
": QC 9S 6H 9H 3S\n" +
": 5D 4S 3D\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: H-5 C-A D-A S-3\n" +
"Freecells:  5C  8H  JH  QH\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS\n" +
": QC 9S 6H 9H\n" +
": 5D 4S 3D\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 1\n" +
"\n" +
"Foundations: H-5 C-A D-A S-3\n" +
"Freecells:  5C  8H  JH\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 6H 9H\n" +
": 5D 4S 3D\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to freecell 3\n" +
"\n" +
"Foundations: H-5 C-A D-A S-3\n" +
"Freecells:  5C  8H  JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 6H\n" +
": 5D 4S 3D\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 2\n" +
"\n" +
"Foundations: H-5 C-A D-A S-3\n" +
"Freecells:      8H  JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 6H 5C\n" +
": 5D 4S 3D\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to freecell 0\n" +
"\n" +
"Foundations: H-5 C-A D-A S-3\n" +
"Freecells:  3D  8H  JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 6H 5C\n" +
": 5D 4S\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: H-5 C-A D-A S-4\n" +
"Freecells:  3D  8H  JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 6H 5C\n" +
": 5D\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 3 to stack 7\n" +
"\n" +
"Foundations: H-5 C-A D-A S-4\n" +
"Freecells:  3D  8H  JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 6H 5C\n" +
":\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S 5D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 2 to stack 3\n" +
"\n" +
"Foundations: H-5 C-A D-A S-4\n" +
"Freecells:  3D  8H  JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 6H\n" +
": 5C\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S 5D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: H-6 C-A D-A S-4\n" +
"Freecells:  3D  8H  JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S\n" +
": 5C\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S 5D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 1 to stack 2\n" +
"\n" +
"Foundations: H-6 C-A D-A S-4\n" +
"Freecells:  3D      JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 8H\n" +
": 5C\n" +
": 2D KD TH TC TD 8D 7C 6D 5S 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S 5D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 4 to stack 3\n" +
"\n" +
"Foundations: H-6 C-A D-A S-4\n" +
"Freecells:  3D      JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 8H\n" +
": 5C 4D 3C\n" +
": 2D KD TH TC TD 8D 7C 6D 5S\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S 5D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: H-6 C-A D-A S-5\n" +
"Freecells:  3D      JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 8H\n" +
": 5C 4D 3C\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S 5D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to freecell 1\n" +
"\n" +
"Foundations: H-6 C-A D-A S-5\n" +
"Freecells:  3D  5D  JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 8H\n" +
": 5C 4D 3C\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: H-6 C-A D-A S-6\n" +
"Freecells:  3D  5D  JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 8H\n" +
": 5C 4D 3C\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S\n" +
": 7S 6C 7D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 7 to stack 6\n" +
"\n" +
"Foundations: H-6 C-A D-A S-6\n" +
"Freecells:  3D  5D  JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 8H\n" +
": 5C 4D 3C\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S 7D\n" +
": 7S 6C\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 1 to stack 7\n" +
"\n" +
"Foundations: H-6 C-A D-A S-6\n" +
"Freecells:  3D      JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 8H\n" +
": 5C 4D 3C\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S 7D\n" +
": 7S 6C 5D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 7 to stack 6\n" +
"\n" +
"Foundations: H-6 C-A D-A S-6\n" +
"Freecells:  3D      JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 8H\n" +
": 5C 4D 3C\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S 7D 6C 5D\n" +
": 7S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D      JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S 8H\n" +
": 5C 4D 3C\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S 7D 6C 5D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to freecell 1\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D  8H  JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC 9S\n" +
": 5C 4D 3C\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S 7D 6C 5D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 2 to stack 7\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D  8H  JH  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC\n" +
": 5C 4D 3C\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to stack 2\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D  8H      9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC JH\n" +
": 5C 4D 3C\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 1 to stack 7\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D          9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC JH\n" +
": 5C 4D 3C\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 3 cards from stack 3 to stack 4\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D          9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC JH\n" +
":\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D 3C\n" +
": 7H JS KH TS KC QD JC\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to freecell 2\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D      JC  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC JH\n" +
":\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D 3C\n" +
": 7H JS KH TS KC QD\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to freecell 1\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D  QD  JC  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC JH\n" +
":\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D 3C\n" +
": 7H JS KH TS KC\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 5 to stack 3\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D  QD  JC  9H\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC JH\n" +
": KC\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D 3C\n" +
": 7H JS KH TS\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 5\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D  QD  JC\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC JH\n" +
": KC\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D 3C\n" +
": 7H JS KH TS 9H\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 1 to stack 3\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D      JC\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D 3C\n" +
": 7H JS KH TS 9H\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to stack 3\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D\n" +
": 4C 2C 9C 8C QS JD\n" +
": KS QH\n" +
": QC JH\n" +
": KC QD JC\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D 3C\n" +
": 7H JS KH TS 9H\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to freecell 3\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D          JD\n" +
": 4C 2C 9C 8C QS\n" +
": KS QH\n" +
": QC JH\n" +
": KC QD JC\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D 3C\n" +
": 7H JS KH TS 9H\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to freecell 1\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D  QS      JD\n" +
": 4C 2C 9C 8C\n" +
": KS QH\n" +
": QC JH\n" +
": KC QD JC\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D 3C\n" +
": 7H JS KH TS 9H\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 0 to stack 5\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D  QS      JD\n" +
": 4C 2C 9C\n" +
": KS QH\n" +
": QC JH\n" +
": KC QD JC\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D 3C\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to freecell 2\n" +
"\n" +
"Foundations: H-6 C-A D-A S-7\n" +
"Freecells:  3D  QS  9C  JD\n" +
": 4C 2C\n" +
": KS QH\n" +
": QC JH\n" +
": KC QD JC\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D 3C\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: H-6 C-2 D-A S-7\n" +
"Freecells:  3D  QS  9C  JD\n" +
": 4C\n" +
": KS QH\n" +
": QC JH\n" +
": KC QD JC\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D 3C\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: H-6 C-3 D-A S-7\n" +
"Freecells:  3D  QS  9C  JD\n" +
": 4C\n" +
": KS QH\n" +
": QC JH\n" +
": KC QD JC\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: H-6 C-4 D-A S-7\n" +
"Freecells:  3D  QS  9C  JD\n" +
":\n" +
": KS QH\n" +
": QC JH\n" +
": KC QD JC\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 0\n" +
"\n" +
"Foundations: H-6 C-4 D-A S-7\n" +
"Freecells:      QS  9C  JD\n" +
": 3D\n" +
": KS QH\n" +
": QC JH\n" +
": KC QD JC\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 3 to stack 1\n" +
"\n" +
"Foundations: H-6 C-4 D-A S-7\n" +
"Freecells:      QS  9C  JD\n" +
": 3D\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to freecell 0\n" +
"\n" +
"Foundations: H-6 C-4 D-A S-7\n" +
"Freecells:  3D  QS  9C  JD\n" +
":\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to stack 0\n" +
"\n" +
"Foundations: H-6 C-4 D-A S-7\n" +
"Freecells:  3D  QS      JD\n" +
": 9C\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S 8H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 7 to stack 0\n" +
"\n" +
"Foundations: H-6 C-4 D-A S-7\n" +
"Freecells:  3D  QS      JD\n" +
": 9C 8H\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 1 to stack 3\n" +
"\n" +
"Foundations: H-6 C-4 D-A S-7\n" +
"Freecells:  3D  QS      JD\n" +
": 9C 8H\n" +
": KS QH\n" +
": QC JH\n" +
": KC QD JC\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": 9S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to freecell 2\n" +
"\n" +
"Foundations: H-6 C-4 D-A S-7\n" +
"Freecells:  3D  QS  9S  JD\n" +
": 9C 8H\n" +
": KS QH\n" +
": QC JH\n" +
": KC QD JC\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 7\n" +
"\n" +
"Foundations: H-6 C-4 D-A S-7\n" +
"Freecells:      QS  9S  JD\n" +
": 9C 8H\n" +
": KS QH\n" +
": QC JH\n" +
": KC QD JC\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": 3D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 3 to stack 1\n" +
"\n" +
"Foundations: H-6 C-4 D-A S-7\n" +
"Freecells:      QS  9S  JD\n" +
": 9C 8H\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": 3D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to freecell 0\n" +
"\n" +
"Foundations: H-6 C-4 D-A S-7\n" +
"Freecells:  3D  QS  9S  JD\n" +
": 9C 8H\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 1 to stack 7\n" +
"\n" +
"Foundations: H-6 C-4 D-A S-7\n" +
"Freecells:  3D      9S  JD\n" +
": 9C 8H\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": QS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 7\n" +
"\n" +
"Foundations: H-6 C-4 D-A S-7\n" +
"Freecells:  3D      9S\n" +
": 9C 8H\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D 5C 4D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to freecell 1\n" +
"\n" +
"Foundations: H-6 C-4 D-A S-7\n" +
"Freecells:  3D  4D  9S\n" +
": 9C 8H\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D 5C\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: H-6 C-5 D-A S-7\n" +
"Freecells:  3D  4D  9S\n" +
": 9C 8H\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C 5D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to freecell 3\n" +
"\n" +
"Foundations: H-6 C-5 D-A S-7\n" +
"Freecells:  3D  4D  9S  5D\n" +
": 9C 8H\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D 6C\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: H-6 C-6 D-A S-7\n" +
"Freecells:  3D  4D  9S  5D\n" +
": 9C 8H\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS 9H 8C\n" +
": 9D 8S 7D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 5\n" +
"\n" +
"Foundations: H-6 C-6 D-A S-7\n" +
"Freecells:  3D  4D  9S  5D\n" +
": 9C 8H\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS 9H 8C 7D\n" +
": 9D 8S\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: H-6 C-6 D-A S-8\n" +
"Freecells:  3D  4D  9S  5D\n" +
": 9C 8H\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS 9H 8C 7D\n" +
": 9D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to the foundations\n" +
"\n" +
"Foundations: H-6 C-6 D-A S-9\n" +
"Freecells:  3D  4D      5D\n" +
": 9C 8H\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D 7C 6D\n" +
": 7H JS KH TS 9H 8C 7D\n" +
": 9D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 4 to stack 0\n" +
"\n" +
"Foundations: H-6 C-6 D-A S-9\n" +
"Freecells:  3D  4D      5D\n" +
": 9C 8H 7C 6D\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD 8D\n" +
": 7H JS KH TS 9H 8C 7D\n" +
": 9D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to freecell 2\n" +
"\n" +
"Foundations: H-6 C-6 D-A S-9\n" +
"Freecells:  3D  4D  8D  5D\n" +
": 9C 8H 7C 6D\n" +
": KS QH JC\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC TD\n" +
": 7H JS KH TS 9H 8C 7D\n" +
": 9D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 4 to stack 1\n" +
"\n" +
"Foundations: H-6 C-6 D-A S-9\n" +
"Freecells:  3D  4D  8D  5D\n" +
": 9C 8H 7C 6D\n" +
": KS QH JC TD\n" +
": QC JH\n" +
": KC QD\n" +
": 2D KD TH TC\n" +
": 7H JS KH TS 9H 8C 7D\n" +
": 9D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 4 to stack 2\n" +
"\n" +
"Foundations: H-6 C-6 D-A S-9\n" +
"Freecells:  3D  4D  8D  5D\n" +
": 9C 8H 7C 6D\n" +
": KS QH JC TD\n" +
": QC JH TC\n" +
": KC QD\n" +
": 2D KD TH\n" +
": 7H JS KH TS 9H 8C 7D\n" +
": 9D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 2\n" +
"\n" +
"Foundations: H-6 C-6 D-A S-9\n" +
"Freecells:  3D  4D  8D  5D\n" +
": 9C 8H 7C 6D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D\n" +
": KC QD\n" +
": 2D KD TH\n" +
": 7H JS KH TS 9H 8C 7D\n" +
":\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 6\n" +
"\n" +
"Foundations: H-6 C-6 D-A S-9\n" +
"Freecells:      4D  8D  5D\n" +
": 9C 8H 7C 6D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D\n" +
": KC QD\n" +
": 2D KD TH\n" +
": 7H JS KH TS 9H 8C 7D\n" +
": 3D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 5 to stack 2\n" +
"\n" +
"Foundations: H-6 C-6 D-A S-9\n" +
"Freecells:      4D  8D  5D\n" +
": 9C 8H 7C 6D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD\n" +
": 2D KD TH\n" +
": 7H JS KH TS 9H\n" +
": 3D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 5 to stack 7\n" +
"\n" +
"Foundations: H-6 C-6 D-A S-9\n" +
"Freecells:      4D  8D  5D\n" +
": 9C 8H 7C 6D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD\n" +
": 2D KD TH\n" +
": 7H JS KH\n" +
": 3D\n" +
": QS JD TS 9H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to freecell 0\n" +
"\n" +
"Foundations: H-6 C-6 D-A S-9\n" +
"Freecells:  KH  4D  8D  5D\n" +
": 9C 8H 7C 6D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD\n" +
": 2D KD TH\n" +
": 7H JS\n" +
": 3D\n" +
": QS JD TS 9H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 5 to stack 3\n" +
"\n" +
"Foundations: H-6 C-6 D-A S-9\n" +
"Freecells:  KH  4D  8D  5D\n" +
": 9C 8H 7C 6D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD JS\n" +
": 2D KD TH\n" +
": 7H\n" +
": 3D\n" +
": QS JD TS 9H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to the foundations\n" +
"\n" +
"Foundations: H-7 C-6 D-A S-9\n" +
"Freecells:  KH  4D  8D  5D\n" +
": 9C 8H 7C 6D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD JS\n" +
": 2D KD TH\n" +
":\n" +
": 3D\n" +
": QS JD TS 9H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 0 to stack 5\n" +
"\n" +
"Foundations: H-7 C-6 D-A S-9\n" +
"Freecells:  KH  4D  8D  5D\n" +
": 9C 8H 7C\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD JS\n" +
": 2D KD TH\n" +
": 6D\n" +
": 3D\n" +
": QS JD TS 9H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: H-7 C-7 D-A S-9\n" +
"Freecells:  KH  4D  8D  5D\n" +
": 9C 8H\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD JS\n" +
": 2D KD TH\n" +
": 6D\n" +
": 3D\n" +
": QS JD TS 9H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: H-8 C-7 D-A S-9\n" +
"Freecells:  KH  4D  8D  5D\n" +
": 9C\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD JS\n" +
": 2D KD TH\n" +
": 6D\n" +
": 3D\n" +
": QS JD TS 9H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: H-9 C-7 D-A S-9\n" +
"Freecells:  KH  4D  8D  5D\n" +
": 9C\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD JS\n" +
": 2D KD TH\n" +
": 6D\n" +
": 3D\n" +
": QS JD TS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: H-T C-7 D-A S-9\n" +
"Freecells:  KH  4D  8D  5D\n" +
": 9C\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD JS\n" +
": 2D KD\n" +
": 6D\n" +
": 3D\n" +
": QS JD TS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: H-T C-7 D-A S-T\n" +
"Freecells:  KH  4D  8D  5D\n" +
": 9C\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD JS\n" +
": 2D KD\n" +
": 6D\n" +
": 3D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: H-T C-7 D-A S-J\n" +
"Freecells:  KH  4D  8D  5D\n" +
": 9C\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD\n" +
": 2D KD\n" +
": 6D\n" +
": 3D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to stack 0\n" +
"\n" +
"Foundations: H-T C-7 D-A S-J\n" +
"Freecells:  KH  4D      5D\n" +
": 9C 8D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD\n" +
": 2D KD\n" +
": 6D\n" +
": 3D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to freecell 2\n" +
"\n" +
"Foundations: H-T C-7 D-A S-J\n" +
"Freecells:  KH  4D  KD  5D\n" +
": 9C 8D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD\n" +
": 2D\n" +
": 6D\n" +
": 3D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: H-T C-7 D-2 S-J\n" +
"Freecells:  KH  4D  KD  5D\n" +
": 9C 8D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD\n" +
":\n" +
": 6D\n" +
": 3D\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: H-T C-7 D-3 S-J\n" +
"Freecells:  KH  4D  KD  5D\n" +
": 9C 8D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD\n" +
":\n" +
": 6D\n" +
":\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 1 to the foundations\n" +
"\n" +
"Foundations: H-T C-7 D-4 S-J\n" +
"Freecells:  KH      KD  5D\n" +
": 9C 8D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD\n" +
":\n" +
": 6D\n" +
":\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to the foundations\n" +
"\n" +
"Foundations: H-T C-7 D-5 S-J\n" +
"Freecells:  KH      KD\n" +
": 9C 8D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD\n" +
":\n" +
": 6D\n" +
":\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to the foundations\n" +
"\n" +
"Foundations: H-T C-7 D-6 S-J\n" +
"Freecells:  KH      KD\n" +
": 9C 8D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C 7D\n" +
": KC QD\n" +
":\n" +
":\n" +
":\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: H-T C-7 D-7 S-J\n" +
"Freecells:  KH      KD\n" +
": 9C 8D\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C\n" +
": KC QD\n" +
":\n" +
":\n" +
":\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: H-T C-7 D-8 S-J\n" +
"Freecells:  KH      KD\n" +
": 9C\n" +
": KS QH JC TD\n" +
": QC JH TC 9D 8C\n" +
": KC QD\n" +
":\n" +
":\n" +
":\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: H-T C-8 D-8 S-J\n" +
"Freecells:  KH      KD\n" +
": 9C\n" +
": KS QH JC TD\n" +
": QC JH TC 9D\n" +
": KC QD\n" +
":\n" +
":\n" +
":\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: H-T C-9 D-8 S-J\n" +
"Freecells:  KH      KD\n" +
":\n" +
": KS QH JC TD\n" +
": QC JH TC 9D\n" +
": KC QD\n" +
":\n" +
":\n" +
":\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: H-T C-9 D-9 S-J\n" +
"Freecells:  KH      KD\n" +
":\n" +
": KS QH JC TD\n" +
": QC JH TC\n" +
": KC QD\n" +
":\n" +
":\n" +
":\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: H-T C-T D-9 S-J\n" +
"Freecells:  KH      KD\n" +
":\n" +
": KS QH JC TD\n" +
": QC JH\n" +
": KC QD\n" +
":\n" +
":\n" +
":\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: H-J C-T D-9 S-J\n" +
"Freecells:  KH      KD\n" +
":\n" +
": KS QH JC TD\n" +
": QC\n" +
": KC QD\n" +
":\n" +
":\n" +
":\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: H-J C-T D-T S-J\n" +
"Freecells:  KH      KD\n" +
":\n" +
": KS QH JC\n" +
": QC\n" +
": KC QD\n" +
":\n" +
":\n" +
":\n" +
": QS JD\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: H-J C-T D-J S-J\n" +
"Freecells:  KH      KD\n" +
":\n" +
": KS QH JC\n" +
": QC\n" +
": KC QD\n" +
":\n" +
":\n" +
":\n" +
": QS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: H-J C-T D-J S-Q\n" +
"Freecells:  KH      KD\n" +
":\n" +
": KS QH JC\n" +
": QC\n" +
": KC QD\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: H-J C-T D-Q S-Q\n" +
"Freecells:  KH      KD\n" +
":\n" +
": KS QH JC\n" +
": QC\n" +
": KC\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: H-J C-J D-Q S-Q\n" +
"Freecells:  KH      KD\n" +
":\n" +
": KS QH\n" +
": QC\n" +
": KC\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: H-J C-Q D-Q S-Q\n" +
"Freecells:  KH      KD\n" +
":\n" +
": KS QH\n" +
":\n" +
": KC\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: H-J C-K D-Q S-Q\n" +
"Freecells:  KH      KD\n" +
":\n" +
": KS QH\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: H-Q C-K D-Q S-Q\n" +
"Freecells:  KH      KD\n" +
":\n" +
": KS\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: H-Q C-K D-Q S-K\n" +
"Freecells:  KH      KD\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to the foundations\n" +
"\n" +
"Foundations: H-K C-K D-Q S-K\n" +
"Freecells:          KD\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to the foundations\n" +
"\n" +
"Foundations: H-K C-K D-K S-K\n" +
"Freecells:\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n\n";

var solution_for_deal_24__default__with_unicoded_suits =
"-=-=-=-=-=-=-=-=-=-=-=-\n" +
"\n" +
"Foundations: ♥-0 ♣-0 ♦-0 ♠-0\n" +
"Freecells:\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ 4♠ 2♥\n" +
": 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ A♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
": A♥ 5♠ 6♠ A♦ 8♥ J♦\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: ♥-0 ♣-0 ♦-0 ♠-A\n" +
"Freecells:\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ 4♠ 2♥\n" +
": 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
": A♥ 5♠ 6♠ A♦ 8♥ J♦\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to freecell 0\n" +
"\n" +
"Foundations: ♥-0 ♣-0 ♦-0 ♠-A\n" +
"Freecells:  J♦\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ 4♠ 2♥\n" +
": 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
": A♥ 5♠ 6♠ A♦ 8♥\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to freecell 1\n" +
"\n" +
"Foundations: ♥-0 ♣-0 ♦-0 ♠-A\n" +
"Freecells:  J♦  8♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ 4♠ 2♥\n" +
": 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
": A♥ 5♠ 6♠ A♦\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: ♥-0 ♣-0 ♦-A ♠-A\n" +
"Freecells:  J♦  8♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ 4♠ 2♥\n" +
": 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
": A♥ 5♠ 6♠\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to freecell 2\n" +
"\n" +
"Foundations: ♥-0 ♣-0 ♦-A ♠-A\n" +
"Freecells:  J♦  8♥  2♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ 4♠\n" +
": 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
": A♥ 5♠ 6♠\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to freecell 3\n" +
"\n" +
"Foundations: ♥-0 ♣-0 ♦-A ♠-A\n" +
"Freecells:  J♦  8♥  2♥  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠\n" +
": 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
": A♥ 5♠ 6♠\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 0\n" +
"\n" +
"Foundations: ♥-0 ♣-0 ♦-A ♠-A\n" +
"Freecells:      8♥  2♥  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
": A♥ 5♠ 6♠\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to freecell 0\n" +
"\n" +
"Foundations: ♥-0 ♣-0 ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  2♥  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
": A♥ 5♠\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 3\n" +
"\n" +
"Foundations: ♥-0 ♣-0 ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  2♥  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
": A♥\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: ♥-A ♣-0 ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  2♥  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
":\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to the foundations\n" +
"\n" +
"Foundations: ♥-2 ♣-0 ♦-A ♠-A\n" +
"Freecells:  6♠  8♥      4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
":\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to freecell 2\n" +
"\n" +
"Foundations: ♥-2 ♣-0 ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  Q♦  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ Q♥ 3♣ A♣ 3♥ 4♥\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
":\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 1 to stack 6\n" +
"\n" +
"Foundations: ♥-2 ♣-0 ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  Q♦  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ Q♥ 3♣ A♣ 3♥\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
": 4♥\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-3 ♣-0 ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  Q♦  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ Q♥ 3♣ A♣\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
": 4♥\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: ♥-4 ♣-0 ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  Q♦  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ Q♥ 3♣ A♣\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
":\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-4 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  Q♦  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ Q♥ 3♣\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ 7♣\n" +
":\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 5 to stack 6\n" +
"\n" +
"Foundations: ♥-4 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  Q♦  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ Q♥ 3♣\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣\n" +
": 7♣\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to stack 5\n" +
"\n" +
"Foundations: ♥-4 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥      4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ Q♥ 3♣\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": 7♣\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 4\n" +
"\n" +
"Foundations: ♥-4 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥      4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ Q♥ 3♣\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
":\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to freecell 2\n" +
"\n" +
"Foundations: ♥-4 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  3♣  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ Q♥\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
":\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 1 to stack 6\n" +
"\n" +
"Foundations: ♥-4 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  3♣  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": Q♥\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 1\n" +
"\n" +
"Foundations: ♥-4 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  3♣\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥ 4♠\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": Q♥\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to freecell 3\n" +
"\n" +
"Foundations: ♥-4 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  3♣  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 5♥\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": Q♥\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  3♣  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
":\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": Q♥\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 3 to stack 4\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  3♣  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
":\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦\n" +
": 5♦ 2♠ J♣ 5♣ J♥\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": Q♥\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 2 to stack 1\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  3♣  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 3♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠\n" +
": 5♦ 2♠ J♣ 5♣ J♥\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": Q♥\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 2\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  3♣  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 3♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥\n" +
": 5♦ 2♠ J♣ 5♣ J♥\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
":\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠ 9♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 7 to stack 6\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  3♣  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 3♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥\n" +
": 5♦ 2♠ J♣ 5♣ J♥\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": 9♦\n" +
": 7♠ 6♣ 7♦ 4♦ 8♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 7 to stack 6\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥  3♣  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 3♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥\n" +
": 5♦ 2♠ J♣ 5♣ J♥\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 4♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to stack 7\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥      4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 3♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥\n" +
": 5♦ 2♠ J♣ 5♣ J♥\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 4♦ 3♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 7 to stack 4\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-A\n" +
"Freecells:  6♠  8♥      4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 3♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥\n" +
": 5♦ 2♠ J♣ 5♣ J♥\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 7\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-A\n" +
"Freecells:      8♥      4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 3♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥\n" +
": 5♦ 2♠ J♣ 5♣ J♥\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to freecell 2\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-A\n" +
"Freecells:      8♥  J♥  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 3♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥\n" +
": 5♦ 2♠ J♣ 5♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to freecell 0\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-A\n" +
"Freecells:  5♣  8♥  J♥  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 3♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥\n" +
": 5♦ 2♠ J♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 3 to stack 5\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-A\n" +
"Freecells:  5♣  8♥  J♥  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 3♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥\n" +
": 5♦ 2♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-2\n" +
"Freecells:  5♣  8♥  J♥  4♠\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 3♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥\n" +
": 5♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 3\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-2\n" +
"Freecells:  5♣  8♥  J♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": 3♦\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥\n" +
": 5♦ 4♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 1 to stack 3\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-2\n" +
"Freecells:  5♣  8♥  J♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
":\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥\n" +
": 5♦ 4♠ 3♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to freecell 3\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-2\n" +
"Freecells:  5♣  8♥  J♥  Q♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
":\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠ K♠\n" +
": 5♦ 4♠ 3♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 2 to stack 1\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-2\n" +
"Freecells:  5♣  8♥  J♥  Q♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠\n" +
": Q♣ 9♠ 6♥ 9♥ 3♠\n" +
": 5♦ 4♠ 3♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-3\n" +
"Freecells:  5♣  8♥  J♥  Q♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠\n" +
": Q♣ 9♠ 6♥ 9♥\n" +
": 5♦ 4♠ 3♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 1\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-3\n" +
"Freecells:  5♣  8♥  J♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 6♥ 9♥\n" +
": 5♦ 4♠ 3♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to freecell 3\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-3\n" +
"Freecells:  5♣  8♥  J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 6♥\n" +
": 5♦ 4♠ 3♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 2\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-3\n" +
"Freecells:      8♥  J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 6♥ 5♣\n" +
": 5♦ 4♠ 3♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to freecell 0\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-3\n" +
"Freecells:  3♦  8♥  J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 6♥ 5♣\n" +
": 5♦ 4♠\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-4\n" +
"Freecells:  3♦  8♥  J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 6♥ 5♣\n" +
": 5♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 3 to stack 7\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-4\n" +
"Freecells:  3♦  8♥  J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 6♥ 5♣\n" +
":\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 2 to stack 3\n" +
"\n" +
"Foundations: ♥-5 ♣-A ♦-A ♠-4\n" +
"Freecells:  3♦  8♥  J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 6♥\n" +
": 5♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-4\n" +
"Freecells:  3♦  8♥  J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠\n" +
": 5♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 1 to stack 2\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-4\n" +
"Freecells:  3♦      J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 8♥\n" +
": 5♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 4 to stack 3\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-4\n" +
"Freecells:  3♦      J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 8♥\n" +
": 5♣ 4♦ 3♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-5\n" +
"Freecells:  3♦      J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 8♥\n" +
": 5♣ 4♦ 3♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to freecell 1\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-5\n" +
"Freecells:  3♦  5♦  J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 8♥\n" +
": 5♣ 4♦ 3♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦ 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-6\n" +
"Freecells:  3♦  5♦  J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 8♥\n" +
": 5♣ 4♦ 3♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠\n" +
": 7♠ 6♣ 7♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 7 to stack 6\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-6\n" +
"Freecells:  3♦  5♦  J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 8♥\n" +
": 5♣ 4♦ 3♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠ 7♦\n" +
": 7♠ 6♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 1 to stack 7\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-6\n" +
"Freecells:  3♦      J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 8♥\n" +
": 5♣ 4♦ 3♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠ 7♦\n" +
": 7♠ 6♣ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 7 to stack 6\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-6\n" +
"Freecells:  3♦      J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 8♥\n" +
": 5♣ 4♦ 3♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 7♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦      J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠ 8♥\n" +
": 5♣ 4♦ 3♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to freecell 1\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦  8♥  J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ 9♠\n" +
": 5♣ 4♦ 3♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 2 to stack 7\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦  8♥  J♥  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣\n" +
": 5♣ 4♦ 3♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to stack 2\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦  8♥      9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": 5♣ 4♦ 3♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 1 to stack 7\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦          9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": 5♣ 4♦ 3♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 3 cards from stack 3 to stack 4\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦          9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
":\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦ J♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to freecell 2\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦      J♣  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
":\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣ Q♦\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to freecell 1\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦  Q♦  J♣  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
":\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ K♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 5 to stack 3\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦  Q♦  J♣  9♥\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 5\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦  Q♦  J♣\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ 9♥\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 1 to stack 3\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦      J♣\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ 9♥\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to stack 3\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠ J♦\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣ Q♦ J♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ 9♥\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to freecell 3\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦          J♦\n" +
": 4♣ 2♣ 9♣ 8♣ Q♠\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣ Q♦ J♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ 9♥\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to freecell 1\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦  Q♠      J♦\n" +
": 4♣ 2♣ 9♣ 8♣\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣ Q♦ J♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ 9♥\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 0 to stack 5\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦  Q♠      J♦\n" +
": 4♣ 2♣ 9♣\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣ Q♦ J♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to freecell 2\n" +
"\n" +
"Foundations: ♥-6 ♣-A ♦-A ♠-7\n" +
"Freecells:  3♦  Q♠  9♣  J♦\n" +
": 4♣ 2♣\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣ Q♦ J♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: ♥-6 ♣-2 ♦-A ♠-7\n" +
"Freecells:  3♦  Q♠  9♣  J♦\n" +
": 4♣\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣ Q♦ J♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-6 ♣-3 ♦-A ♠-7\n" +
"Freecells:  3♦  Q♠  9♣  J♦\n" +
": 4♣\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣ Q♦ J♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: ♥-6 ♣-4 ♦-A ♠-7\n" +
"Freecells:  3♦  Q♠  9♣  J♦\n" +
":\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣ Q♦ J♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 0\n" +
"\n" +
"Foundations: ♥-6 ♣-4 ♦-A ♠-7\n" +
"Freecells:      Q♠  9♣  J♦\n" +
": 3♦\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣ Q♦ J♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 3 to stack 1\n" +
"\n" +
"Foundations: ♥-6 ♣-4 ♦-A ♠-7\n" +
"Freecells:      Q♠  9♣  J♦\n" +
": 3♦\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to freecell 0\n" +
"\n" +
"Foundations: ♥-6 ♣-4 ♦-A ♠-7\n" +
"Freecells:  3♦  Q♠  9♣  J♦\n" +
":\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to stack 0\n" +
"\n" +
"Foundations: ♥-6 ♣-4 ♦-A ♠-7\n" +
"Freecells:  3♦  Q♠      J♦\n" +
": 9♣\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠ 8♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 7 to stack 0\n" +
"\n" +
"Foundations: ♥-6 ♣-4 ♦-A ♠-7\n" +
"Freecells:  3♦  Q♠      J♦\n" +
": 9♣ 8♥\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 1 to stack 3\n" +
"\n" +
"Foundations: ♥-6 ♣-4 ♦-A ♠-7\n" +
"Freecells:  3♦  Q♠      J♦\n" +
": 9♣ 8♥\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣ Q♦ J♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 9♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to freecell 2\n" +
"\n" +
"Foundations: ♥-6 ♣-4 ♦-A ♠-7\n" +
"Freecells:  3♦  Q♠  9♠  J♦\n" +
": 9♣ 8♥\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣ Q♦ J♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 7\n" +
"\n" +
"Foundations: ♥-6 ♣-4 ♦-A ♠-7\n" +
"Freecells:      Q♠  9♠  J♦\n" +
": 9♣ 8♥\n" +
": K♠ Q♥\n" +
": Q♣ J♥\n" +
": K♣ Q♦ J♣\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 3♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 3 to stack 1\n" +
"\n" +
"Foundations: ♥-6 ♣-4 ♦-A ♠-7\n" +
"Freecells:      Q♠  9♠  J♦\n" +
": 9♣ 8♥\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": 3♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to freecell 0\n" +
"\n" +
"Foundations: ♥-6 ♣-4 ♦-A ♠-7\n" +
"Freecells:  3♦  Q♠  9♠  J♦\n" +
": 9♣ 8♥\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 1 to stack 7\n" +
"\n" +
"Foundations: ♥-6 ♣-4 ♦-A ♠-7\n" +
"Freecells:  3♦      9♠  J♦\n" +
": 9♣ 8♥\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": Q♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 7\n" +
"\n" +
"Foundations: ♥-6 ♣-4 ♦-A ♠-7\n" +
"Freecells:  3♦      9♠\n" +
": 9♣ 8♥\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to freecell 1\n" +
"\n" +
"Foundations: ♥-6 ♣-4 ♦-A ♠-7\n" +
"Freecells:  3♦  4♦  9♠\n" +
": 9♣ 8♥\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-6 ♣-5 ♦-A ♠-7\n" +
"Freecells:  3♦  4♦  9♠\n" +
": 9♣ 8♥\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣ 5♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to freecell 3\n" +
"\n" +
"Foundations: ♥-6 ♣-5 ♦-A ♠-7\n" +
"Freecells:  3♦  4♦  9♠  5♦\n" +
": 9♣ 8♥\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦ 6♣\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: ♥-6 ♣-6 ♦-A ♠-7\n" +
"Freecells:  3♦  4♦  9♠  5♦\n" +
": 9♣ 8♥\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣\n" +
": 9♦ 8♠ 7♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 5\n" +
"\n" +
"Foundations: ♥-6 ♣-6 ♦-A ♠-7\n" +
"Freecells:  3♦  4♦  9♠  5♦\n" +
": 9♣ 8♥\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦\n" +
": 9♦ 8♠\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: ♥-6 ♣-6 ♦-A ♠-8\n" +
"Freecells:  3♦  4♦  9♠  5♦\n" +
": 9♣ 8♥\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦\n" +
": 9♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to the foundations\n" +
"\n" +
"Foundations: ♥-6 ♣-6 ♦-A ♠-9\n" +
"Freecells:  3♦  4♦      5♦\n" +
": 9♣ 8♥\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦\n" +
": 9♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 4 to stack 0\n" +
"\n" +
"Foundations: ♥-6 ♣-6 ♦-A ♠-9\n" +
"Freecells:  3♦  4♦      5♦\n" +
": 9♣ 8♥ 7♣ 6♦\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦ 8♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦\n" +
": 9♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to freecell 2\n" +
"\n" +
"Foundations: ♥-6 ♣-6 ♦-A ♠-9\n" +
"Freecells:  3♦  4♦  8♦  5♦\n" +
": 9♣ 8♥ 7♣ 6♦\n" +
": K♠ Q♥ J♣\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣ T♦\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦\n" +
": 9♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 4 to stack 1\n" +
"\n" +
"Foundations: ♥-6 ♣-6 ♦-A ♠-9\n" +
"Freecells:  3♦  4♦  8♦  5♦\n" +
": 9♣ 8♥ 7♣ 6♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥ T♣\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦\n" +
": 9♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 4 to stack 2\n" +
"\n" +
"Foundations: ♥-6 ♣-6 ♦-A ♠-9\n" +
"Freecells:  3♦  4♦  8♦  5♦\n" +
": 9♣ 8♥ 7♣ 6♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦\n" +
": 9♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 2\n" +
"\n" +
"Foundations: ♥-6 ♣-6 ♦-A ♠-9\n" +
"Freecells:  3♦  4♦  8♦  5♦\n" +
": 9♣ 8♥ 7♣ 6♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦\n" +
":\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 6\n" +
"\n" +
"Foundations: ♥-6 ♣-6 ♦-A ♠-9\n" +
"Freecells:      4♦  8♦  5♦\n" +
": 9♣ 8♥ 7♣ 6♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥\n" +
": 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦\n" +
": 3♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 5 to stack 2\n" +
"\n" +
"Foundations: ♥-6 ♣-6 ♦-A ♠-9\n" +
"Freecells:      4♦  8♦  5♦\n" +
": 9♣ 8♥ 7♣ 6♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥\n" +
": 7♥ J♠ K♥ T♠ 9♥\n" +
": 3♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 5 to stack 7\n" +
"\n" +
"Foundations: ♥-6 ♣-6 ♦-A ♠-9\n" +
"Freecells:      4♦  8♦  5♦\n" +
": 9♣ 8♥ 7♣ 6♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥\n" +
": 7♥ J♠ K♥\n" +
": 3♦\n" +
": Q♠ J♦ T♠ 9♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to freecell 0\n" +
"\n" +
"Foundations: ♥-6 ♣-6 ♦-A ♠-9\n" +
"Freecells:  K♥  4♦  8♦  5♦\n" +
": 9♣ 8♥ 7♣ 6♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦\n" +
": 2♦ K♦ T♥\n" +
": 7♥ J♠\n" +
": 3♦\n" +
": Q♠ J♦ T♠ 9♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 5 to stack 3\n" +
"\n" +
"Foundations: ♥-6 ♣-6 ♦-A ♠-9\n" +
"Freecells:  K♥  4♦  8♦  5♦\n" +
": 9♣ 8♥ 7♣ 6♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦ J♠\n" +
": 2♦ K♦ T♥\n" +
": 7♥\n" +
": 3♦\n" +
": Q♠ J♦ T♠ 9♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-6 ♦-A ♠-9\n" +
"Freecells:  K♥  4♦  8♦  5♦\n" +
": 9♣ 8♥ 7♣ 6♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦ J♠\n" +
": 2♦ K♦ T♥\n" +
":\n" +
": 3♦\n" +
": Q♠ J♦ T♠ 9♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 0 to stack 5\n" +
"\n" +
"Foundations: ♥-7 ♣-6 ♦-A ♠-9\n" +
"Freecells:  K♥  4♦  8♦  5♦\n" +
": 9♣ 8♥ 7♣\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦ J♠\n" +
": 2♦ K♦ T♥\n" +
": 6♦\n" +
": 3♦\n" +
": Q♠ J♦ T♠ 9♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-7 ♦-A ♠-9\n" +
"Freecells:  K♥  4♦  8♦  5♦\n" +
": 9♣ 8♥\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦ J♠\n" +
": 2♦ K♦ T♥\n" +
": 6♦\n" +
": 3♦\n" +
": Q♠ J♦ T♠ 9♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: ♥-8 ♣-7 ♦-A ♠-9\n" +
"Freecells:  K♥  4♦  8♦  5♦\n" +
": 9♣\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦ J♠\n" +
": 2♦ K♦ T♥\n" +
": 6♦\n" +
": 3♦\n" +
": Q♠ J♦ T♠ 9♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: ♥-9 ♣-7 ♦-A ♠-9\n" +
"Freecells:  K♥  4♦  8♦  5♦\n" +
": 9♣\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦ J♠\n" +
": 2♦ K♦ T♥\n" +
": 6♦\n" +
": 3♦\n" +
": Q♠ J♦ T♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-7 ♦-A ♠-9\n" +
"Freecells:  K♥  4♦  8♦  5♦\n" +
": 9♣\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦ J♠\n" +
": 2♦ K♦\n" +
": 6♦\n" +
": 3♦\n" +
": Q♠ J♦ T♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-7 ♦-A ♠-T\n" +
"Freecells:  K♥  4♦  8♦  5♦\n" +
": 9♣\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦ J♠\n" +
": 2♦ K♦\n" +
": 6♦\n" +
": 3♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-7 ♦-A ♠-J\n" +
"Freecells:  K♥  4♦  8♦  5♦\n" +
": 9♣\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦\n" +
": 2♦ K♦\n" +
": 6♦\n" +
": 3♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to stack 0\n" +
"\n" +
"Foundations: ♥-T ♣-7 ♦-A ♠-J\n" +
"Freecells:  K♥  4♦      5♦\n" +
": 9♣ 8♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦\n" +
": 2♦ K♦\n" +
": 6♦\n" +
": 3♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to freecell 2\n" +
"\n" +
"Foundations: ♥-T ♣-7 ♦-A ♠-J\n" +
"Freecells:  K♥  4♦  K♦  5♦\n" +
": 9♣ 8♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦\n" +
": 2♦\n" +
": 6♦\n" +
": 3♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-7 ♦-2 ♠-J\n" +
"Freecells:  K♥  4♦  K♦  5♦\n" +
": 9♣ 8♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦\n" +
":\n" +
": 6♦\n" +
": 3♦\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-7 ♦-3 ♠-J\n" +
"Freecells:  K♥  4♦  K♦  5♦\n" +
": 9♣ 8♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦\n" +
":\n" +
": 6♦\n" +
":\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 1 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-7 ♦-4 ♠-J\n" +
"Freecells:  K♥      K♦  5♦\n" +
": 9♣ 8♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦\n" +
":\n" +
": 6♦\n" +
":\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-7 ♦-5 ♠-J\n" +
"Freecells:  K♥      K♦\n" +
": 9♣ 8♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦\n" +
":\n" +
": 6♦\n" +
":\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-7 ♦-6 ♠-J\n" +
"Freecells:  K♥      K♦\n" +
": 9♣ 8♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣ 7♦\n" +
": K♣ Q♦\n" +
":\n" +
":\n" +
":\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-7 ♦-7 ♠-J\n" +
"Freecells:  K♥      K♦\n" +
": 9♣ 8♦\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣\n" +
": K♣ Q♦\n" +
":\n" +
":\n" +
":\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-7 ♦-8 ♠-J\n" +
"Freecells:  K♥      K♦\n" +
": 9♣\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦ 8♣\n" +
": K♣ Q♦\n" +
":\n" +
":\n" +
":\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-8 ♦-8 ♠-J\n" +
"Freecells:  K♥      K♦\n" +
": 9♣\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦\n" +
": K♣ Q♦\n" +
":\n" +
":\n" +
":\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-9 ♦-8 ♠-J\n" +
"Freecells:  K♥      K♦\n" +
":\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣ 9♦\n" +
": K♣ Q♦\n" +
":\n" +
":\n" +
":\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-9 ♦-9 ♠-J\n" +
"Freecells:  K♥      K♦\n" +
":\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥ T♣\n" +
": K♣ Q♦\n" +
":\n" +
":\n" +
":\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-T ♦-9 ♠-J\n" +
"Freecells:  K♥      K♦\n" +
":\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣ J♥\n" +
": K♣ Q♦\n" +
":\n" +
":\n" +
":\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: ♥-J ♣-T ♦-9 ♠-J\n" +
"Freecells:  K♥      K♦\n" +
":\n" +
": K♠ Q♥ J♣ T♦\n" +
": Q♣\n" +
": K♣ Q♦\n" +
":\n" +
":\n" +
":\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-J ♣-T ♦-T ♠-J\n" +
"Freecells:  K♥      K♦\n" +
":\n" +
": K♠ Q♥ J♣\n" +
": Q♣\n" +
": K♣ Q♦\n" +
":\n" +
":\n" +
":\n" +
": Q♠ J♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: ♥-J ♣-T ♦-J ♠-J\n" +
"Freecells:  K♥      K♦\n" +
":\n" +
": K♠ Q♥ J♣\n" +
": Q♣\n" +
": K♣ Q♦\n" +
":\n" +
":\n" +
":\n" +
": Q♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: ♥-J ♣-T ♦-J ♠-Q\n" +
"Freecells:  K♥      K♦\n" +
":\n" +
": K♠ Q♥ J♣\n" +
": Q♣\n" +
": K♣ Q♦\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: ♥-J ♣-T ♦-Q ♠-Q\n" +
"Freecells:  K♥      K♦\n" +
":\n" +
": K♠ Q♥ J♣\n" +
": Q♣\n" +
": K♣\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-J ♣-J ♦-Q ♠-Q\n" +
"Freecells:  K♥      K♦\n" +
":\n" +
": K♠ Q♥\n" +
": Q♣\n" +
": K♣\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: ♥-J ♣-Q ♦-Q ♠-Q\n" +
"Freecells:  K♥      K♦\n" +
":\n" +
": K♠ Q♥\n" +
":\n" +
": K♣\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: ♥-J ♣-K ♦-Q ♠-Q\n" +
"Freecells:  K♥      K♦\n" +
":\n" +
": K♠ Q♥\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-Q ♣-K ♦-Q ♠-Q\n" +
"Freecells:  K♥      K♦\n" +
":\n" +
": K♠\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-Q ♣-K ♦-Q ♠-K\n" +
"Freecells:  K♥      K♦\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to the foundations\n" +
"\n" +
"Foundations: ♥-K ♣-K ♦-Q ♠-K\n" +
"Freecells:          K♦\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to the foundations\n" +
"\n" +
"Foundations: ♥-K ♣-K ♦-K ♠-K\n" +
"Freecells:\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n\n";

var pysol_simple_simon_deal_24 = "4C QH 3C 8C AD JH 8S KS\n" +
"5H 9S 6H AC 4D TD 4S 6D\n" +
"QC 2S JC 9H QS KC 4H 8D\n" +
"5D KD TH 5C 3H 8H 7C\n" +
"2D JS KH TC 3S JD\n" +
"7H 5S 6S TS 9D\n" +
"AH 6C 7D 2H\n" +
"7S 9C QD\n" +
"2C 3D\n" +
"AS\n" +
"";

var solution_for_pysol_simple_simon_deal_24__default = "-=-=-=-=-=-=-=-=-=-=-=-\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS\n" +
": 5H 9S 6H AC 4D TD 4S 6D\n" +
": QC 2S JC 9H QS KC 4H 8D\n" +
": 5D KD TH 5C 3H 8H 7C\n" +
": 2D JS KH TC 3S JD\n" +
": 7H 5S 6S TS 9D\n" +
": AH 6C 7D 2H\n" +
": 7S 9C QD\n" +
": 2C 3D\n" +
": AS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 4 to stack 7\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS\n" +
": 5H 9S 6H AC 4D TD 4S 6D\n" +
": QC 2S JC 9H QS KC 4H 8D\n" +
": 5D KD TH 5C 3H 8H 7C\n" +
": 2D JS KH TC 3S\n" +
": 7H 5S 6S TS 9D\n" +
": AH 6C 7D 2H\n" +
": 7S 9C QD JD\n" +
": 2C 3D\n" +
": AS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 2 to stack 5\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS\n" +
": 5H 9S 6H AC 4D TD 4S 6D\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 8H 7C\n" +
": 2D JS KH TC 3S\n" +
": 7H 5S 6S TS 9D 8D\n" +
": AH 6C 7D 2H\n" +
": 7S 9C QD JD\n" +
": 2C 3D\n" +
": AS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 9 to stack 6\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS\n" +
": 5H 9S 6H AC 4D TD 4S 6D\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 8H 7C\n" +
": 2D JS KH TC 3S\n" +
": 7H 5S 6S TS 9D 8D\n" +
": AH 6C 7D 2H AS\n" +
": 7S 9C QD JD\n" +
": 2C 3D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 1 to stack 3\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS\n" +
": 5H 9S 6H AC 4D TD 4S\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 8H 7C 6D\n" +
": 2D JS KH TC 3S\n" +
": 7H 5S 6S TS 9D 8D\n" +
": AH 6C 7D 2H AS\n" +
": 7S 9C QD JD\n" +
": 2C 3D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 4 to stack 1\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS\n" +
": 5H 9S 6H AC 4D TD 4S 3S\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 8H 7C 6D\n" +
": 2D JS KH TC\n" +
": 7H 5S 6S TS 9D 8D\n" +
": AH 6C 7D 2H AS\n" +
": 7S 9C QD JD\n" +
": 2C 3D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 3 cards from stack 5 to stack 7\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS\n" +
": 5H 9S 6H AC 4D TD 4S 3S\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 8H 7C 6D\n" +
": 2D JS KH TC\n" +
": 7H 5S 6S\n" +
": AH 6C 7D 2H AS\n" +
": 7S 9C QD JD TS 9D 8D\n" +
": 2C 3D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 5 to stack 9\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS\n" +
": 5H 9S 6H AC 4D TD 4S 3S\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 8H 7C 6D\n" +
": 2D JS KH TC\n" +
": 7H 5S\n" +
": AH 6C 7D 2H AS\n" +
": 7S 9C QD JD TS 9D 8D\n" +
": 2C 3D\n" +
": 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 1 to stack 5\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS\n" +
": 5H 9S 6H AC 4D TD\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 8H 7C 6D\n" +
": 2D JS KH TC\n" +
": 7H 5S 4S 3S\n" +
": AH 6C 7D 2H AS\n" +
": 7S 9C QD JD TS 9D 8D\n" +
": 2C 3D\n" +
": 6S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 3 cards from stack 5 to stack 9\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS\n" +
": 5H 9S 6H AC 4D TD\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 8H 7C 6D\n" +
": 2D JS KH TC\n" +
": 7H\n" +
": AH 6C 7D 2H AS\n" +
": 7S 9C QD JD TS 9D 8D\n" +
": 2C 3D\n" +
": 6S 5S 4S 3S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 7 to stack 1\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS\n" +
": 5H 9S 6H AC 4D TD 9D 8D\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 8H 7C 6D\n" +
": 2D JS KH TC\n" +
": 7H\n" +
": AH 6C 7D 2H AS\n" +
": 7S 9C QD JD TS\n" +
": 2C 3D\n" +
": 6S 5S 4S 3S\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 4 cards from stack 9 to stack 5\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS\n" +
": 5H 9S 6H AC 4D TD 9D 8D\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 8H 7C 6D\n" +
": 2D JS KH TC\n" +
": 7H 6S 5S 4S 3S\n" +
": AH 6C 7D 2H AS\n" +
": 7S 9C QD JD TS\n" +
": 2C 3D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 5 cards from stack 5 to stack 1\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS\n" +
": 5H 9S 6H AC 4D TD 9D 8D 7H 6S 5S 4S 3S\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 8H 7C 6D\n" +
": 2D JS KH TC\n" +
":\n" +
": AH 6C 7D 2H AS\n" +
": 7S 9C QD JD TS\n" +
": 2C 3D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 3 cards from stack 7 to stack 0\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D TD 9D 8D 7H 6S 5S 4S 3S\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 8H 7C 6D\n" +
": 2D JS KH TC\n" +
":\n" +
": AH 6C 7D 2H AS\n" +
": 7S 9C\n" +
": 2C 3D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 7 to stack 4\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D TD 9D 8D 7H 6S 5S 4S 3S\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 8H 7C 6D\n" +
": 2D JS KH TC 9C\n" +
":\n" +
": AH 6C 7D 2H AS\n" +
": 7S\n" +
": 2C 3D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 4 cards from stack 1 to stack 7\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D TD 9D 8D 7H\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 8H 7C 6D\n" +
": 2D JS KH TC 9C\n" +
":\n" +
": AH 6C 7D 2H AS\n" +
": 7S 6S 5S 4S 3S\n" +
": 2C 3D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 3 cards from stack 3 to stack 4\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D TD 9D 8D 7H\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H\n" +
": 2D JS KH TC 9C 8H 7C 6D\n" +
":\n" +
": AH 6C 7D 2H AS\n" +
": 7S 6S 5S 4S 3S\n" +
": 2C 3D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 6 to stack 3\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D TD 9D 8D 7H\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 2H AS\n" +
": 2D JS KH TC 9C 8H 7C 6D\n" +
":\n" +
": AH 6C 7D\n" +
": 7S 6S 5S 4S 3S\n" +
": 2C 3D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 4 to stack 6\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D TD 9D 8D 7H\n" +
": QC 2S JC 9H QS KC 4H\n" +
": 5D KD TH 5C 3H 2H AS\n" +
": 2D JS KH TC 9C 8H 7C\n" +
":\n" +
": AH 6C 7D 6D\n" +
": 7S 6S 5S 4S 3S\n" +
": 2C 3D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 3 cards from stack 3 to stack 2\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D TD 9D 8D 7H\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AS\n" +
": 5D KD TH 5C\n" +
": 2D JS KH TC 9C 8H 7C\n" +
":\n" +
": AH 6C 7D 6D\n" +
": 7S 6S 5S 4S 3S\n" +
": 2C 3D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 1 to stack 9\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D TD 9D 8D\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AS\n" +
": 5D KD TH 5C\n" +
": 2D JS KH TC 9C 8H 7C\n" +
":\n" +
": AH 6C 7D 6D\n" +
": 7S 6S 5S 4S 3S\n" +
": 2C 3D\n" +
": 7H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 6 to stack 1\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D TD 9D 8D 7D 6D\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AS\n" +
": 5D KD TH 5C\n" +
": 2D JS KH TC 9C 8H 7C\n" +
":\n" +
": AH 6C\n" +
": 7S 6S 5S 4S 3S\n" +
": 2C 3D\n" +
": 7H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 4\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D TD 9D 8D 7D 6D\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AS\n" +
": 5D KD TH 5C\n" +
": 2D JS KH TC 9C 8H 7C 6C\n" +
":\n" +
": AH\n" +
": 7S 6S 5S 4S 3S\n" +
": 2C 3D\n" +
": 7H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 3 to stack 4\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D TD 9D 8D 7D 6D\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AS\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7C 6C 5C\n" +
":\n" +
": AH\n" +
": 7S 6S 5S 4S 3S\n" +
": 2C 3D\n" +
": 7H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 2 to stack 5\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D TD 9D 8D 7D 6D\n" +
": QC 2S JC 9H QS KC 4H 3H 2H\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7C 6C 5C\n" +
": AS\n" +
": AH\n" +
": 7S 6S 5S 4S 3S\n" +
": 2C 3D\n" +
": 7H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 2\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D TD 9D 8D 7D 6D\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7C 6C 5C\n" +
": AS\n" +
":\n" +
": 7S 6S 5S 4S 3S\n" +
": 2C 3D\n" +
": 7H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 5 cards from stack 1 to stack 6\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7C 6C 5C\n" +
": AS\n" +
": TD 9D 8D 7D 6D\n" +
": 7S 6S 5S 4S 3S\n" +
": 2C 3D\n" +
": 7H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 8 to stack 1\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D 3D\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7C 6C 5C\n" +
": AS\n" +
": TD 9D 8D 7D 6D\n" +
": 7S 6S 5S 4S 3S\n" +
": 2C\n" +
": 7H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 5 to stack 8\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D 3D\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7C 6C 5C\n" +
":\n" +
": TD 9D 8D 7D 6D\n" +
": 7S 6S 5S 4S 3S\n" +
": 2C AS\n" +
": 7H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 8 to stack 1\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D 3D 2C AS\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7C 6C 5C\n" +
":\n" +
": TD 9D 8D 7D 6D\n" +
": 7S 6S 5S 4S 3S\n" +
":\n" +
": 7H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 3 cards from stack 4 to stack 5\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D 3D 2C AS\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H\n" +
": 7C 6C 5C\n" +
": TD 9D 8D 7D 6D\n" +
": 7S 6S 5S 4S 3S\n" +
":\n" +
": 7H\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 9 to stack 4\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TS\n" +
": 5H 9S 6H AC 4D 3D 2C AS\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7H\n" +
": 7C 6C 5C\n" +
": TD 9D 8D 7D 6D\n" +
": 7S 6S 5S 4S 3S\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 0 to stack 8\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD\n" +
": 5H 9S 6H AC 4D 3D 2C AS\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7H\n" +
": 7C 6C 5C\n" +
": TD 9D 8D 7D 6D\n" +
": 7S 6S 5S 4S 3S\n" +
": TS\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 5 cards from stack 6 to stack 0\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S KS QD JD TD 9D 8D 7D 6D\n" +
": 5H 9S 6H AC 4D 3D 2C AS\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7H\n" +
": 7C 6C 5C\n" +
":\n" +
": 7S 6S 5S 4S 3S\n" +
": TS\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 8 cards from stack 0 to stack 9\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S\n" +
": 5H 9S 6H AC 4D 3D 2C AS\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7H\n" +
": 7C 6C 5C\n" +
":\n" +
": 7S 6S 5S 4S 3S\n" +
": TS\n" +
": KS QD JD TD 9D 8D 7D 6D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 5 cards from stack 7 to stack 0\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S 7S 6S 5S 4S 3S\n" +
": 5H 9S 6H AC 4D 3D 2C AS\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7H\n" +
": 7C 6C 5C\n" +
":\n" +
":\n" +
": TS\n" +
": KS QD JD TD 9D 8D 7D 6D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 4 cards from stack 1 to stack 5\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S 7S 6S 5S 4S 3S\n" +
": 5H 9S 6H AC\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7H\n" +
": 7C 6C 5C 4D 3D 2C AS\n" +
":\n" +
":\n" +
": TS\n" +
": KS QD JD TD 9D 8D 7D 6D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 1 to stack 6\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S 7S 6S 5S 4S 3S\n" +
": 5H 9S 6H\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7H\n" +
": 7C 6C 5C 4D 3D 2C AS\n" +
": AC\n" +
":\n" +
": TS\n" +
": KS QD JD TD 9D 8D 7D 6D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 1 to stack 4\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH 8S 7S 6S 5S 4S 3S\n" +
": 5H 9S\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7H 6H\n" +
": 7C 6C 5C 4D 3D 2C AS\n" +
": AC\n" +
":\n" +
": TS\n" +
": KS QD JD TD 9D 8D 7D 6D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 6 cards from stack 0 to stack 1\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH\n" +
": 5H 9S 8S 7S 6S 5S 4S 3S\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7H 6H\n" +
": 7C 6C 5C 4D 3D 2C AS\n" +
": AC\n" +
":\n" +
": TS\n" +
": KS QD JD TD 9D 8D 7D 6D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 7 cards from stack 1 to stack 8\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH\n" +
": 5H\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7H 6H\n" +
": 7C 6C 5C 4D 3D 2C AS\n" +
": AC\n" +
":\n" +
": TS 9S 8S 7S 6S 5S 4S 3S\n" +
": KS QD JD TD 9D 8D 7D 6D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 1 to stack 4\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH\n" +
":\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD TH\n" +
": 2D JS KH TC 9C 8H 7H 6H 5H\n" +
": 7C 6C 5C 4D 3D 2C AS\n" +
": AC\n" +
":\n" +
": TS 9S 8S 7S 6S 5S 4S 3S\n" +
": KS QD JD TD 9D 8D 7D 6D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 3 to stack 0\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH\n" +
":\n" +
": QC 2S JC 9H QS KC 4H 3H 2H AH\n" +
": 5D KD\n" +
": 2D JS KH TC 9C 8H 7H 6H 5H\n" +
": 7C 6C 5C 4D 3D 2C AS\n" +
": AC\n" +
":\n" +
": TS 9S 8S 7S 6S 5S 4S 3S\n" +
": KS QD JD TD 9D 8D 7D 6D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 4 cards from stack 2 to stack 4\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH\n" +
":\n" +
": QC 2S JC 9H QS KC\n" +
": 5D KD\n" +
": 2D JS KH TC 9C 8H 7H 6H 5H 4H 3H 2H AH\n" +
": 7C 6C 5C 4D 3D 2C AS\n" +
": AC\n" +
":\n" +
": TS 9S 8S 7S 6S 5S 4S 3S\n" +
": KS QD JD TD 9D 8D 7D 6D\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 7 cards from stack 9 to stack 3\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH\n" +
":\n" +
": QC 2S JC 9H QS KC\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH TC 9C 8H 7H 6H 5H 4H 3H 2H AH\n" +
": 7C 6C 5C 4D 3D 2C AS\n" +
": AC\n" +
":\n" +
": TS 9S 8S 7S 6S 5S 4S 3S\n" +
": KS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 5 to stack 7\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH\n" +
":\n" +
": QC 2S JC 9H QS KC\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH TC 9C 8H 7H 6H 5H 4H 3H 2H AH\n" +
": 7C 6C 5C 4D 3D 2C\n" +
": AC\n" +
": AS\n" +
": TS 9S 8S 7S 6S 5S 4S 3S\n" +
": KS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 5\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH\n" +
":\n" +
": QC 2S JC 9H QS KC\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH TC 9C 8H 7H 6H 5H 4H 3H 2H AH\n" +
": 7C 6C 5C 4D 3D 2C AC\n" +
":\n" +
": AS\n" +
": TS 9S 8S 7S 6S 5S 4S 3S\n" +
": KS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 2 to stack 1\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH\n" +
": KC\n" +
": QC 2S JC 9H QS\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH TC 9C 8H 7H 6H 5H 4H 3H 2H AH\n" +
": 7C 6C 5C 4D 3D 2C AC\n" +
":\n" +
": AS\n" +
": TS 9S 8S 7S 6S 5S 4S 3S\n" +
": KS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 2 to stack 9\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH\n" +
": KC\n" +
": QC 2S JC 9H\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH TC 9C 8H 7H 6H 5H 4H 3H 2H AH\n" +
": 7C 6C 5C 4D 3D 2C AC\n" +
":\n" +
": AS\n" +
": TS 9S 8S 7S 6S 5S 4S 3S\n" +
": KS QS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 8 cards from stack 4 to stack 2\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH\n" +
": KC\n" +
": QC 2S JC 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH TC 9C\n" +
": 7C 6C 5C 4D 3D 2C AC\n" +
":\n" +
": AS\n" +
": TS 9S 8S 7S 6S 5S 4S 3S\n" +
": KS QS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 9 cards from stack 2 to stack 0\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
": KC\n" +
": QC 2S JC\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH TC 9C\n" +
": 7C 6C 5C 4D 3D 2C AC\n" +
":\n" +
": AS\n" +
": TS 9S 8S 7S 6S 5S 4S 3S\n" +
": KS QS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 4 to stack 2\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
": KC\n" +
": QC 2S JC TC 9C\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH\n" +
": 7C 6C 5C 4D 3D 2C AC\n" +
":\n" +
": AS\n" +
": TS 9S 8S 7S 6S 5S 4S 3S\n" +
": KS QS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 3 cards from stack 2 to stack 9\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
": KC\n" +
": QC 2S\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH\n" +
": 7C 6C 5C 4D 3D 2C AC\n" +
":\n" +
": AS\n" +
": TS 9S 8S 7S 6S 5S 4S 3S\n" +
": KS QS JC TC 9C\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 7 to stack 2\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
": KC\n" +
": QC 2S AS\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH\n" +
": 7C 6C 5C 4D 3D 2C AC\n" +
":\n" +
":\n" +
": TS 9S 8S 7S 6S 5S 4S 3S\n" +
": KS QS JC TC 9C\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 2 to stack 8\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
": KC\n" +
": QC\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH\n" +
": 7C 6C 5C 4D 3D 2C AC\n" +
":\n" +
":\n" +
": TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
": KS QS JC TC 9C\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 2 to stack 1\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
": KC QC\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH\n" +
": 7C 6C 5C 4D 3D 2C AC\n" +
":\n" +
":\n" +
": TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
": KS QS JC TC 9C\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 3 cards from stack 9 to stack 1\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
": KC QC JC TC 9C\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH\n" +
": 7C 6C 5C 4D 3D 2C AC\n" +
":\n" +
":\n" +
": TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
": KS QS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 11 cards from stack 0 to stack 9\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C AD\n" +
": KC QC JC TC 9C\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH\n" +
": 7C 6C 5C 4D 3D 2C AC\n" +
":\n" +
":\n" +
": TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
": KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 0 to stack 6\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 8C\n" +
": KC QC JC TC 9C\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH\n" +
": 7C 6C 5C 4D 3D 2C AC\n" +
": AD\n" +
":\n" +
": TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
": KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 0 to stack 1\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C\n" +
": KC QC JC TC 9C 8C\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH\n" +
": 7C 6C 5C 4D 3D 2C AC\n" +
": AD\n" +
":\n" +
": TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
": KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 5 to stack 0\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 2C AC\n" +
": KC QC JC TC 9C 8C\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH\n" +
": 7C 6C 5C 4D 3D\n" +
": AD\n" +
":\n" +
": TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
": KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 5 cards from stack 5 to stack 1\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 2C AC\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4D 3D\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS KH\n" +
":\n" +
": AD\n" +
":\n" +
": TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
": KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 4 to stack 7\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 2C AC\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4D 3D\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D JS\n" +
":\n" +
": AD\n" +
": KH\n" +
": TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
": KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 4 to stack 2\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 2C AC\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4D 3D\n" +
": JS\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D\n" +
":\n" +
": AD\n" +
": KH\n" +
": TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
": KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 4\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 2C AC\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4D 3D\n" +
": JS\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 2D AD\n" +
":\n" +
":\n" +
": KH\n" +
": TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
": KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 4 to stack 1\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 2C AC\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD\n" +
": JS\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
":\n" +
":\n" +
":\n" +
": KH\n" +
": TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
": KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 10 cards from stack 8 to stack 2\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 2C AC\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD\n" +
": JS TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
":\n" +
":\n" +
":\n" +
": KH\n" +
":\n" +
": KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 11 cards from stack 9 to stack 5\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 2C AC\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD\n" +
": JS TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
":\n" +
": JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
":\n" +
": KH\n" +
":\n" +
": KS QS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 11 cards from stack 2 to stack 9\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-0\n" +
"Freecells:\n" +
": 4C QH 3C 2C AC\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
":\n" +
": JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
":\n" +
": KH\n" +
":\n" +
": KS QS JS TS 9S 8S 7S 6S 5S 4S 3S 2S AS\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move the sequence on top of Stack 9 to the foundations\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-K\n" +
"Freecells:\n" +
": 4C QH 3C 2C AC\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
":\n" +
": JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
":\n" +
": KH\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 3 cards from stack 0 to stack 6\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-K\n" +
"Freecells:\n" +
": 4C QH\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
":\n" +
": JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
": 3C 2C AC\n" +
": KH\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 11 cards from stack 5 to stack 0\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-K\n" +
"Freecells:\n" +
": 4C QH JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
":\n" +
":\n" +
": 3C 2C AC\n" +
": KH\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 12 cards from stack 0 to stack 7\n" +
"\n" +
"Foundations: H-0 C-0 D-0 S-K\n" +
"Freecells:\n" +
": 4C\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
":\n" +
":\n" +
": 3C 2C AC\n" +
": KH QH JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move the sequence on top of Stack 7 to the foundations\n" +
"\n" +
"Foundations: H-K C-0 D-0 S-K\n" +
"Freecells:\n" +
": 4C\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
":\n" +
":\n" +
": 3C 2C AC\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 3 cards from stack 6 to stack 0\n" +
"\n" +
"Foundations: H-K C-0 D-0 S-K\n" +
"Freecells:\n" +
": 4C 3C 2C AC\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 4 cards from stack 1 to stack 4\n" +
"\n" +
"Foundations: H-K C-0 D-0 S-K\n" +
"Freecells:\n" +
": 4C 3C 2C AC\n" +
": KC QC JC TC 9C 8C 7C 6C 5C\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 4D 3D 2D AD\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 4 cards from stack 0 to stack 1\n" +
"\n" +
"Foundations: H-K C-0 D-0 S-K\n" +
"Freecells:\n" +
":\n" +
": KC QC JC TC 9C 8C 7C 6C 5C 4C 3C 2C AC\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 4D 3D 2D AD\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move the sequence on top of Stack 1 to the foundations\n" +
"\n" +
"Foundations: H-K C-K D-0 S-K\n" +
"Freecells:\n" +
":\n" +
":\n" +
":\n" +
": 5D KD QD JD TD 9D 8D 7D 6D\n" +
": 4D 3D 2D AD\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 8 cards from stack 3 to stack 8\n" +
"\n" +
"Foundations: H-K C-K D-0 S-K\n" +
"Freecells:\n" +
":\n" +
":\n" +
":\n" +
": 5D\n" +
": 4D 3D 2D AD\n" +
":\n" +
":\n" +
":\n" +
": KD QD JD TD 9D 8D 7D 6D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 4 cards from stack 4 to stack 3\n" +
"\n" +
"Foundations: H-K C-K D-0 S-K\n" +
"Freecells:\n" +
":\n" +
":\n" +
":\n" +
": 5D 4D 3D 2D AD\n" +
":\n" +
":\n" +
":\n" +
":\n" +
": KD QD JD TD 9D 8D 7D 6D\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 5 cards from stack 3 to stack 8\n" +
"\n" +
"Foundations: H-K C-K D-0 S-K\n" +
"Freecells:\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
": KD QD JD TD 9D 8D 7D 6D 5D 4D 3D 2D AD\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move the sequence on top of Stack 8 to the foundations\n" +
"\n" +
"Foundations: H-K C-K D-K S-K\n" +
"Freecells:\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n\n" +
"";

var board_without_trailing_newline__proto =
("Freecells:  2S  KC  -  -\n" +
"Foundations: H-A C-A D-0 S-0\n" +
"8H 7C JH 2C 2H 4C\n" +
"2D 9S 5S 4S 9C 8D KS\n" +
"TC 9H 6C 8S KH 7H 4H\n" +
"JS 3C TD 5C 6S 6H TS\n" +
"JD AS QH 5H 3H KD\n" +
"9D 7S AD 8C 3S JC\n" +
"QC QD 6D 4D 3D\n" +
"TH 7D QS 5D");

var board_without_trailing_newline = String(board_without_trailing_newline__proto).replace(/\n+$/, '');

var solution_for_board_without_trailing_newline =
(
"-=-=-=-=-=-=-=-=-=-=-=-\n" +
"\n" +
"Foundations: ♥-A ♣-A ♦-0 ♠-0\n" +
"Freecells:  2♠  K♣\n" +
": 8♥ 7♣ J♥ 2♣ 2♥ 4♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦ A♠ Q♥ 5♥ 3♥ K♦\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to freecell 2\n" +
"\n" +
"Foundations: ♥-A ♣-A ♦-0 ♠-0\n" +
"Freecells:  2♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ 2♣ 2♥ 4♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦ A♠ Q♥ 5♥ 3♥\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 4\n" +
"\n" +
"Foundations: ♥-A ♣-A ♦-0 ♠-0\n" +
"Freecells:      K♣  K♦\n" +
": 8♥ 7♣ J♥ 2♣ 2♥ 4♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦ A♠ Q♥ 5♥ 3♥ 2♠\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to freecell 3\n" +
"\n" +
"Foundations: ♥-A ♣-A ♦-0 ♠-0\n" +
"Freecells:      K♣  K♦  4♣\n" +
": 8♥ 7♣ J♥ 2♣ 2♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦ A♠ Q♥ 5♥ 3♥ 2♠\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: ♥-2 ♣-A ♦-0 ♠-0\n" +
"Freecells:      K♣  K♦  4♣\n" +
": 8♥ 7♣ J♥ 2♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦ A♠ Q♥ 5♥ 3♥ 2♠\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: ♥-2 ♣-2 ♦-0 ♠-0\n" +
"Freecells:      K♣  K♦  4♣\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦ A♠ Q♥ 5♥ 3♥ 2♠\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 7\n" +
"\n" +
"Foundations: ♥-2 ♣-2 ♦-0 ♠-0\n" +
"Freecells:      K♣  K♦\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦ A♠ Q♥ 5♥ 3♥ 2♠\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to freecell 0\n" +
"\n" +
"Foundations: ♥-2 ♣-2 ♦-0 ♠-0\n" +
"Freecells:  2♠  K♣  K♦\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦ A♠ Q♥ 5♥ 3♥\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-3 ♣-2 ♦-0 ♠-0\n" +
"Freecells:  2♠  K♣  K♦\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦ A♠ Q♥ 5♥\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: ♥-4 ♣-2 ♦-0 ♠-0\n" +
"Freecells:  2♠  K♣  K♦\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦ A♠ Q♥ 5♥\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-5 ♣-2 ♦-0 ♠-0\n" +
"Freecells:  2♠  K♣  K♦\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦ A♠ Q♥\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 6\n" +
"\n" +
"Foundations: ♥-5 ♣-2 ♦-0 ♠-0\n" +
"Freecells:      K♣  K♦\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦ A♠ Q♥\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦ 2♠\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to freecell 3\n" +
"\n" +
"Foundations: ♥-5 ♣-2 ♦-0 ♠-0\n" +
"Freecells:      K♣  K♦  Q♥\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦ A♠\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦ 2♠\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-5 ♣-2 ♦-0 ♠-A\n" +
"Freecells:      K♣  K♦  Q♥\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦ 2♠\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: ♥-5 ♣-2 ♦-0 ♠-2\n" +
"Freecells:      K♣  K♦  Q♥\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 1\n" +
"\n" +
"Foundations: ♥-5 ♣-2 ♦-0 ♠-2\n" +
"Freecells:      K♣  K♦\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦\n" +
": 9♦ 7♠ A♦ 8♣ 3♠ J♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to freecell 0\n" +
"\n" +
"Foundations: ♥-5 ♣-2 ♦-0 ♠-2\n" +
"Freecells:  J♣  K♣  K♦\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦\n" +
": 9♦ 7♠ A♦ 8♣ 3♠\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to the foundations\n" +
"\n" +
"Foundations: ♥-5 ♣-2 ♦-0 ♠-3\n" +
"Freecells:  J♣  K♣  K♦\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦\n" +
": 9♦ 7♠ A♦ 8♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 1\n" +
"\n" +
"Foundations: ♥-5 ♣-2 ♦-0 ♠-3\n" +
"Freecells:      K♣  K♦\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦\n" +
": 9♦ 7♠ A♦ 8♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to freecell 3\n" +
"\n" +
"Foundations: ♥-5 ♣-2 ♦-0 ♠-3\n" +
"Freecells:      K♣  K♦  8♣\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦\n" +
": 9♦ 7♠ A♦\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to the foundations\n" +
"\n" +
"Foundations: ♥-5 ♣-2 ♦-A ♠-3\n" +
"Freecells:      K♣  K♦  8♣\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦\n" +
": 9♦ 7♠\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to freecell 0\n" +
"\n" +
"Foundations: ♥-5 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦  8♣\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦\n" +
": 9♦\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 5\n" +
"\n" +
"Foundations: ♥-5 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠\n" +
": J♦\n" +
": 9♦ 8♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to freecell 3\n" +
"\n" +
"Foundations: ♥-5 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦  T♠\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 6♥\n" +
": J♦\n" +
": 9♦ 8♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: ♥-6 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦  T♠\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥ 7♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠\n" +
": J♦\n" +
": 9♦ 8♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦  T♠\n" +
": 8♥ 7♣ J♥\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠\n" +
": J♦\n" +
": 9♦ 8♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 0\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠\n" +
": J♦\n" +
": 9♦ 8♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 5 to stack 0\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠\n" +
": J♦\n" +
":\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 7 to stack 5\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠\n" +
": J♦\n" +
": 5♦ 4♣\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 5 to stack 3\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": J♦\n" +
":\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦ Q♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to freecell 3\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦  Q♠\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": J♦\n" +
":\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥ 7♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 7 to stack 5\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦  Q♠\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": J♦\n" +
": 7♦\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 2\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥ Q♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": J♦\n" +
": 7♦\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 5 to stack 0\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠ K♥ Q♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": J♦\n" +
":\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 2 to stack 5\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣\n" +
": T♣ 9♥ 6♣ 8♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": J♦\n" +
": K♥ Q♠\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": T♥\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 7 to stack 1\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥\n" +
": T♣ 9♥ 6♣ 8♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": J♦\n" +
": K♥ Q♠\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to freecell 3\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦  8♠\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥\n" +
": T♣ 9♥ 6♣\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": J♦\n" +
": K♥ Q♠\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 2 to stack 7\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦  8♠\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥\n" +
": T♣ 9♥\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": J♦\n" +
": K♥ Q♠\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": 6♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 2\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥\n" +
": T♣ 9♥ 8♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": J♦\n" +
": K♥ Q♠\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
": 6♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 7 to stack 0\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥\n" +
": T♣ 9♥ 8♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": J♦\n" +
": K♥ Q♠\n" +
": Q♣ Q♦ 6♦ 4♦ 3♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to freecell 3\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦  3♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥\n" +
": T♣ 9♥ 8♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": J♦\n" +
": K♥ Q♠\n" +
": Q♣ Q♦ 6♦ 4♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 6 to stack 7\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦  3♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥\n" +
": T♣ 9♥ 8♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": J♦\n" +
": K♥ Q♠\n" +
": Q♣ Q♦ 6♦\n" +
": 4♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to stack 3\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥\n" +
": T♣ 9♥ 8♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦\n" +
": J♦\n" +
": K♥ Q♠\n" +
": Q♣ Q♦ 6♦\n" +
": 4♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 4 to stack 5\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥\n" +
": T♣ 9♥ 8♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦\n" +
":\n" +
": K♥ Q♠ J♦\n" +
": Q♣ Q♦ 6♦\n" +
": 4♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 3 cards from stack 2 to stack 5\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥\n" +
":\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦\n" +
":\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 4♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 4 cards from stack 1 to stack 4\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 2♦ 9♠ 5♠ 4♠ 9♣ 8♦\n" +
":\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦\n" +
": K♠ Q♥ J♣ T♥\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 4♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 1 to stack 2\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-3\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 2♦ 9♠ 5♠ 4♠\n" +
": 9♣ 8♦\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦\n" +
": K♠ Q♥ J♣ T♥\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 4♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-4\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 2♦ 9♠ 5♠\n" +
": 9♣ 8♦\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦\n" +
": K♠ Q♥ J♣ T♥\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 4♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-5\n" +
"Freecells:  7♠  K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 2♦ 9♠\n" +
": 9♣ 8♦\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦\n" +
": K♠ Q♥ J♣ T♥\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 4♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to stack 2\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-5\n" +
"Freecells:      K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 2♦ 9♠\n" +
": 9♣ 8♦ 7♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦\n" +
": K♠ Q♥ J♣ T♥\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 4♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 1 to stack 4\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-A ♠-5\n" +
"Freecells:      K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 2♦\n" +
": 9♣ 8♦ 7♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦\n" +
": K♠ Q♥ J♣ T♥ 9♠\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 4♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-2 ♠-5\n" +
"Freecells:      K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
":\n" +
": 9♣ 8♦ 7♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦\n" +
": K♠ Q♥ J♣ T♥ 9♠\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 4♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-3 ♠-5\n" +
"Freecells:      K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
":\n" +
": 9♣ 8♦ 7♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": K♠ Q♥ J♣ T♥ 9♠\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 4♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-4 ♠-5\n" +
"Freecells:      K♣  K♦\n" +
": 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
":\n" +
": 9♣ 8♦ 7♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": K♠ Q♥ J♣ T♥ 9♠\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 6 cards from stack 0 to stack 1\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-4 ♠-5\n" +
"Freecells:      K♣  K♦\n" +
": 8♥ 7♣\n" +
": J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 9♣ 8♦ 7♠\n" +
": J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣\n" +
": K♠ Q♥ J♣ T♥ 9♠\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 3 cards from stack 3 to stack 7\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-4 ♠-5\n" +
"Freecells:      K♣  K♦\n" +
": 8♥ 7♣\n" +
": J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 9♣ 8♦ 7♠\n" +
": J♠ 3♣ T♦ 5♣\n" +
": K♠ Q♥ J♣ T♥ 9♠\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 6♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 2 cards from stack 0 to stack 4\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-4 ♠-5\n" +
"Freecells:      K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 9♣ 8♦ 7♠\n" +
": J♠ 3♣ T♦ 5♣\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 6♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to freecell 3\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-4 ♠-5\n" +
"Freecells:      K♣  K♦  5♣\n" +
":\n" +
": J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 9♣ 8♦ 7♠\n" +
": J♠ 3♣ T♦\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 6♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to freecell 0\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-4 ♠-5\n" +
"Freecells:  T♦  K♣  K♦  5♣\n" +
":\n" +
": J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 9♣ 8♦ 7♠\n" +
": J♠ 3♣\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 6♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move 1 cards from stack 3 to stack 0\n" +
"\n" +
"Foundations: ♥-7 ♣-2 ♦-4 ♠-5\n" +
"Freecells:  T♦  K♣  K♦  5♣\n" +
": 3♣\n" +
": J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 9♣ 8♦ 7♠\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 6♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 0 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-3 ♦-4 ♠-5\n" +
"Freecells:  T♦  K♣  K♦  5♣\n" +
":\n" +
": J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 9♣ 8♦ 7♠\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 6♠ 5♦ 4♣\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-4 ♦-4 ♠-5\n" +
"Freecells:  T♦  K♣  K♦  5♣\n" +
":\n" +
": J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 9♣ 8♦ 7♠\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 6♠ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 3 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-5 ♦-4 ♠-5\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 9♣ 8♦ 7♠\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 6♠ 5♦\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-5 ♦-5 ♠-5\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦ 8♣ 7♦ 6♣\n" +
": 9♣ 8♦ 7♠\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-6 ♦-5 ♠-5\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦ 8♣ 7♦\n" +
": 9♣ 8♦ 7♠\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦ 6♦\n" +
": 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-6 ♦-6 ♠-5\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦ 8♣ 7♦\n" +
": 9♣ 8♦ 7♠\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦\n" +
": 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-7 ♦-6 ♠-5\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦ 8♣ 7♦\n" +
": 9♣ 8♦ 7♠\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦\n" +
": 6♠\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 7 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-7 ♦-6 ♠-6\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦ 8♣ 7♦\n" +
": 9♣ 8♦ 7♠\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-7 ♦-6 ♠-7\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦ 8♣ 7♦\n" +
": 9♣ 8♦\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-7 ♦-7 ♠-7\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦ 8♣\n" +
": 9♣ 8♦\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥\n" +
": K♥ Q♠ J♦ T♣ 9♥ 8♠\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to the foundations\n" +
"\n" +
"Foundations: ♥-7 ♣-7 ♦-7 ♠-8\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦ 8♣\n" +
": 9♣ 8♦\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠ 8♥\n" +
": K♥ Q♠ J♦ T♣ 9♥\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-8 ♣-7 ♦-7 ♠-8\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦ 8♣\n" +
": 9♣ 8♦\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠\n" +
": K♥ Q♠ J♦ T♣ 9♥\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: ♥-8 ♣-7 ♦-8 ♠-8\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦ 8♣\n" +
": 9♣\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠\n" +
": K♥ Q♠ J♦ T♣ 9♥\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-8 ♣-8 ♦-8 ♠-8\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦\n" +
": 9♣\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠\n" +
": K♥ Q♠ J♦ T♣ 9♥\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to the foundations\n" +
"\n" +
"Foundations: ♥-9 ♣-8 ♦-8 ♠-8\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦\n" +
": 9♣\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥ 9♠\n" +
": K♥ Q♠ J♦ T♣\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-9 ♣-8 ♦-8 ♠-9\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦\n" +
": 9♣\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥\n" +
": K♥ Q♠ J♦ T♣\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 2 to the foundations\n" +
"\n" +
"Foundations: ♥-9 ♣-9 ♦-8 ♠-9\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠ 9♦\n" +
":\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥\n" +
": K♥ Q♠ J♦ T♣\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-9 ♣-9 ♦-9 ♠-9\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠\n" +
":\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥\n" +
": K♥ Q♠ J♦ T♣\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to the foundations\n" +
"\n" +
"Foundations: ♥-9 ♣-T ♦-9 ♠-9\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠\n" +
":\n" +
": J♠\n" +
": K♠ Q♥ J♣ T♥\n" +
": K♥ Q♠ J♦\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-T ♦-9 ♠-9\n" +
"Freecells:  T♦  K♣  K♦\n" +
":\n" +
": J♥ T♠\n" +
":\n" +
": J♠\n" +
": K♠ Q♥ J♣\n" +
": K♥ Q♠ J♦\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 0 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-T ♦-T ♠-9\n" +
"Freecells:      K♣  K♦\n" +
":\n" +
": J♥ T♠\n" +
":\n" +
": J♠\n" +
": K♠ Q♥ J♣\n" +
": K♥ Q♠ J♦\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-T ♦-T ♠-T\n" +
"Freecells:      K♣  K♦\n" +
":\n" +
": J♥\n" +
":\n" +
": J♠\n" +
": K♠ Q♥ J♣\n" +
": K♥ Q♠ J♦\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 3 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-T ♦-T ♠-J\n" +
"Freecells:      K♣  K♦\n" +
":\n" +
": J♥\n" +
":\n" +
":\n" +
": K♠ Q♥ J♣\n" +
": K♥ Q♠ J♦\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-T ♦-J ♠-J\n" +
"Freecells:      K♣  K♦\n" +
":\n" +
": J♥\n" +
":\n" +
":\n" +
": K♠ Q♥ J♣\n" +
": K♥ Q♠\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-T ♣-J ♦-J ♠-J\n" +
"Freecells:      K♣  K♦\n" +
":\n" +
": J♥\n" +
":\n" +
":\n" +
": K♠ Q♥\n" +
": K♥ Q♠\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 1 to the foundations\n" +
"\n" +
"Foundations: ♥-J ♣-J ♦-J ♠-J\n" +
"Freecells:      K♣  K♦\n" +
":\n" +
":\n" +
":\n" +
":\n" +
": K♠ Q♥\n" +
": K♥ Q♠\n" +
": Q♣ Q♦\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: ♥-J ♣-J ♦-Q ♠-J\n" +
"Freecells:      K♣  K♦\n" +
":\n" +
":\n" +
":\n" +
":\n" +
": K♠ Q♥\n" +
": K♥ Q♠\n" +
": Q♣\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to the foundations\n" +
"\n" +
"Foundations: ♥-J ♣-J ♦-Q ♠-Q\n" +
"Freecells:      K♣  K♦\n" +
":\n" +
":\n" +
":\n" +
":\n" +
": K♠ Q♥\n" +
": K♥\n" +
": Q♣\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-Q ♣-J ♦-Q ♠-Q\n" +
"Freecells:      K♣  K♦\n" +
":\n" +
":\n" +
":\n" +
":\n" +
": K♠\n" +
": K♥\n" +
": Q♣\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 2 to the foundations\n" +
"\n" +
"Foundations: ♥-Q ♣-J ♦-K ♠-Q\n" +
"Freecells:      K♣\n" +
":\n" +
":\n" +
":\n" +
":\n" +
": K♠\n" +
": K♥\n" +
": Q♣\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 6 to the foundations\n" +
"\n" +
"Foundations: ♥-Q ♣-Q ♦-K ♠-Q\n" +
"Freecells:      K♣\n" +
":\n" +
":\n" +
":\n" +
":\n" +
": K♠\n" +
": K♥\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 5 to the foundations\n" +
"\n" +
"Foundations: ♥-K ♣-Q ♦-K ♠-Q\n" +
"Freecells:      K♣\n" +
":\n" +
":\n" +
":\n" +
":\n" +
": K♠\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from stack 4 to the foundations\n" +
"\n" +
"Foundations: ♥-K ♣-Q ♦-K ♠-K\n" +
"Freecells:      K♣\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
"Move a card from freecell 1 to the foundations\n" +
"\n" +
"Foundations: ♥-K ♣-K ♦-K ♠-K\n" +
"Freecells:\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
":\n" +
"\n" +
"\n" +
"====================\n" +
"\n" +
""
);

function test_js_fc_solve_class()
{
    module("FC_Solve.Algorithmic");

    test("FC_Solve main test", function() {
        expect(3);

        // TEST
        ok (true, "True is, well, true.");

        var success = false;

        var instance = new FC_Solve({
            cmd_line_preset: 'default',
            set_status_callback: function () { return; },
            set_output: function (buffer) {
                success = true;
                // TEST
                equal (buffer, solution_for_deal_24__default,
                    "Solution is right"
                );
            },
        });

        var solve_err_code = instance.do_solve(ms_deal_24);

        while (solve_err_code == FCS_STATE_SUSPEND_PROCESS) {
            solve_err_code = instance.resume_solution();
        }

        // TEST
        ok (success, "do_solve was successful.");

    });

    test("FC_Solve unicoded solution", function() {
        expect(3);

        // TEST
        ok (true, "True is, well, true.");

        var success = false;

        var instance = new FC_Solve({
            is_unicode_cards: true,
            cmd_line_preset: 'default',
            set_status_callback: function () { return; },
            set_output: function (buffer) {
                success = true;

                // TEST
                equal (buffer,
                    solution_for_deal_24__default__with_unicoded_suits,
                    "Unicode Solution is right"
                );
            },
        });

        var solve_err_code = instance.do_solve(ms_deal_24);

        while (solve_err_code == FCS_STATE_SUSPEND_PROCESS) {
            solve_err_code = instance.resume_solution();
        }

        // TEST
        ok (success, "do_solve was successful.");

    });

    test("FC_Solve arbitrary parameters - Solve Simple Simon.", function() {
        expect(2);

        var success = false;

        var instance = new FC_Solve({
            cmd_line_preset: 'default',
            string_params: '-g simple_simon',
            dir_base: 'fcs1',
            set_status_callback: function () { return; },
            set_output: function (buffer) {
                success = true;
                // TEST
                equal (buffer, solution_for_pysol_simple_simon_deal_24__default,
                    "Simple Simon Solution is right"
                );
            },
        });

        var solve_err_code = instance.do_solve(pysol_simple_simon_deal_24);

        while (solve_err_code == FCS_STATE_SUSPEND_PROCESS) {
            solve_err_code = instance.resume_solution();
        }

        // TEST
        ok (success, "do_solve was successful.");

    });

    test("FC_Solve solve board without a trailing newline", function() {
        expect(3);

        // TEST
        ok (true, "True is, well, true.");

        var success = false;

        var instance = new FC_Solve({
            is_unicode_cards: true,
            cmd_line_preset: "as",
            set_status_callback: function () { return; },
            set_output: function (buffer) {
                success = true;

                // TEST
                equal (buffer,
                    solution_for_board_without_trailing_newline,
                    "Board without a trailing newline solution is right."
                );
            },
        });

        var solve_err_code = instance.do_solve(board_without_trailing_newline);

        while (solve_err_code == FCS_STATE_SUSPEND_PROCESS) {
            solve_err_code = instance.resume_solution();
        }

        // TEST
        ok (success, "do_solve for no trailing newline was successful.");

    });
    return;
}

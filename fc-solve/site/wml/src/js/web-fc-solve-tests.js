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

    return;
}

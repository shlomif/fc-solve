"use strict";
function mydef(x, y) {
    if (typeof define !== 'function') {
        return require('amdefine')(module)(x, y);
    } else {
        return define(x, y);
    }
}
mydef(["web-fc-solve", "libfcs-wrap",
    'dist/fc_solve_find_index_s2ints.js'], function(w, Module, s2i) {
    let FC_Solve = w.FC_Solve;
    const FC_Solve_init_wrappers_with_module =
            w.FC_Solve_init_wrappers_with_module;
    const FCS_STATE_SUSPEND_PROCESS = w.FCS_STATE_SUSPEND_PROCESS;
    const FCS_STATE_WAS_SOLVED = w.FCS_STATE_WAS_SOLVED;
    const ms_deal_24 = `4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD TH TC TD 8D
7H JS KH TS KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
`;
    const solution_for_deal_24__default = `-=-=-=-=-=-=-=-=-=-=-=-

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 3 to the foundations

Foundations: H-0 C-0 D-0 S-A
Freecells:
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 6 to freecell 0

Foundations: H-0 C-0 D-0 S-A
Freecells:  JD
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 6 to freecell 1

Foundations: H-0 C-0 D-0 S-A
Freecells:  JD  8H
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 6 to the foundations

Foundations: H-0 C-0 D-A S-A
Freecells:  JD  8H
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 0 to freecell 2

Foundations: H-0 C-0 D-A S-A
Freecells:  JD  8H  2H
: 4C 2C 9C 8C QS 4S
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 0 to freecell 3

Foundations: H-0 C-0 D-A S-A
Freecells:  JD  8H  2H  4S
: 4C 2C 9C 8C QS
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S
: 7S 6C 7D 4D 8S 9D


====================

Move a card from freecell 0 to stack 0

Foundations: H-0 C-0 D-A S-A
Freecells:      8H  2H  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 6 to freecell 0

Foundations: H-0 C-0 D-A S-A
Freecells:  6S  8H  2H  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 6 to stack 3

Foundations: H-0 C-0 D-A S-A
Freecells:  6S  8H  2H  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 6 to the foundations

Foundations: H-A C-0 D-A S-A
Freecells:  6S  8H  2H  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
:
: 7S 6C 7D 4D 8S 9D


====================

Move a card from freecell 2 to the foundations

Foundations: H-2 C-0 D-A S-A
Freecells:  6S  8H      4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
:
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 1 to freecell 2

Foundations: H-2 C-0 D-A S-A
Freecells:  6S  8H  QD  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC 3H 4H
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
:
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 1 to stack 6

Foundations: H-2 C-0 D-A S-A
Freecells:  6S  8H  QD  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC 3H
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: 4H
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 1 to the foundations

Foundations: H-3 C-0 D-A S-A
Freecells:  6S  8H  QD  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: 4H
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 6 to the foundations

Foundations: H-4 C-0 D-A S-A
Freecells:  6S  8H  QD  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
:
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 1 to the foundations

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H  QD  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
:
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 5 to stack 6

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H  QD  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC
: 7C
: 7S 6C 7D 4D 8S 9D


====================

Move a card from freecell 2 to stack 5

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H      4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC QD
: 7C
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 6 to stack 4

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H      4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
:
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 1 to freecell 2

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 5H QH
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
:
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 1 to stack 6

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 5H
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D


====================

Move a card from freecell 3 to stack 1

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H  3C
: 4C 2C 9C 8C QS JD
: 5H 4S
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 1 to freecell 3

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 5H
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 1 to the foundations

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
:
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D


====================

Move 2 cards from stack 3 to stack 4

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
:
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 2 to stack 1

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 6 to stack 2

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD
:
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 7 to stack 6

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD
: 9D
: 7S 6C 7D 4D 8S


====================

Move 1 cards from stack 7 to stack 6

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD
: 9D 8S
: 7S 6C 7D 4D


====================

Move a card from freecell 2 to stack 7

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H      4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD
: 9D 8S
: 7S 6C 7D 4D 3C


====================

Move 2 cards from stack 7 to stack 4

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H      4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD
: 9D 8S
: 7S 6C 7D


====================

Move a card from freecell 0 to stack 7

Foundations: H-5 C-A D-A S-A
Freecells:      8H      4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 3 to freecell 2

Foundations: H-5 C-A D-A S-A
Freecells:      8H  JH  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 3 to freecell 0

Foundations: H-5 C-A D-A S-A
Freecells:  5C  8H  JH  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD
: 9D 8S
: 7S 6C 7D 6S


====================

Move 1 cards from stack 3 to stack 5

Foundations: H-5 C-A D-A S-A
Freecells:  5C  8H  JH  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 3 to the foundations

Foundations: H-5 C-A D-A S-2
Freecells:  5C  8H  JH  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from freecell 3 to stack 3

Foundations: H-5 C-A D-A S-2
Freecells:  5C  8H  JH
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 4S
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move 1 cards from stack 1 to stack 3

Foundations: H-5 C-A D-A S-2
Freecells:  5C  8H  JH
: 4C 2C 9C 8C QS JD
:
: QC 9S 6H 9H 3S KS QH
: 5D 4S 3D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 2 to freecell 3

Foundations: H-5 C-A D-A S-2
Freecells:  5C  8H  JH  QH
: 4C 2C 9C 8C QS JD
:
: QC 9S 6H 9H 3S KS
: 5D 4S 3D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move 1 cards from stack 2 to stack 1

Foundations: H-5 C-A D-A S-2
Freecells:  5C  8H  JH  QH
: 4C 2C 9C 8C QS JD
: KS
: QC 9S 6H 9H 3S
: 5D 4S 3D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 2 to the foundations

Foundations: H-5 C-A D-A S-3
Freecells:  5C  8H  JH  QH
: 4C 2C 9C 8C QS JD
: KS
: QC 9S 6H 9H
: 5D 4S 3D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from freecell 3 to stack 1

Foundations: H-5 C-A D-A S-3
Freecells:  5C  8H  JH
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 6H 9H
: 5D 4S 3D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 2 to freecell 3

Foundations: H-5 C-A D-A S-3
Freecells:  5C  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 6H
: 5D 4S 3D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from freecell 0 to stack 2

Foundations: H-5 C-A D-A S-3
Freecells:      8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 6H 5C
: 5D 4S 3D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 3 to freecell 0

Foundations: H-5 C-A D-A S-3
Freecells:  3D  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 6H 5C
: 5D 4S
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 3 to the foundations

Foundations: H-5 C-A D-A S-4
Freecells:  3D  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 6H 5C
: 5D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move 1 cards from stack 3 to stack 7

Foundations: H-5 C-A D-A S-4
Freecells:  3D  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 6H 5C
:
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D


====================

Move 1 cards from stack 2 to stack 3

Foundations: H-5 C-A D-A S-4
Freecells:  3D  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 6H
: 5C
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D


====================

Move a card from stack 2 to the foundations

Foundations: H-6 C-A D-A S-4
Freecells:  3D  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S
: 5C
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D


====================

Move a card from freecell 1 to stack 2

Foundations: H-6 C-A D-A S-4
Freecells:  3D      JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D


====================

Move 2 cards from stack 4 to stack 3

Foundations: H-6 C-A D-A S-4
Freecells:  3D      JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D


====================

Move a card from stack 4 to the foundations

Foundations: H-6 C-A D-A S-5
Freecells:  3D      JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D


====================

Move a card from stack 7 to freecell 1

Foundations: H-6 C-A D-A S-5
Freecells:  3D  5D  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 7 to the foundations

Foundations: H-6 C-A D-A S-6
Freecells:  3D  5D  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D


====================

Move 1 cards from stack 7 to stack 6

Foundations: H-6 C-A D-A S-6
Freecells:  3D  5D  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D
: 7S 6C


====================

Move a card from freecell 1 to stack 7

Foundations: H-6 C-A D-A S-6
Freecells:  3D      JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D
: 7S 6C 5D


====================

Move 2 cards from stack 7 to stack 6

Foundations: H-6 C-A D-A S-6
Freecells:  3D      JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 7S


====================

Move a card from stack 7 to the foundations

Foundations: H-6 C-A D-A S-7
Freecells:  3D      JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
:


====================

Move a card from stack 2 to freecell 1

Foundations: H-6 C-A D-A S-7
Freecells:  3D  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
:


====================

Move 1 cards from stack 2 to stack 7

Foundations: H-6 C-A D-A S-7
Freecells:  3D  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 9S


====================

Move a card from freecell 2 to stack 2

Foundations: H-6 C-A D-A S-7
Freecells:  3D  8H      9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 9S


====================

Move a card from freecell 1 to stack 7

Foundations: H-6 C-A D-A S-7
Freecells:  3D          9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move 3 cards from stack 3 to stack 4

Foundations: H-6 C-A D-A S-7
Freecells:  3D          9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
:
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 5 to freecell 2

Foundations: H-6 C-A D-A S-7
Freecells:  3D      JC  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
:
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS KC QD
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 5 to freecell 1

Foundations: H-6 C-A D-A S-7
Freecells:  3D  QD  JC  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
:
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS KC
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move 1 cards from stack 5 to stack 3

Foundations: H-6 C-A D-A S-7
Freecells:  3D  QD  JC  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: KC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from freecell 3 to stack 5

Foundations: H-6 C-A D-A S-7
Freecells:  3D  QD  JC
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: KC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from freecell 1 to stack 3

Foundations: H-6 C-A D-A S-7
Freecells:  3D      JC
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from freecell 2 to stack 3

Foundations: H-6 C-A D-A S-7
Freecells:  3D
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 0 to freecell 3

Foundations: H-6 C-A D-A S-7
Freecells:  3D          JD
: 4C 2C 9C 8C QS
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 0 to freecell 1

Foundations: H-6 C-A D-A S-7
Freecells:  3D  QS      JD
: 4C 2C 9C 8C
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move 1 cards from stack 0 to stack 5

Foundations: H-6 C-A D-A S-7
Freecells:  3D  QS      JD
: 4C 2C 9C
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 0 to freecell 2

Foundations: H-6 C-A D-A S-7
Freecells:  3D  QS  9C  JD
: 4C 2C
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 0 to the foundations

Foundations: H-6 C-2 D-A S-7
Freecells:  3D  QS  9C  JD
: 4C
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 4 to the foundations

Foundations: H-6 C-3 D-A S-7
Freecells:  3D  QS  9C  JD
: 4C
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 0 to the foundations

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  QS  9C  JD
:
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from freecell 0 to stack 0

Foundations: H-6 C-4 D-A S-7
Freecells:      QS  9C  JD
: 3D
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move 1 cards from stack 3 to stack 1

Foundations: H-6 C-4 D-A S-7
Freecells:      QS  9C  JD
: 3D
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 0 to freecell 0

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  QS  9C  JD
:
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from freecell 2 to stack 0

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  QS      JD
: 9C
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move 1 cards from stack 7 to stack 0

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  QS      JD
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S


====================

Move 1 cards from stack 1 to stack 3

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  QS      JD
: 9C 8H
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S


====================

Move a card from stack 7 to freecell 2

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  QS  9S  JD
: 9C 8H
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
:


====================

Move a card from freecell 0 to stack 7

Foundations: H-6 C-4 D-A S-7
Freecells:      QS  9S  JD
: 9C 8H
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 3D


====================

Move 1 cards from stack 3 to stack 1

Foundations: H-6 C-4 D-A S-7
Freecells:      QS  9S  JD
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 3D


====================

Move a card from stack 7 to freecell 0

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  QS  9S  JD
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
:


====================

Move a card from freecell 1 to stack 7

Foundations: H-6 C-4 D-A S-7
Freecells:  3D      9S  JD
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: QS


====================

Move a card from freecell 3 to stack 7

Foundations: H-6 C-4 D-A S-7
Freecells:  3D      9S
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: QS JD


====================

Move a card from stack 4 to freecell 1

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  4D  9S
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: QS JD


====================

Move a card from stack 4 to the foundations

Foundations: H-6 C-5 D-A S-7
Freecells:  3D  4D  9S
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: QS JD


====================

Move a card from stack 6 to freecell 3

Foundations: H-6 C-5 D-A S-7
Freecells:  3D  4D  9S  5D
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C
: QS JD


====================

Move a card from stack 6 to the foundations

Foundations: H-6 C-6 D-A S-7
Freecells:  3D  4D  9S  5D
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS 9H 8C
: 9D 8S 7D
: QS JD


====================

Move 1 cards from stack 6 to stack 5

Foundations: H-6 C-6 D-A S-7
Freecells:  3D  4D  9S  5D
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS 9H 8C 7D
: 9D 8S
: QS JD


====================

Move a card from stack 6 to the foundations

Foundations: H-6 C-6 D-A S-8
Freecells:  3D  4D  9S  5D
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD


====================

Move a card from freecell 2 to the foundations

Foundations: H-6 C-6 D-A S-9
Freecells:  3D  4D      5D
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD


====================

Move 2 cards from stack 4 to stack 0

Foundations: H-6 C-6 D-A S-9
Freecells:  3D  4D      5D
: 9C 8H 7C 6D
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD


====================

Move a card from stack 4 to freecell 2

Foundations: H-6 C-6 D-A S-9
Freecells:  3D  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD


====================

Move 1 cards from stack 4 to stack 1

Foundations: H-6 C-6 D-A S-9
Freecells:  3D  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH
: KC QD
: 2D KD TH TC
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD


====================

Move 1 cards from stack 4 to stack 2

Foundations: H-6 C-6 D-A S-9
Freecells:  3D  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC
: KC QD
: 2D KD TH
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD


====================

Move 1 cards from stack 6 to stack 2

Foundations: H-6 C-6 D-A S-9
Freecells:  3D  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D
: KC QD
: 2D KD TH
: 7H JS KH TS 9H 8C 7D
:
: QS JD


====================

Move a card from freecell 0 to stack 6

Foundations: H-6 C-6 D-A S-9
Freecells:      4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D
: KC QD
: 2D KD TH
: 7H JS KH TS 9H 8C 7D
: 3D
: QS JD


====================

Move 2 cards from stack 5 to stack 2

Foundations: H-6 C-6 D-A S-9
Freecells:      4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
: 2D KD TH
: 7H JS KH TS 9H
: 3D
: QS JD


====================

Move 2 cards from stack 5 to stack 7

Foundations: H-6 C-6 D-A S-9
Freecells:      4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
: 2D KD TH
: 7H JS KH
: 3D
: QS JD TS 9H


====================

Move a card from stack 5 to freecell 0

Foundations: H-6 C-6 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
: 2D KD TH
: 7H JS
: 3D
: QS JD TS 9H


====================

Move 1 cards from stack 5 to stack 3

Foundations: H-6 C-6 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD TH
: 7H
: 3D
: QS JD TS 9H


====================

Move a card from stack 5 to the foundations

Foundations: H-7 C-6 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD TH
:
: 3D
: QS JD TS 9H


====================

Move 1 cards from stack 0 to stack 5

Foundations: H-7 C-6 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C 8H 7C
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD TH
: 6D
: 3D
: QS JD TS 9H


====================

Move a card from stack 0 to the foundations

Foundations: H-7 C-7 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C 8H
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD TH
: 6D
: 3D
: QS JD TS 9H


====================

Move a card from stack 0 to the foundations

Foundations: H-8 C-7 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD TH
: 6D
: 3D
: QS JD TS 9H


====================

Move a card from stack 7 to the foundations

Foundations: H-9 C-7 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD TH
: 6D
: 3D
: QS JD TS


====================

Move a card from stack 4 to the foundations

Foundations: H-T C-7 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD
: 6D
: 3D
: QS JD TS


====================

Move a card from stack 7 to the foundations

Foundations: H-T C-7 D-A S-T
Freecells:  KH  4D  8D  5D
: 9C
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD
: 6D
: 3D
: QS JD


====================

Move a card from stack 3 to the foundations

Foundations: H-T C-7 D-A S-J
Freecells:  KH  4D  8D  5D
: 9C
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
: 2D KD
: 6D
: 3D
: QS JD


====================

Move a card from freecell 2 to stack 0

Foundations: H-T C-7 D-A S-J
Freecells:  KH  4D      5D
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
: 2D KD
: 6D
: 3D
: QS JD


====================

Move a card from stack 4 to freecell 2

Foundations: H-T C-7 D-A S-J
Freecells:  KH  4D  KD  5D
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
: 2D
: 6D
: 3D
: QS JD


====================

Move a card from stack 4 to the foundations

Foundations: H-T C-7 D-2 S-J
Freecells:  KH  4D  KD  5D
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
:
: 6D
: 3D
: QS JD


====================

Move a card from stack 6 to the foundations

Foundations: H-T C-7 D-3 S-J
Freecells:  KH  4D  KD  5D
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
:
: 6D
:
: QS JD


====================

Move a card from freecell 1 to the foundations

Foundations: H-T C-7 D-4 S-J
Freecells:  KH      KD  5D
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
:
: 6D
:
: QS JD


====================

Move a card from freecell 3 to the foundations

Foundations: H-T C-7 D-5 S-J
Freecells:  KH      KD
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
:
: 6D
:
: QS JD


====================

Move a card from stack 5 to the foundations

Foundations: H-T C-7 D-6 S-J
Freecells:  KH      KD
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 2 to the foundations

Foundations: H-T C-7 D-7 S-J
Freecells:  KH      KD
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 0 to the foundations

Foundations: H-T C-7 D-8 S-J
Freecells:  KH      KD
: 9C
: KS QH JC TD
: QC JH TC 9D 8C
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 2 to the foundations

Foundations: H-T C-8 D-8 S-J
Freecells:  KH      KD
: 9C
: KS QH JC TD
: QC JH TC 9D
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 0 to the foundations

Foundations: H-T C-9 D-8 S-J
Freecells:  KH      KD
:
: KS QH JC TD
: QC JH TC 9D
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 2 to the foundations

Foundations: H-T C-9 D-9 S-J
Freecells:  KH      KD
:
: KS QH JC TD
: QC JH TC
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 2 to the foundations

Foundations: H-T C-T D-9 S-J
Freecells:  KH      KD
:
: KS QH JC TD
: QC JH
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 2 to the foundations

Foundations: H-J C-T D-9 S-J
Freecells:  KH      KD
:
: KS QH JC TD
: QC
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 1 to the foundations

Foundations: H-J C-T D-T S-J
Freecells:  KH      KD
:
: KS QH JC
: QC
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 7 to the foundations

Foundations: H-J C-T D-J S-J
Freecells:  KH      KD
:
: KS QH JC
: QC
: KC QD
:
:
:
: QS


====================

Move a card from stack 7 to the foundations

Foundations: H-J C-T D-J S-Q
Freecells:  KH      KD
:
: KS QH JC
: QC
: KC QD
:
:
:
:


====================

Move a card from stack 3 to the foundations

Foundations: H-J C-T D-Q S-Q
Freecells:  KH      KD
:
: KS QH JC
: QC
: KC
:
:
:
:


====================

Move a card from stack 1 to the foundations

Foundations: H-J C-J D-Q S-Q
Freecells:  KH      KD
:
: KS QH
: QC
: KC
:
:
:
:


====================

Move a card from stack 2 to the foundations

Foundations: H-J C-Q D-Q S-Q
Freecells:  KH      KD
:
: KS QH
:
: KC
:
:
:
:


====================

Move a card from stack 3 to the foundations

Foundations: H-J C-K D-Q S-Q
Freecells:  KH      KD
:
: KS QH
:
:
:
:
:
:


====================

Move a card from stack 1 to the foundations

Foundations: H-Q C-K D-Q S-Q
Freecells:  KH      KD
:
: KS
:
:
:
:
:
:


====================

Move a card from stack 1 to the foundations

Foundations: H-Q C-K D-Q S-K
Freecells:  KH      KD
:
:
:
:
:
:
:
:


====================

Move a card from freecell 0 to the foundations

Foundations: H-K C-K D-Q S-K
Freecells:          KD
:
:
:
:
:
:
:
:


====================

Move a card from freecell 2 to the foundations

Foundations: H-K C-K D-K S-K
Freecells:
:
:
:
:
:
:
:
:


====================

`;
    const solution_for_deal_24__default__with_unicoded_suits =
`-=-=-=-=-=-=-=-=-=-=-=-

Foundations: ♥-0 ♣-0 ♦-0 ♠-0
Freecells:
: 4♣ 2♣ 9♣ 8♣ Q♠ 4♠ 2♥
: 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ A♠
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
: A♥ 5♠ 6♠ A♦ 8♥ J♦
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 3 to the foundations

Foundations: ♥-0 ♣-0 ♦-0 ♠-A
Freecells:
: 4♣ 2♣ 9♣ 8♣ Q♠ 4♠ 2♥
: 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
: A♥ 5♠ 6♠ A♦ 8♥ J♦
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 6 to freecell 0

Foundations: ♥-0 ♣-0 ♦-0 ♠-A
Freecells:  J♦
: 4♣ 2♣ 9♣ 8♣ Q♠ 4♠ 2♥
: 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
: A♥ 5♠ 6♠ A♦ 8♥
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 6 to freecell 1

Foundations: ♥-0 ♣-0 ♦-0 ♠-A
Freecells:  J♦  8♥
: 4♣ 2♣ 9♣ 8♣ Q♠ 4♠ 2♥
: 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
: A♥ 5♠ 6♠ A♦
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 6 to the foundations

Foundations: ♥-0 ♣-0 ♦-A ♠-A
Freecells:  J♦  8♥
: 4♣ 2♣ 9♣ 8♣ Q♠ 4♠ 2♥
: 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
: A♥ 5♠ 6♠
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 0 to freecell 2

Foundations: ♥-0 ♣-0 ♦-A ♠-A
Freecells:  J♦  8♥  2♥
: 4♣ 2♣ 9♣ 8♣ Q♠ 4♠
: 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
: A♥ 5♠ 6♠
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 0 to freecell 3

Foundations: ♥-0 ♣-0 ♦-A ♠-A
Freecells:  J♦  8♥  2♥  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠
: 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
: A♥ 5♠ 6♠
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from freecell 0 to stack 0

Foundations: ♥-0 ♣-0 ♦-A ♠-A
Freecells:      8♥  2♥  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
: A♥ 5♠ 6♠
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 6 to freecell 0

Foundations: ♥-0 ♣-0 ♦-A ♠-A
Freecells:  6♠  8♥  2♥  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
: A♥ 5♠
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move 1 cards from stack 6 to stack 3

Foundations: ♥-0 ♣-0 ♦-A ♠-A
Freecells:  6♠  8♥  2♥  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
: A♥
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 6 to the foundations

Foundations: ♥-A ♣-0 ♦-A ♠-A
Freecells:  6♠  8♥  2♥  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
:
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from freecell 2 to the foundations

Foundations: ♥-2 ♣-0 ♦-A ♠-A
Freecells:  6♠  8♥      4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ Q♥ 3♣ A♣ 3♥ 4♥ Q♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
:
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 1 to freecell 2

Foundations: ♥-2 ♣-0 ♦-A ♠-A
Freecells:  6♠  8♥  Q♦  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ Q♥ 3♣ A♣ 3♥ 4♥
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
:
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move 1 cards from stack 1 to stack 6

Foundations: ♥-2 ♣-0 ♦-A ♠-A
Freecells:  6♠  8♥  Q♦  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ Q♥ 3♣ A♣ 3♥
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
: 4♥
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 1 to the foundations

Foundations: ♥-3 ♣-0 ♦-A ♠-A
Freecells:  6♠  8♥  Q♦  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ Q♥ 3♣ A♣
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
: 4♥
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 6 to the foundations

Foundations: ♥-4 ♣-0 ♦-A ♠-A
Freecells:  6♠  8♥  Q♦  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ Q♥ 3♣ A♣
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
:
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 1 to the foundations

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥  Q♦  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ Q♥ 3♣
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ 7♣
:
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move 1 cards from stack 5 to stack 6

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥  Q♦  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ Q♥ 3♣
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣
: 7♣
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from freecell 2 to stack 5

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥      4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ Q♥ 3♣
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ K♣ Q♦
: 7♣
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move 1 cards from stack 6 to stack 4

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥      4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ Q♥ 3♣
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣
: 7♥ J♠ K♥ T♠ K♣ Q♦
:
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 1 to freecell 2

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥  3♣  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ Q♥
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣
: 7♥ J♠ K♥ T♠ K♣ Q♦
:
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move 1 cards from stack 1 to stack 6

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥  3♣  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣
: 7♥ J♠ K♥ T♠ K♣ Q♦
: Q♥
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from freecell 3 to stack 1

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥  3♣
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥ 4♠
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣
: 7♥ J♠ K♥ T♠ K♣ Q♦
: Q♥
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 1 to freecell 3

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥  3♣  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 5♥
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣
: 7♥ J♠ K♥ T♠ K♣ Q♦
: Q♥
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move a card from stack 1 to the foundations

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥  3♣  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
:
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥ 6♦ 5♠
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣
: 7♥ J♠ K♥ T♠ K♣ Q♦
: Q♥
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move 2 cards from stack 3 to stack 4

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥  3♣  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
:
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ 3♦
: 5♦ 2♠ J♣ 5♣ J♥
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠
: 7♥ J♠ K♥ T♠ K♣ Q♦
: Q♥
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move 1 cards from stack 2 to stack 1

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥  3♣  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 3♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠
: 5♦ 2♠ J♣ 5♣ J♥
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠
: 7♥ J♠ K♥ T♠ K♣ Q♦
: Q♥
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move 1 cards from stack 6 to stack 2

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥  3♣  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 3♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥
: 5♦ 2♠ J♣ 5♣ J♥
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠
: 7♥ J♠ K♥ T♠ K♣ Q♦
:
: 7♠ 6♣ 7♦ 4♦ 8♠ 9♦


====================

Move 1 cards from stack 7 to stack 6

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥  3♣  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 3♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥
: 5♦ 2♠ J♣ 5♣ J♥
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠
: 7♥ J♠ K♥ T♠ K♣ Q♦
: 9♦
: 7♠ 6♣ 7♦ 4♦ 8♠


====================

Move 1 cards from stack 7 to stack 6

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥  3♣  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 3♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥
: 5♦ 2♠ J♣ 5♣ J♥
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠
: 7♥ J♠ K♥ T♠ K♣ Q♦
: 9♦ 8♠
: 7♠ 6♣ 7♦ 4♦


====================

Move a card from freecell 2 to stack 7

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥      4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 3♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥
: 5♦ 2♠ J♣ 5♣ J♥
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠
: 7♥ J♠ K♥ T♠ K♣ Q♦
: 9♦ 8♠
: 7♠ 6♣ 7♦ 4♦ 3♣


====================

Move 2 cards from stack 7 to stack 4

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  6♠  8♥      4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 3♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥
: 5♦ 2♠ J♣ 5♣ J♥
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦
: 9♦ 8♠
: 7♠ 6♣ 7♦


====================

Move a card from freecell 0 to stack 7

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:      8♥      4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 3♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥
: 5♦ 2♠ J♣ 5♣ J♥
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move a card from stack 3 to freecell 2

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:      8♥  J♥  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 3♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥
: 5♦ 2♠ J♣ 5♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move a card from stack 3 to freecell 0

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  5♣  8♥  J♥  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 3♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥
: 5♦ 2♠ J♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move 1 cards from stack 3 to stack 5

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  5♣  8♥  J♥  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 3♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥
: 5♦ 2♠
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move a card from stack 3 to the foundations

Foundations: ♥-5 ♣-A ♦-A ♠-2
Freecells:  5♣  8♥  J♥  4♠
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 3♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥
: 5♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move a card from freecell 3 to stack 3

Foundations: ♥-5 ♣-A ♦-A ♠-2
Freecells:  5♣  8♥  J♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: 3♦
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥
: 5♦ 4♠
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move 1 cards from stack 1 to stack 3

Foundations: ♥-5 ♣-A ♦-A ♠-2
Freecells:  5♣  8♥  J♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
:
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠ Q♥
: 5♦ 4♠ 3♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move a card from stack 2 to freecell 3

Foundations: ♥-5 ♣-A ♦-A ♠-2
Freecells:  5♣  8♥  J♥  Q♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
:
: Q♣ 9♠ 6♥ 9♥ 3♠ K♠
: 5♦ 4♠ 3♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move 1 cards from stack 2 to stack 1

Foundations: ♥-5 ♣-A ♦-A ♠-2
Freecells:  5♣  8♥  J♥  Q♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠
: Q♣ 9♠ 6♥ 9♥ 3♠
: 5♦ 4♠ 3♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move a card from stack 2 to the foundations

Foundations: ♥-5 ♣-A ♦-A ♠-3
Freecells:  5♣  8♥  J♥  Q♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠
: Q♣ 9♠ 6♥ 9♥
: 5♦ 4♠ 3♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move a card from freecell 3 to stack 1

Foundations: ♥-5 ♣-A ♦-A ♠-3
Freecells:  5♣  8♥  J♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 6♥ 9♥
: 5♦ 4♠ 3♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move a card from stack 2 to freecell 3

Foundations: ♥-5 ♣-A ♦-A ♠-3
Freecells:  5♣  8♥  J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 6♥
: 5♦ 4♠ 3♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move a card from freecell 0 to stack 2

Foundations: ♥-5 ♣-A ♦-A ♠-3
Freecells:      8♥  J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 6♥ 5♣
: 5♦ 4♠ 3♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move a card from stack 3 to freecell 0

Foundations: ♥-5 ♣-A ♦-A ♠-3
Freecells:  3♦  8♥  J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 6♥ 5♣
: 5♦ 4♠
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move a card from stack 3 to the foundations

Foundations: ♥-5 ♣-A ♦-A ♠-4
Freecells:  3♦  8♥  J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 6♥ 5♣
: 5♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move 1 cards from stack 3 to stack 7

Foundations: ♥-5 ♣-A ♦-A ♠-4
Freecells:  3♦  8♥  J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 6♥ 5♣
:
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠ 5♦


====================

Move 1 cards from stack 2 to stack 3

Foundations: ♥-5 ♣-A ♦-A ♠-4
Freecells:  3♦  8♥  J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 6♥
: 5♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠ 5♦


====================

Move a card from stack 2 to the foundations

Foundations: ♥-6 ♣-A ♦-A ♠-4
Freecells:  3♦  8♥  J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠
: 5♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠ 5♦


====================

Move a card from freecell 1 to stack 2

Foundations: ♥-6 ♣-A ♦-A ♠-4
Freecells:  3♦      J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 8♥
: 5♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠ 5♦


====================

Move 2 cards from stack 4 to stack 3

Foundations: ♥-6 ♣-A ♦-A ♠-4
Freecells:  3♦      J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 8♥
: 5♣ 4♦ 3♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♠
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠ 5♦


====================

Move a card from stack 4 to the foundations

Foundations: ♥-6 ♣-A ♦-A ♠-5
Freecells:  3♦      J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 8♥
: 5♣ 4♦ 3♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠ 5♦


====================

Move a card from stack 7 to freecell 1

Foundations: ♥-6 ♣-A ♦-A ♠-5
Freecells:  3♦  5♦  J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 8♥
: 5♣ 4♦ 3♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦ 6♠


====================

Move a card from stack 7 to the foundations

Foundations: ♥-6 ♣-A ♦-A ♠-6
Freecells:  3♦  5♦  J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 8♥
: 5♣ 4♦ 3♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠
: 7♠ 6♣ 7♦


====================

Move 1 cards from stack 7 to stack 6

Foundations: ♥-6 ♣-A ♦-A ♠-6
Freecells:  3♦  5♦  J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 8♥
: 5♣ 4♦ 3♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠ 7♦
: 7♠ 6♣


====================

Move a card from freecell 1 to stack 7

Foundations: ♥-6 ♣-A ♦-A ♠-6
Freecells:  3♦      J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 8♥
: 5♣ 4♦ 3♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠ 7♦
: 7♠ 6♣ 5♦


====================

Move 2 cards from stack 7 to stack 6

Foundations: ♥-6 ♣-A ♦-A ♠-6
Freecells:  3♦      J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 8♥
: 5♣ 4♦ 3♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 7♠


====================

Move a card from stack 7 to the foundations

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦      J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠ 8♥
: 5♣ 4♦ 3♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠ 7♦ 6♣ 5♦
:


====================

Move a card from stack 2 to freecell 1

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦  8♥  J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ 9♠
: 5♣ 4♦ 3♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠ 7♦ 6♣ 5♦
:


====================

Move 1 cards from stack 2 to stack 7

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦  8♥  J♥  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣
: 5♣ 4♦ 3♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠


====================

Move a card from freecell 2 to stack 2

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦  8♥      9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ J♥
: 5♣ 4♦ 3♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠


====================

Move a card from freecell 1 to stack 7

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦          9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ J♥
: 5♣ 4♦ 3♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move 3 cards from stack 3 to stack 4

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦          9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ J♥
:
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦ J♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move a card from stack 5 to freecell 2

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦      J♣  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ J♥
:
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣ Q♦
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move a card from stack 5 to freecell 1

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦  Q♦  J♣  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ J♥
:
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣
: 7♥ J♠ K♥ T♠ K♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move 1 cards from stack 5 to stack 3

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦  Q♦  J♣  9♥
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ J♥
: K♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣
: 7♥ J♠ K♥ T♠
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move a card from freecell 3 to stack 5

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦  Q♦  J♣
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ J♥
: K♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣
: 7♥ J♠ K♥ T♠ 9♥
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move a card from freecell 1 to stack 3

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦      J♣
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣
: 7♥ J♠ K♥ T♠ 9♥
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move a card from freecell 2 to stack 3

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦
: 4♣ 2♣ 9♣ 8♣ Q♠ J♦
: K♠ Q♥
: Q♣ J♥
: K♣ Q♦ J♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣
: 7♥ J♠ K♥ T♠ 9♥
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move a card from stack 0 to freecell 3

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦          J♦
: 4♣ 2♣ 9♣ 8♣ Q♠
: K♠ Q♥
: Q♣ J♥
: K♣ Q♦ J♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣
: 7♥ J♠ K♥ T♠ 9♥
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move a card from stack 0 to freecell 1

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦  Q♠      J♦
: 4♣ 2♣ 9♣ 8♣
: K♠ Q♥
: Q♣ J♥
: K♣ Q♦ J♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣
: 7♥ J♠ K♥ T♠ 9♥
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move 1 cards from stack 0 to stack 5

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦  Q♠      J♦
: 4♣ 2♣ 9♣
: K♠ Q♥
: Q♣ J♥
: K♣ Q♦ J♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move a card from stack 0 to freecell 2

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  3♦  Q♠  9♣  J♦
: 4♣ 2♣
: K♠ Q♥
: Q♣ J♥
: K♣ Q♦ J♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move a card from stack 0 to the foundations

Foundations: ♥-6 ♣-2 ♦-A ♠-7
Freecells:  3♦  Q♠  9♣  J♦
: 4♣
: K♠ Q♥
: Q♣ J♥
: K♣ Q♦ J♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦ 3♣
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move a card from stack 4 to the foundations

Foundations: ♥-6 ♣-3 ♦-A ♠-7
Freecells:  3♦  Q♠  9♣  J♦
: 4♣
: K♠ Q♥
: Q♣ J♥
: K♣ Q♦ J♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move a card from stack 0 to the foundations

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  3♦  Q♠  9♣  J♦
:
: K♠ Q♥
: Q♣ J♥
: K♣ Q♦ J♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move a card from freecell 0 to stack 0

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:      Q♠  9♣  J♦
: 3♦
: K♠ Q♥
: Q♣ J♥
: K♣ Q♦ J♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move 1 cards from stack 3 to stack 1

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:      Q♠  9♣  J♦
: 3♦
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move a card from stack 0 to freecell 0

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  3♦  Q♠  9♣  J♦
:
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move a card from freecell 2 to stack 0

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  3♦  Q♠      J♦
: 9♣
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠ 8♥


====================

Move 1 cards from stack 7 to stack 0

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  3♦  Q♠      J♦
: 9♣ 8♥
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠


====================

Move 1 cards from stack 1 to stack 3

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  3♦  Q♠      J♦
: 9♣ 8♥
: K♠ Q♥
: Q♣ J♥
: K♣ Q♦ J♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 9♠


====================

Move a card from stack 7 to freecell 2

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  3♦  Q♠  9♠  J♦
: 9♣ 8♥
: K♠ Q♥
: Q♣ J♥
: K♣ Q♦ J♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
:


====================

Move a card from freecell 0 to stack 7

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:      Q♠  9♠  J♦
: 9♣ 8♥
: K♠ Q♥
: Q♣ J♥
: K♣ Q♦ J♣
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 3♦


====================

Move 1 cards from stack 3 to stack 1

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:      Q♠  9♠  J♦
: 9♣ 8♥
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: 3♦


====================

Move a card from stack 7 to freecell 0

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  3♦  Q♠  9♠  J♦
: 9♣ 8♥
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
:


====================

Move a card from freecell 1 to stack 7

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  3♦      9♠  J♦
: 9♣ 8♥
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: Q♠


====================

Move a card from freecell 3 to stack 7

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  3♦      9♠
: 9♣ 8♥
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣ 4♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: Q♠ J♦


====================

Move a card from stack 4 to freecell 1

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  3♦  4♦  9♠
: 9♣ 8♥
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦ 5♣
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: Q♠ J♦


====================

Move a card from stack 4 to the foundations

Foundations: ♥-6 ♣-5 ♦-A ♠-7
Freecells:  3♦  4♦  9♠
: 9♣ 8♥
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣ 5♦
: Q♠ J♦


====================

Move a card from stack 6 to freecell 3

Foundations: ♥-6 ♣-5 ♦-A ♠-7
Freecells:  3♦  4♦  9♠  5♦
: 9♣ 8♥
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦ 6♣
: Q♠ J♦


====================

Move a card from stack 6 to the foundations

Foundations: ♥-6 ♣-6 ♦-A ♠-7
Freecells:  3♦  4♦  9♠  5♦
: 9♣ 8♥
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣
: 9♦ 8♠ 7♦
: Q♠ J♦


====================

Move 1 cards from stack 6 to stack 5

Foundations: ♥-6 ♣-6 ♦-A ♠-7
Freecells:  3♦  4♦  9♠  5♦
: 9♣ 8♥
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦
: 9♦ 8♠
: Q♠ J♦


====================

Move a card from stack 6 to the foundations

Foundations: ♥-6 ♣-6 ♦-A ♠-8
Freecells:  3♦  4♦  9♠  5♦
: 9♣ 8♥
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦
: 9♦
: Q♠ J♦


====================

Move a card from freecell 2 to the foundations

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  3♦  4♦      5♦
: 9♣ 8♥
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦ 7♣ 6♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦
: 9♦
: Q♠ J♦


====================

Move 2 cards from stack 4 to stack 0

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  3♦  4♦      5♦
: 9♣ 8♥ 7♣ 6♦
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦ 8♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦
: 9♦
: Q♠ J♦


====================

Move a card from stack 4 to freecell 2

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  3♦  4♦  8♦  5♦
: 9♣ 8♥ 7♣ 6♦
: K♠ Q♥ J♣
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣ T♦
: 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦
: 9♦
: Q♠ J♦


====================

Move 1 cards from stack 4 to stack 1

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  3♦  4♦  8♦  5♦
: 9♣ 8♥ 7♣ 6♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥
: K♣ Q♦
: 2♦ K♦ T♥ T♣
: 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦
: 9♦
: Q♠ J♦


====================

Move 1 cards from stack 4 to stack 2

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  3♦  4♦  8♦  5♦
: 9♣ 8♥ 7♣ 6♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣
: K♣ Q♦
: 2♦ K♦ T♥
: 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦
: 9♦
: Q♠ J♦


====================

Move 1 cards from stack 6 to stack 2

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  3♦  4♦  8♦  5♦
: 9♣ 8♥ 7♣ 6♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦
: K♣ Q♦
: 2♦ K♦ T♥
: 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦
:
: Q♠ J♦


====================

Move a card from freecell 0 to stack 6

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:      4♦  8♦  5♦
: 9♣ 8♥ 7♣ 6♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦
: K♣ Q♦
: 2♦ K♦ T♥
: 7♥ J♠ K♥ T♠ 9♥ 8♣ 7♦
: 3♦
: Q♠ J♦


====================

Move 2 cards from stack 5 to stack 2

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:      4♦  8♦  5♦
: 9♣ 8♥ 7♣ 6♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦
: 2♦ K♦ T♥
: 7♥ J♠ K♥ T♠ 9♥
: 3♦
: Q♠ J♦


====================

Move 2 cards from stack 5 to stack 7

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:      4♦  8♦  5♦
: 9♣ 8♥ 7♣ 6♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦
: 2♦ K♦ T♥
: 7♥ J♠ K♥
: 3♦
: Q♠ J♦ T♠ 9♥


====================

Move a card from stack 5 to freecell 0

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  K♥  4♦  8♦  5♦
: 9♣ 8♥ 7♣ 6♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦
: 2♦ K♦ T♥
: 7♥ J♠
: 3♦
: Q♠ J♦ T♠ 9♥


====================

Move 1 cards from stack 5 to stack 3

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  K♥  4♦  8♦  5♦
: 9♣ 8♥ 7♣ 6♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦ J♠
: 2♦ K♦ T♥
: 7♥
: 3♦
: Q♠ J♦ T♠ 9♥


====================

Move a card from stack 5 to the foundations

Foundations: ♥-7 ♣-6 ♦-A ♠-9
Freecells:  K♥  4♦  8♦  5♦
: 9♣ 8♥ 7♣ 6♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦ J♠
: 2♦ K♦ T♥
:
: 3♦
: Q♠ J♦ T♠ 9♥


====================

Move 1 cards from stack 0 to stack 5

Foundations: ♥-7 ♣-6 ♦-A ♠-9
Freecells:  K♥  4♦  8♦  5♦
: 9♣ 8♥ 7♣
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦ J♠
: 2♦ K♦ T♥
: 6♦
: 3♦
: Q♠ J♦ T♠ 9♥


====================

Move a card from stack 0 to the foundations

Foundations: ♥-7 ♣-7 ♦-A ♠-9
Freecells:  K♥  4♦  8♦  5♦
: 9♣ 8♥
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦ J♠
: 2♦ K♦ T♥
: 6♦
: 3♦
: Q♠ J♦ T♠ 9♥


====================

Move a card from stack 0 to the foundations

Foundations: ♥-8 ♣-7 ♦-A ♠-9
Freecells:  K♥  4♦  8♦  5♦
: 9♣
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦ J♠
: 2♦ K♦ T♥
: 6♦
: 3♦
: Q♠ J♦ T♠ 9♥


====================

Move a card from stack 7 to the foundations

Foundations: ♥-9 ♣-7 ♦-A ♠-9
Freecells:  K♥  4♦  8♦  5♦
: 9♣
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦ J♠
: 2♦ K♦ T♥
: 6♦
: 3♦
: Q♠ J♦ T♠


====================

Move a card from stack 4 to the foundations

Foundations: ♥-T ♣-7 ♦-A ♠-9
Freecells:  K♥  4♦  8♦  5♦
: 9♣
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦ J♠
: 2♦ K♦
: 6♦
: 3♦
: Q♠ J♦ T♠


====================

Move a card from stack 7 to the foundations

Foundations: ♥-T ♣-7 ♦-A ♠-T
Freecells:  K♥  4♦  8♦  5♦
: 9♣
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦ J♠
: 2♦ K♦
: 6♦
: 3♦
: Q♠ J♦


====================

Move a card from stack 3 to the foundations

Foundations: ♥-T ♣-7 ♦-A ♠-J
Freecells:  K♥  4♦  8♦  5♦
: 9♣
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦
: 2♦ K♦
: 6♦
: 3♦
: Q♠ J♦


====================

Move a card from freecell 2 to stack 0

Foundations: ♥-T ♣-7 ♦-A ♠-J
Freecells:  K♥  4♦      5♦
: 9♣ 8♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦
: 2♦ K♦
: 6♦
: 3♦
: Q♠ J♦


====================

Move a card from stack 4 to freecell 2

Foundations: ♥-T ♣-7 ♦-A ♠-J
Freecells:  K♥  4♦  K♦  5♦
: 9♣ 8♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦
: 2♦
: 6♦
: 3♦
: Q♠ J♦


====================

Move a card from stack 4 to the foundations

Foundations: ♥-T ♣-7 ♦-2 ♠-J
Freecells:  K♥  4♦  K♦  5♦
: 9♣ 8♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦
:
: 6♦
: 3♦
: Q♠ J♦


====================

Move a card from stack 6 to the foundations

Foundations: ♥-T ♣-7 ♦-3 ♠-J
Freecells:  K♥  4♦  K♦  5♦
: 9♣ 8♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦
:
: 6♦
:
: Q♠ J♦


====================

Move a card from freecell 1 to the foundations

Foundations: ♥-T ♣-7 ♦-4 ♠-J
Freecells:  K♥      K♦  5♦
: 9♣ 8♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦
:
: 6♦
:
: Q♠ J♦


====================

Move a card from freecell 3 to the foundations

Foundations: ♥-T ♣-7 ♦-5 ♠-J
Freecells:  K♥      K♦
: 9♣ 8♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦
:
: 6♦
:
: Q♠ J♦


====================

Move a card from stack 5 to the foundations

Foundations: ♥-T ♣-7 ♦-6 ♠-J
Freecells:  K♥      K♦
: 9♣ 8♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣ 7♦
: K♣ Q♦
:
:
:
: Q♠ J♦


====================

Move a card from stack 2 to the foundations

Foundations: ♥-T ♣-7 ♦-7 ♠-J
Freecells:  K♥      K♦
: 9♣ 8♦
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣
: K♣ Q♦
:
:
:
: Q♠ J♦


====================

Move a card from stack 0 to the foundations

Foundations: ♥-T ♣-7 ♦-8 ♠-J
Freecells:  K♥      K♦
: 9♣
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦ 8♣
: K♣ Q♦
:
:
:
: Q♠ J♦


====================

Move a card from stack 2 to the foundations

Foundations: ♥-T ♣-8 ♦-8 ♠-J
Freecells:  K♥      K♦
: 9♣
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦
: K♣ Q♦
:
:
:
: Q♠ J♦


====================

Move a card from stack 0 to the foundations

Foundations: ♥-T ♣-9 ♦-8 ♠-J
Freecells:  K♥      K♦
:
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣ 9♦
: K♣ Q♦
:
:
:
: Q♠ J♦


====================

Move a card from stack 2 to the foundations

Foundations: ♥-T ♣-9 ♦-9 ♠-J
Freecells:  K♥      K♦
:
: K♠ Q♥ J♣ T♦
: Q♣ J♥ T♣
: K♣ Q♦
:
:
:
: Q♠ J♦


====================

Move a card from stack 2 to the foundations

Foundations: ♥-T ♣-T ♦-9 ♠-J
Freecells:  K♥      K♦
:
: K♠ Q♥ J♣ T♦
: Q♣ J♥
: K♣ Q♦
:
:
:
: Q♠ J♦


====================

Move a card from stack 2 to the foundations

Foundations: ♥-J ♣-T ♦-9 ♠-J
Freecells:  K♥      K♦
:
: K♠ Q♥ J♣ T♦
: Q♣
: K♣ Q♦
:
:
:
: Q♠ J♦


====================

Move a card from stack 1 to the foundations

Foundations: ♥-J ♣-T ♦-T ♠-J
Freecells:  K♥      K♦
:
: K♠ Q♥ J♣
: Q♣
: K♣ Q♦
:
:
:
: Q♠ J♦


====================

Move a card from stack 7 to the foundations

Foundations: ♥-J ♣-T ♦-J ♠-J
Freecells:  K♥      K♦
:
: K♠ Q♥ J♣
: Q♣
: K♣ Q♦
:
:
:
: Q♠


====================

Move a card from stack 7 to the foundations

Foundations: ♥-J ♣-T ♦-J ♠-Q
Freecells:  K♥      K♦
:
: K♠ Q♥ J♣
: Q♣
: K♣ Q♦
:
:
:
:


====================

Move a card from stack 3 to the foundations

Foundations: ♥-J ♣-T ♦-Q ♠-Q
Freecells:  K♥      K♦
:
: K♠ Q♥ J♣
: Q♣
: K♣
:
:
:
:


====================

Move a card from stack 1 to the foundations

Foundations: ♥-J ♣-J ♦-Q ♠-Q
Freecells:  K♥      K♦
:
: K♠ Q♥
: Q♣
: K♣
:
:
:
:


====================

Move a card from stack 2 to the foundations

Foundations: ♥-J ♣-Q ♦-Q ♠-Q
Freecells:  K♥      K♦
:
: K♠ Q♥
:
: K♣
:
:
:
:


====================

Move a card from stack 3 to the foundations

Foundations: ♥-J ♣-K ♦-Q ♠-Q
Freecells:  K♥      K♦
:
: K♠ Q♥
:
:
:
:
:
:


====================

Move a card from stack 1 to the foundations

Foundations: ♥-Q ♣-K ♦-Q ♠-Q
Freecells:  K♥      K♦
:
: K♠
:
:
:
:
:
:


====================

Move a card from stack 1 to the foundations

Foundations: ♥-Q ♣-K ♦-Q ♠-K
Freecells:  K♥      K♦
:
:
:
:
:
:
:
:


====================

Move a card from freecell 0 to the foundations

Foundations: ♥-K ♣-K ♦-Q ♠-K
Freecells:          K♦
:
:
:
:
:
:
:
:


====================

Move a card from freecell 2 to the foundations

Foundations: ♥-K ♣-K ♦-K ♠-K
Freecells:
:
:
:
:
:
:
:
:


====================

`;
    const solution_for_deal_24__default__with_unicoded_card_chars =
`-=-=-=-=-=-=-=-=-=-=-=-

Foundations: ♥-0 ♣-0 ♦-0 ♠-0
Freecells:
: 🃔 🃒 🃙 🃘 🂬 🂤 🂲
: 🂵 🂼 🃓 🃑 🂳 🂴 🃌
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂡
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
: 🂱 🂥 🂦 🃁 🂸 🃋
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 3 to the foundations

Foundations: ♥-0 ♣-0 ♦-0 ♠-A
Freecells:
: 🃔 🃒 🃙 🃘 🂬 🂤 🂲
: 🂵 🂼 🃓 🃑 🂳 🂴 🃌
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
: 🂱 🂥 🂦 🃁 🂸 🃋
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 6 to freecell 0

Foundations: ♥-0 ♣-0 ♦-0 ♠-A
Freecells:  🃋
: 🃔 🃒 🃙 🃘 🂬 🂤 🂲
: 🂵 🂼 🃓 🃑 🂳 🂴 🃌
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
: 🂱 🂥 🂦 🃁 🂸
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 6 to freecell 1

Foundations: ♥-0 ♣-0 ♦-0 ♠-A
Freecells:  🃋  🂸
: 🃔 🃒 🃙 🃘 🂬 🂤 🂲
: 🂵 🂼 🃓 🃑 🂳 🂴 🃌
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
: 🂱 🂥 🂦 🃁
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 6 to the foundations

Foundations: ♥-0 ♣-0 ♦-A ♠-A
Freecells:  🃋  🂸
: 🃔 🃒 🃙 🃘 🂬 🂤 🂲
: 🂵 🂼 🃓 🃑 🂳 🂴 🃌
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
: 🂱 🂥 🂦
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 0 to freecell 2

Foundations: ♥-0 ♣-0 ♦-A ♠-A
Freecells:  🃋  🂸  🂲
: 🃔 🃒 🃙 🃘 🂬 🂤
: 🂵 🂼 🃓 🃑 🂳 🂴 🃌
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
: 🂱 🂥 🂦
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 0 to freecell 3

Foundations: ♥-0 ♣-0 ♦-A ♠-A
Freecells:  🃋  🂸  🂲  🂤
: 🃔 🃒 🃙 🃘 🂬
: 🂵 🂼 🃓 🃑 🂳 🂴 🃌
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
: 🂱 🂥 🂦
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from freecell 0 to stack 0

Foundations: ♥-0 ♣-0 ♦-A ♠-A
Freecells:      🂸  🂲  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂼 🃓 🃑 🂳 🂴 🃌
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
: 🂱 🂥 🂦
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 6 to freecell 0

Foundations: ♥-0 ♣-0 ♦-A ♠-A
Freecells:  🂦  🂸  🂲  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂼 🃓 🃑 🂳 🂴 🃌
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
: 🂱 🂥
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move 1 cards from stack 6 to stack 3

Foundations: ♥-0 ♣-0 ♦-A ♠-A
Freecells:  🂦  🂸  🂲  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂼 🃓 🃑 🂳 🂴 🃌
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
: 🂱
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 6 to the foundations

Foundations: ♥-A ♣-0 ♦-A ♠-A
Freecells:  🂦  🂸  🂲  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂼 🃓 🃑 🂳 🂴 🃌
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
:
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from freecell 2 to the foundations

Foundations: ♥-2 ♣-0 ♦-A ♠-A
Freecells:  🂦  🂸      🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂼 🃓 🃑 🂳 🂴 🃌
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
:
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 1 to freecell 2

Foundations: ♥-2 ♣-0 ♦-A ♠-A
Freecells:  🂦  🂸  🃌  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂼 🃓 🃑 🂳 🂴
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
:
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move 1 cards from stack 1 to stack 6

Foundations: ♥-2 ♣-0 ♦-A ♠-A
Freecells:  🂦  🂸  🃌  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂼 🃓 🃑 🂳
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
: 🂴
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 1 to the foundations

Foundations: ♥-3 ♣-0 ♦-A ♠-A
Freecells:  🂦  🂸  🃌  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂼 🃓 🃑
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
: 🂴
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 6 to the foundations

Foundations: ♥-4 ♣-0 ♦-A ♠-A
Freecells:  🂦  🂸  🃌  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂼 🃓 🃑
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
:
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 1 to the foundations

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸  🃌  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂼 🃓
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃗
:
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move 1 cards from stack 5 to stack 6

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸  🃌  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂼 🃓
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝
: 🃗
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from freecell 2 to stack 5

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸      🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂼 🃓
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🃗
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move 1 cards from stack 6 to stack 4

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸      🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂼 🃓
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗
: 🂷 🂫 🂽 🂪 🃝 🃌
:
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 1 to freecell 2

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸  🃓  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂼
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗
: 🂷 🂫 🂽 🂪 🃝 🃌
:
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move 1 cards from stack 1 to stack 6

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸  🃓  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🂼
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from freecell 3 to stack 1

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸  🃓
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵 🂤
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🂼
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 1 to freecell 3

Foundations: ♥-4 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸  🃓  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂵
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🂼
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move a card from stack 1 to the foundations

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸  🃓  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
:
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻 🃆 🂥
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🂼
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move 2 cards from stack 3 to stack 4

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸  🃓  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
:
: 🃜 🂩 🂶 🂹 🂣 🂭 🃃
: 🃅 🂢 🃛 🃕 🂻
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🂼
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move 1 cards from stack 2 to stack 1

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸  🃓  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🃃
: 🃜 🂩 🂶 🂹 🂣 🂭
: 🃅 🂢 🃛 🃕 🂻
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🂼
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move 1 cards from stack 6 to stack 2

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸  🃓  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🃃
: 🃜 🂩 🂶 🂹 🂣 🂭 🂼
: 🃅 🂢 🃛 🃕 🂻
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥
: 🂷 🂫 🂽 🂪 🃝 🃌
:
: 🂧 🃖 🃇 🃄 🂨 🃉


====================

Move 1 cards from stack 7 to stack 6

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸  🃓  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🃃
: 🃜 🂩 🂶 🂹 🂣 🂭 🂼
: 🃅 🂢 🃛 🃕 🂻
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🃉
: 🂧 🃖 🃇 🃄 🂨


====================

Move 1 cards from stack 7 to stack 6

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸  🃓  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🃃
: 🃜 🂩 🂶 🂹 🂣 🂭 🂼
: 🃅 🂢 🃛 🃕 🂻
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🃉 🂨
: 🂧 🃖 🃇 🃄


====================

Move a card from freecell 2 to stack 7

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸      🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🃃
: 🃜 🂩 🂶 🂹 🂣 🂭 🂼
: 🃅 🂢 🃛 🃕 🂻
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🃉 🂨
: 🂧 🃖 🃇 🃄 🃓


====================

Move 2 cards from stack 7 to stack 4

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  🂦  🂸      🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🃃
: 🃜 🂩 🂶 🂹 🂣 🂭 🂼
: 🃅 🂢 🃛 🃕 🂻
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🃉 🂨
: 🂧 🃖 🃇


====================

Move a card from freecell 0 to stack 7

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:      🂸      🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🃃
: 🃜 🂩 🂶 🂹 🂣 🂭 🂼
: 🃅 🂢 🃛 🃕 🂻
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move a card from stack 3 to freecell 2

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:      🂸  🂻  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🃃
: 🃜 🂩 🂶 🂹 🂣 🂭 🂼
: 🃅 🂢 🃛 🃕
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move a card from stack 3 to freecell 0

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  🃕  🂸  🂻  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🃃
: 🃜 🂩 🂶 🂹 🂣 🂭 🂼
: 🃅 🂢 🃛
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move 1 cards from stack 3 to stack 5

Foundations: ♥-5 ♣-A ♦-A ♠-A
Freecells:  🃕  🂸  🂻  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🃃
: 🃜 🂩 🂶 🂹 🂣 🂭 🂼
: 🃅 🂢
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move a card from stack 3 to the foundations

Foundations: ♥-5 ♣-A ♦-A ♠-2
Freecells:  🃕  🂸  🂻  🂤
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🃃
: 🃜 🂩 🂶 🂹 🂣 🂭 🂼
: 🃅
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move a card from freecell 3 to stack 3

Foundations: ♥-5 ♣-A ♦-A ♠-2
Freecells:  🃕  🂸  🂻
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🃃
: 🃜 🂩 🂶 🂹 🂣 🂭 🂼
: 🃅 🂤
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move 1 cards from stack 1 to stack 3

Foundations: ♥-5 ♣-A ♦-A ♠-2
Freecells:  🃕  🂸  🂻
: 🃔 🃒 🃙 🃘 🂬 🃋
:
: 🃜 🂩 🂶 🂹 🂣 🂭 🂼
: 🃅 🂤 🃃
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move a card from stack 2 to freecell 3

Foundations: ♥-5 ♣-A ♦-A ♠-2
Freecells:  🃕  🂸  🂻  🂼
: 🃔 🃒 🃙 🃘 🂬 🃋
:
: 🃜 🂩 🂶 🂹 🂣 🂭
: 🃅 🂤 🃃
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move 1 cards from stack 2 to stack 1

Foundations: ♥-5 ♣-A ♦-A ♠-2
Freecells:  🃕  🂸  🂻  🂼
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭
: 🃜 🂩 🂶 🂹 🂣
: 🃅 🂤 🃃
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move a card from stack 2 to the foundations

Foundations: ♥-5 ♣-A ♦-A ♠-3
Freecells:  🃕  🂸  🂻  🂼
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭
: 🃜 🂩 🂶 🂹
: 🃅 🂤 🃃
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move a card from freecell 3 to stack 1

Foundations: ♥-5 ♣-A ♦-A ♠-3
Freecells:  🃕  🂸  🂻
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂶 🂹
: 🃅 🂤 🃃
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move a card from stack 2 to freecell 3

Foundations: ♥-5 ♣-A ♦-A ♠-3
Freecells:  🃕  🂸  🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂶
: 🃅 🂤 🃃
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move a card from freecell 0 to stack 2

Foundations: ♥-5 ♣-A ♦-A ♠-3
Freecells:      🂸  🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂶 🃕
: 🃅 🂤 🃃
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move a card from stack 3 to freecell 0

Foundations: ♥-5 ♣-A ♦-A ♠-3
Freecells:  🃃  🂸  🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂶 🃕
: 🃅 🂤
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move a card from stack 3 to the foundations

Foundations: ♥-5 ♣-A ♦-A ♠-4
Freecells:  🃃  🂸  🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂶 🃕
: 🃅
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move 1 cards from stack 3 to stack 7

Foundations: ♥-5 ♣-A ♦-A ♠-4
Freecells:  🃃  🂸  🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂶 🃕
:
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦 🃅


====================

Move 1 cards from stack 2 to stack 3

Foundations: ♥-5 ♣-A ♦-A ♠-4
Freecells:  🃃  🂸  🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂶
: 🃕
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦 🃅


====================

Move a card from stack 2 to the foundations

Foundations: ♥-6 ♣-A ♦-A ♠-4
Freecells:  🃃  🂸  🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩
: 🃕
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦 🃅


====================

Move a card from freecell 1 to stack 2

Foundations: ♥-6 ♣-A ♦-A ♠-4
Freecells:  🃃      🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂸
: 🃕
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦 🃅


====================

Move 2 cards from stack 4 to stack 3

Foundations: ♥-6 ♣-A ♦-A ♠-4
Freecells:  🃃      🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂸
: 🃕 🃄 🃓
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🂥
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦 🃅


====================

Move a card from stack 4 to the foundations

Foundations: ♥-6 ♣-A ♦-A ♠-5
Freecells:  🃃      🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂸
: 🃕 🃄 🃓
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦 🃅


====================

Move a card from stack 7 to freecell 1

Foundations: ♥-6 ♣-A ♦-A ♠-5
Freecells:  🃃  🃅  🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂸
: 🃕 🃄 🃓
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇 🂦


====================

Move a card from stack 7 to the foundations

Foundations: ♥-6 ♣-A ♦-A ♠-6
Freecells:  🃃  🃅  🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂸
: 🃕 🃄 🃓
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨
: 🂧 🃖 🃇


====================

Move 1 cards from stack 7 to stack 6

Foundations: ♥-6 ♣-A ♦-A ♠-6
Freecells:  🃃  🃅  🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂸
: 🃕 🃄 🃓
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨 🃇
: 🂧 🃖


====================

Move a card from freecell 1 to stack 7

Foundations: ♥-6 ♣-A ♦-A ♠-6
Freecells:  🃃      🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂸
: 🃕 🃄 🃓
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨 🃇
: 🂧 🃖 🃅


====================

Move 2 cards from stack 7 to stack 6

Foundations: ♥-6 ♣-A ♦-A ♠-6
Freecells:  🃃      🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂸
: 🃕 🃄 🃓
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨 🃇 🃖 🃅
: 🂧


====================

Move a card from stack 7 to the foundations

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃      🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩 🂸
: 🃕 🃄 🃓
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨 🃇 🃖 🃅
:


====================

Move a card from stack 2 to freecell 1

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃  🂸  🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂩
: 🃕 🃄 🃓
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨 🃇 🃖 🃅
:


====================

Move 1 cards from stack 2 to stack 7

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃  🂸  🂻  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜
: 🃕 🃄 🃓
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨 🃇 🃖 🃅
: 🂩


====================

Move a card from freecell 2 to stack 2

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃  🂸      🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂻
: 🃕 🃄 🃓
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨 🃇 🃖 🃅
: 🂩


====================

Move a card from freecell 1 to stack 7

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃          🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂻
: 🃕 🃄 🃓
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move 3 cards from stack 3 to stack 4

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃          🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂻
:
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌 🃛
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move a card from stack 5 to freecell 2

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃      🃛  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂻
:
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝 🃌
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move a card from stack 5 to freecell 1

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃  🃌  🃛  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂻
:
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄 🃓
: 🂷 🂫 🂽 🂪 🃝
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move 1 cards from stack 5 to stack 3

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃  🃌  🃛  🂹
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂻
: 🃝
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄 🃓
: 🂷 🂫 🂽 🂪
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move a card from freecell 3 to stack 5

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃  🃌  🃛
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂻
: 🃝
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄 🃓
: 🂷 🂫 🂽 🂪 🂹
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move a card from freecell 1 to stack 3

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃      🃛
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄 🃓
: 🂷 🂫 🂽 🂪 🂹
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move a card from freecell 2 to stack 3

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃
: 🃔 🃒 🃙 🃘 🂬 🃋
: 🂭 🂼
: 🃜 🂻
: 🃝 🃌 🃛
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄 🃓
: 🂷 🂫 🂽 🂪 🂹
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move a card from stack 0 to freecell 3

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃          🃋
: 🃔 🃒 🃙 🃘 🂬
: 🂭 🂼
: 🃜 🂻
: 🃝 🃌 🃛
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄 🃓
: 🂷 🂫 🂽 🂪 🂹
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move a card from stack 0 to freecell 1

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃  🂬      🃋
: 🃔 🃒 🃙 🃘
: 🂭 🂼
: 🃜 🂻
: 🃝 🃌 🃛
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄 🃓
: 🂷 🂫 🂽 🂪 🂹
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move 1 cards from stack 0 to stack 5

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃  🂬      🃋
: 🃔 🃒 🃙
: 🂭 🂼
: 🃜 🂻
: 🃝 🃌 🃛
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄 🃓
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move a card from stack 0 to freecell 2

Foundations: ♥-6 ♣-A ♦-A ♠-7
Freecells:  🃃  🂬  🃙  🃋
: 🃔 🃒
: 🂭 🂼
: 🃜 🂻
: 🃝 🃌 🃛
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄 🃓
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move a card from stack 0 to the foundations

Foundations: ♥-6 ♣-2 ♦-A ♠-7
Freecells:  🃃  🂬  🃙  🃋
: 🃔
: 🂭 🂼
: 🃜 🂻
: 🃝 🃌 🃛
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄 🃓
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move a card from stack 4 to the foundations

Foundations: ♥-6 ♣-3 ♦-A ♠-7
Freecells:  🃃  🂬  🃙  🃋
: 🃔
: 🂭 🂼
: 🃜 🂻
: 🃝 🃌 🃛
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move a card from stack 0 to the foundations

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  🃃  🂬  🃙  🃋
:
: 🂭 🂼
: 🃜 🂻
: 🃝 🃌 🃛
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move a card from freecell 0 to stack 0

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:      🂬  🃙  🃋
: 🃃
: 🂭 🂼
: 🃜 🂻
: 🃝 🃌 🃛
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move 1 cards from stack 3 to stack 1

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:      🂬  🃙  🃋
: 🃃
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move a card from stack 0 to freecell 0

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  🃃  🂬  🃙  🃋
:
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move a card from freecell 2 to stack 0

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  🃃  🂬      🃋
: 🃙
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂩 🂸


====================

Move 1 cards from stack 7 to stack 0

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  🃃  🂬      🃋
: 🃙 🂸
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂩


====================

Move 1 cards from stack 1 to stack 3

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  🃃  🂬      🃋
: 🃙 🂸
: 🂭 🂼
: 🃜 🂻
: 🃝 🃌 🃛
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂩


====================

Move a card from stack 7 to freecell 2

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  🃃  🂬  🂩  🃋
: 🃙 🂸
: 🂭 🂼
: 🃜 🂻
: 🃝 🃌 🃛
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
:


====================

Move a card from freecell 0 to stack 7

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:      🂬  🂩  🃋
: 🃙 🂸
: 🂭 🂼
: 🃜 🂻
: 🃝 🃌 🃛
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🃃


====================

Move 1 cards from stack 3 to stack 1

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:      🂬  🂩  🃋
: 🃙 🂸
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🃃


====================

Move a card from stack 7 to freecell 0

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  🃃  🂬  🂩  🃋
: 🃙 🂸
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
:


====================

Move a card from freecell 1 to stack 7

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  🃃      🂩  🃋
: 🃙 🂸
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂬


====================

Move a card from freecell 3 to stack 7

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  🃃      🂩
: 🃙 🂸
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕 🃄
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂬 🃋


====================

Move a card from stack 4 to freecell 1

Foundations: ♥-6 ♣-4 ♦-A ♠-7
Freecells:  🃃  🃄  🂩
: 🃙 🂸
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆 🃕
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂬 🃋


====================

Move a card from stack 4 to the foundations

Foundations: ♥-6 ♣-5 ♦-A ♠-7
Freecells:  🃃  🃄  🂩
: 🃙 🂸
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖 🃅
: 🂬 🃋


====================

Move a card from stack 6 to freecell 3

Foundations: ♥-6 ♣-5 ♦-A ♠-7
Freecells:  🃃  🃄  🂩  🃅
: 🃙 🂸
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇 🃖
: 🂬 🃋


====================

Move a card from stack 6 to the foundations

Foundations: ♥-6 ♣-6 ♦-A ♠-7
Freecells:  🃃  🃄  🂩  🃅
: 🃙 🂸
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🂹 🃘
: 🃉 🂨 🃇
: 🂬 🃋


====================

Move 1 cards from stack 6 to stack 5

Foundations: ♥-6 ♣-6 ♦-A ♠-7
Freecells:  🃃  🃄  🂩  🃅
: 🃙 🂸
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🂹 🃘 🃇
: 🃉 🂨
: 🂬 🃋


====================

Move a card from stack 6 to the foundations

Foundations: ♥-6 ♣-6 ♦-A ♠-8
Freecells:  🃃  🃄  🂩  🃅
: 🃙 🂸
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🂹 🃘 🃇
: 🃉
: 🂬 🃋


====================

Move a card from freecell 2 to the foundations

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  🃃  🃄      🃅
: 🃙 🂸
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈 🃗 🃆
: 🂷 🂫 🂽 🂪 🂹 🃘 🃇
: 🃉
: 🂬 🃋


====================

Move 2 cards from stack 4 to stack 0

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  🃃  🃄      🃅
: 🃙 🂸 🃗 🃆
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊 🃈
: 🂷 🂫 🂽 🂪 🂹 🃘 🃇
: 🃉
: 🂬 🃋


====================

Move a card from stack 4 to freecell 2

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  🃃  🃄  🃈  🃅
: 🃙 🂸 🃗 🃆
: 🂭 🂼 🃛
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚 🃊
: 🂷 🂫 🂽 🂪 🂹 🃘 🃇
: 🃉
: 🂬 🃋


====================

Move 1 cards from stack 4 to stack 1

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  🃃  🃄  🃈  🃅
: 🃙 🂸 🃗 🃆
: 🂭 🂼 🃛 🃊
: 🃜 🂻
: 🃝 🃌
: 🃂 🃍 🂺 🃚
: 🂷 🂫 🂽 🂪 🂹 🃘 🃇
: 🃉
: 🂬 🃋


====================

Move 1 cards from stack 4 to stack 2

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  🃃  🃄  🃈  🃅
: 🃙 🂸 🃗 🃆
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚
: 🃝 🃌
: 🃂 🃍 🂺
: 🂷 🂫 🂽 🂪 🂹 🃘 🃇
: 🃉
: 🂬 🃋


====================

Move 1 cards from stack 6 to stack 2

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  🃃  🃄  🃈  🃅
: 🃙 🂸 🃗 🃆
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉
: 🃝 🃌
: 🃂 🃍 🂺
: 🂷 🂫 🂽 🂪 🂹 🃘 🃇
:
: 🂬 🃋


====================

Move a card from freecell 0 to stack 6

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:      🃄  🃈  🃅
: 🃙 🂸 🃗 🃆
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉
: 🃝 🃌
: 🃂 🃍 🂺
: 🂷 🂫 🂽 🂪 🂹 🃘 🃇
: 🃃
: 🂬 🃋


====================

Move 2 cards from stack 5 to stack 2

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:      🃄  🃈  🃅
: 🃙 🂸 🃗 🃆
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌
: 🃂 🃍 🂺
: 🂷 🂫 🂽 🂪 🂹
: 🃃
: 🂬 🃋


====================

Move 2 cards from stack 5 to stack 7

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:      🃄  🃈  🃅
: 🃙 🂸 🃗 🃆
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌
: 🃂 🃍 🂺
: 🂷 🂫 🂽
: 🃃
: 🂬 🃋 🂪 🂹


====================

Move a card from stack 5 to freecell 0

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  🂽  🃄  🃈  🃅
: 🃙 🂸 🃗 🃆
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌
: 🃂 🃍 🂺
: 🂷 🂫
: 🃃
: 🂬 🃋 🂪 🂹


====================

Move 1 cards from stack 5 to stack 3

Foundations: ♥-6 ♣-6 ♦-A ♠-9
Freecells:  🂽  🃄  🃈  🃅
: 🃙 🂸 🃗 🃆
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌 🂫
: 🃂 🃍 🂺
: 🂷
: 🃃
: 🂬 🃋 🂪 🂹


====================

Move a card from stack 5 to the foundations

Foundations: ♥-7 ♣-6 ♦-A ♠-9
Freecells:  🂽  🃄  🃈  🃅
: 🃙 🂸 🃗 🃆
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌 🂫
: 🃂 🃍 🂺
:
: 🃃
: 🂬 🃋 🂪 🂹


====================

Move 1 cards from stack 0 to stack 5

Foundations: ♥-7 ♣-6 ♦-A ♠-9
Freecells:  🂽  🃄  🃈  🃅
: 🃙 🂸 🃗
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌 🂫
: 🃂 🃍 🂺
: 🃆
: 🃃
: 🂬 🃋 🂪 🂹


====================

Move a card from stack 0 to the foundations

Foundations: ♥-7 ♣-7 ♦-A ♠-9
Freecells:  🂽  🃄  🃈  🃅
: 🃙 🂸
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌 🂫
: 🃂 🃍 🂺
: 🃆
: 🃃
: 🂬 🃋 🂪 🂹


====================

Move a card from stack 0 to the foundations

Foundations: ♥-8 ♣-7 ♦-A ♠-9
Freecells:  🂽  🃄  🃈  🃅
: 🃙
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌 🂫
: 🃂 🃍 🂺
: 🃆
: 🃃
: 🂬 🃋 🂪 🂹


====================

Move a card from stack 7 to the foundations

Foundations: ♥-9 ♣-7 ♦-A ♠-9
Freecells:  🂽  🃄  🃈  🃅
: 🃙
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌 🂫
: 🃂 🃍 🂺
: 🃆
: 🃃
: 🂬 🃋 🂪


====================

Move a card from stack 4 to the foundations

Foundations: ♥-T ♣-7 ♦-A ♠-9
Freecells:  🂽  🃄  🃈  🃅
: 🃙
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌 🂫
: 🃂 🃍
: 🃆
: 🃃
: 🂬 🃋 🂪


====================

Move a card from stack 7 to the foundations

Foundations: ♥-T ♣-7 ♦-A ♠-T
Freecells:  🂽  🃄  🃈  🃅
: 🃙
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌 🂫
: 🃂 🃍
: 🃆
: 🃃
: 🂬 🃋


====================

Move a card from stack 3 to the foundations

Foundations: ♥-T ♣-7 ♦-A ♠-J
Freecells:  🂽  🃄  🃈  🃅
: 🃙
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌
: 🃂 🃍
: 🃆
: 🃃
: 🂬 🃋


====================

Move a card from freecell 2 to stack 0

Foundations: ♥-T ♣-7 ♦-A ♠-J
Freecells:  🂽  🃄      🃅
: 🃙 🃈
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌
: 🃂 🃍
: 🃆
: 🃃
: 🂬 🃋


====================

Move a card from stack 4 to freecell 2

Foundations: ♥-T ♣-7 ♦-A ♠-J
Freecells:  🂽  🃄  🃍  🃅
: 🃙 🃈
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌
: 🃂
: 🃆
: 🃃
: 🂬 🃋


====================

Move a card from stack 4 to the foundations

Foundations: ♥-T ♣-7 ♦-2 ♠-J
Freecells:  🂽  🃄  🃍  🃅
: 🃙 🃈
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌
:
: 🃆
: 🃃
: 🂬 🃋


====================

Move a card from stack 6 to the foundations

Foundations: ♥-T ♣-7 ♦-3 ♠-J
Freecells:  🂽  🃄  🃍  🃅
: 🃙 🃈
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌
:
: 🃆
:
: 🂬 🃋


====================

Move a card from freecell 1 to the foundations

Foundations: ♥-T ♣-7 ♦-4 ♠-J
Freecells:  🂽      🃍  🃅
: 🃙 🃈
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌
:
: 🃆
:
: 🂬 🃋


====================

Move a card from freecell 3 to the foundations

Foundations: ♥-T ♣-7 ♦-5 ♠-J
Freecells:  🂽      🃍
: 🃙 🃈
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌
:
: 🃆
:
: 🂬 🃋


====================

Move a card from stack 5 to the foundations

Foundations: ♥-T ♣-7 ♦-6 ♠-J
Freecells:  🂽      🃍
: 🃙 🃈
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘 🃇
: 🃝 🃌
:
:
:
: 🂬 🃋


====================

Move a card from stack 2 to the foundations

Foundations: ♥-T ♣-7 ♦-7 ♠-J
Freecells:  🂽      🃍
: 🃙 🃈
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘
: 🃝 🃌
:
:
:
: 🂬 🃋


====================

Move a card from stack 0 to the foundations

Foundations: ♥-T ♣-7 ♦-8 ♠-J
Freecells:  🂽      🃍
: 🃙
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉 🃘
: 🃝 🃌
:
:
:
: 🂬 🃋


====================

Move a card from stack 2 to the foundations

Foundations: ♥-T ♣-8 ♦-8 ♠-J
Freecells:  🂽      🃍
: 🃙
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉
: 🃝 🃌
:
:
:
: 🂬 🃋


====================

Move a card from stack 0 to the foundations

Foundations: ♥-T ♣-9 ♦-8 ♠-J
Freecells:  🂽      🃍
:
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚 🃉
: 🃝 🃌
:
:
:
: 🂬 🃋


====================

Move a card from stack 2 to the foundations

Foundations: ♥-T ♣-9 ♦-9 ♠-J
Freecells:  🂽      🃍
:
: 🂭 🂼 🃛 🃊
: 🃜 🂻 🃚
: 🃝 🃌
:
:
:
: 🂬 🃋


====================

Move a card from stack 2 to the foundations

Foundations: ♥-T ♣-T ♦-9 ♠-J
Freecells:  🂽      🃍
:
: 🂭 🂼 🃛 🃊
: 🃜 🂻
: 🃝 🃌
:
:
:
: 🂬 🃋


====================

Move a card from stack 2 to the foundations

Foundations: ♥-J ♣-T ♦-9 ♠-J
Freecells:  🂽      🃍
:
: 🂭 🂼 🃛 🃊
: 🃜
: 🃝 🃌
:
:
:
: 🂬 🃋


====================

Move a card from stack 1 to the foundations

Foundations: ♥-J ♣-T ♦-T ♠-J
Freecells:  🂽      🃍
:
: 🂭 🂼 🃛
: 🃜
: 🃝 🃌
:
:
:
: 🂬 🃋


====================

Move a card from stack 7 to the foundations

Foundations: ♥-J ♣-T ♦-J ♠-J
Freecells:  🂽      🃍
:
: 🂭 🂼 🃛
: 🃜
: 🃝 🃌
:
:
:
: 🂬


====================

Move a card from stack 7 to the foundations

Foundations: ♥-J ♣-T ♦-J ♠-Q
Freecells:  🂽      🃍
:
: 🂭 🂼 🃛
: 🃜
: 🃝 🃌
:
:
:
:


====================

Move a card from stack 3 to the foundations

Foundations: ♥-J ♣-T ♦-Q ♠-Q
Freecells:  🂽      🃍
:
: 🂭 🂼 🃛
: 🃜
: 🃝
:
:
:
:


====================

Move a card from stack 1 to the foundations

Foundations: ♥-J ♣-J ♦-Q ♠-Q
Freecells:  🂽      🃍
:
: 🂭 🂼
: 🃜
: 🃝
:
:
:
:


====================

Move a card from stack 2 to the foundations

Foundations: ♥-J ♣-Q ♦-Q ♠-Q
Freecells:  🂽      🃍
:
: 🂭 🂼
:
: 🃝
:
:
:
:


====================

Move a card from stack 3 to the foundations

Foundations: ♥-J ♣-K ♦-Q ♠-Q
Freecells:  🂽      🃍
:
: 🂭 🂼
:
:
:
:
:
:


====================

Move a card from stack 1 to the foundations

Foundations: ♥-Q ♣-K ♦-Q ♠-Q
Freecells:  🂽      🃍
:
: 🂭
:
:
:
:
:
:


====================

Move a card from stack 1 to the foundations

Foundations: ♥-Q ♣-K ♦-Q ♠-K
Freecells:  🂽      🃍
:
:
:
:
:
:
:
:


====================

Move a card from freecell 0 to the foundations

Foundations: ♥-K ♣-K ♦-Q ♠-K
Freecells:          🃍
:
:
:
:
:
:
:
:


====================

Move a card from freecell 2 to the foundations

Foundations: ♥-K ♣-K ♦-K ♠-K
Freecells:
:
:
:
:
:
:
:
:


====================

`
    ;

    const pysol_simple_simon_deal_24 = `4C QH 3C 8C AD JH 8S KS
5H 9S 6H AC 4D TD 4S 6D
QC 2S JC 9H QS KC 4H 8D
5D KD TH 5C 3H 8H 7C
2D JS KH TC 3S JD
7H 5S 6S TS 9D
AH 6C 7D 2H
7S 9C QD
2C 3D
AS
`;

    const solution_for_pysol_simple_simon_deal_24__default =
`-=-=-=-=-=-=-=-=-=-=-=-

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS
: 5H 9S 6H AC 4D TD 4S 6D
: QC 2S JC 9H QS KC 4H 8D
: 5D KD TH 5C 3H 8H 7C
: 2D JS KH TC 3S JD
: 7H 5S 6S TS 9D
: AH 6C 7D 2H
: 7S 9C QD
: 2C 3D
: AS


====================

Move 1 cards from stack 4 to stack 7

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS
: 5H 9S 6H AC 4D TD 4S 6D
: QC 2S JC 9H QS KC 4H 8D
: 5D KD TH 5C 3H 8H 7C
: 2D JS KH TC 3S
: 7H 5S 6S TS 9D
: AH 6C 7D 2H
: 7S 9C QD JD
: 2C 3D
: AS


====================

Move 1 cards from stack 2 to stack 5

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS
: 5H 9S 6H AC 4D TD 4S 6D
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 8H 7C
: 2D JS KH TC 3S
: 7H 5S 6S TS 9D 8D
: AH 6C 7D 2H
: 7S 9C QD JD
: 2C 3D
: AS


====================

Move 1 cards from stack 9 to stack 6

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS
: 5H 9S 6H AC 4D TD 4S 6D
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 8H 7C
: 2D JS KH TC 3S
: 7H 5S 6S TS 9D 8D
: AH 6C 7D 2H AS
: 7S 9C QD JD
: 2C 3D
:


====================

Move 1 cards from stack 1 to stack 3

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS
: 5H 9S 6H AC 4D TD 4S
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 8H 7C 6D
: 2D JS KH TC 3S
: 7H 5S 6S TS 9D 8D
: AH 6C 7D 2H AS
: 7S 9C QD JD
: 2C 3D
:


====================

Move 1 cards from stack 4 to stack 1

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS
: 5H 9S 6H AC 4D TD 4S 3S
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 8H 7C 6D
: 2D JS KH TC
: 7H 5S 6S TS 9D 8D
: AH 6C 7D 2H AS
: 7S 9C QD JD
: 2C 3D
:


====================

Move 3 cards from stack 5 to stack 7

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS
: 5H 9S 6H AC 4D TD 4S 3S
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 8H 7C 6D
: 2D JS KH TC
: 7H 5S 6S
: AH 6C 7D 2H AS
: 7S 9C QD JD TS 9D 8D
: 2C 3D
:


====================

Move 1 cards from stack 5 to stack 9

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS
: 5H 9S 6H AC 4D TD 4S 3S
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 8H 7C 6D
: 2D JS KH TC
: 7H 5S
: AH 6C 7D 2H AS
: 7S 9C QD JD TS 9D 8D
: 2C 3D
: 6S


====================

Move 2 cards from stack 1 to stack 5

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS
: 5H 9S 6H AC 4D TD
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 8H 7C 6D
: 2D JS KH TC
: 7H 5S 4S 3S
: AH 6C 7D 2H AS
: 7S 9C QD JD TS 9D 8D
: 2C 3D
: 6S


====================

Move 3 cards from stack 5 to stack 9

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS
: 5H 9S 6H AC 4D TD
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 8H 7C 6D
: 2D JS KH TC
: 7H
: AH 6C 7D 2H AS
: 7S 9C QD JD TS 9D 8D
: 2C 3D
: 6S 5S 4S 3S


====================

Move 2 cards from stack 7 to stack 1

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS
: 5H 9S 6H AC 4D TD 9D 8D
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 8H 7C 6D
: 2D JS KH TC
: 7H
: AH 6C 7D 2H AS
: 7S 9C QD JD TS
: 2C 3D
: 6S 5S 4S 3S


====================

Move 4 cards from stack 9 to stack 5

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS
: 5H 9S 6H AC 4D TD 9D 8D
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 8H 7C 6D
: 2D JS KH TC
: 7H 6S 5S 4S 3S
: AH 6C 7D 2H AS
: 7S 9C QD JD TS
: 2C 3D
:


====================

Move 5 cards from stack 5 to stack 1

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS
: 5H 9S 6H AC 4D TD 9D 8D 7H 6S 5S 4S 3S
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 8H 7C 6D
: 2D JS KH TC
:
: AH 6C 7D 2H AS
: 7S 9C QD JD TS
: 2C 3D
:


====================

Move 3 cards from stack 7 to stack 0

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D TD 9D 8D 7H 6S 5S 4S 3S
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 8H 7C 6D
: 2D JS KH TC
:
: AH 6C 7D 2H AS
: 7S 9C
: 2C 3D
:


====================

Move 1 cards from stack 7 to stack 4

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D TD 9D 8D 7H 6S 5S 4S 3S
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 8H 7C 6D
: 2D JS KH TC 9C
:
: AH 6C 7D 2H AS
: 7S
: 2C 3D
:


====================

Move 4 cards from stack 1 to stack 7

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D TD 9D 8D 7H
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 8H 7C 6D
: 2D JS KH TC 9C
:
: AH 6C 7D 2H AS
: 7S 6S 5S 4S 3S
: 2C 3D
:


====================

Move 3 cards from stack 3 to stack 4

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D TD 9D 8D 7H
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H
: 2D JS KH TC 9C 8H 7C 6D
:
: AH 6C 7D 2H AS
: 7S 6S 5S 4S 3S
: 2C 3D
:


====================

Move 2 cards from stack 6 to stack 3

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D TD 9D 8D 7H
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 2H AS
: 2D JS KH TC 9C 8H 7C 6D
:
: AH 6C 7D
: 7S 6S 5S 4S 3S
: 2C 3D
:


====================

Move 1 cards from stack 4 to stack 6

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D TD 9D 8D 7H
: QC 2S JC 9H QS KC 4H
: 5D KD TH 5C 3H 2H AS
: 2D JS KH TC 9C 8H 7C
:
: AH 6C 7D 6D
: 7S 6S 5S 4S 3S
: 2C 3D
:


====================

Move 3 cards from stack 3 to stack 2

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D TD 9D 8D 7H
: QC 2S JC 9H QS KC 4H 3H 2H AS
: 5D KD TH 5C
: 2D JS KH TC 9C 8H 7C
:
: AH 6C 7D 6D
: 7S 6S 5S 4S 3S
: 2C 3D
:


====================

Move 1 cards from stack 1 to stack 9

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D TD 9D 8D
: QC 2S JC 9H QS KC 4H 3H 2H AS
: 5D KD TH 5C
: 2D JS KH TC 9C 8H 7C
:
: AH 6C 7D 6D
: 7S 6S 5S 4S 3S
: 2C 3D
: 7H


====================

Move 2 cards from stack 6 to stack 1

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D TD 9D 8D 7D 6D
: QC 2S JC 9H QS KC 4H 3H 2H AS
: 5D KD TH 5C
: 2D JS KH TC 9C 8H 7C
:
: AH 6C
: 7S 6S 5S 4S 3S
: 2C 3D
: 7H


====================

Move 1 cards from stack 6 to stack 4

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D TD 9D 8D 7D 6D
: QC 2S JC 9H QS KC 4H 3H 2H AS
: 5D KD TH 5C
: 2D JS KH TC 9C 8H 7C 6C
:
: AH
: 7S 6S 5S 4S 3S
: 2C 3D
: 7H


====================

Move 1 cards from stack 3 to stack 4

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D TD 9D 8D 7D 6D
: QC 2S JC 9H QS KC 4H 3H 2H AS
: 5D KD TH
: 2D JS KH TC 9C 8H 7C 6C 5C
:
: AH
: 7S 6S 5S 4S 3S
: 2C 3D
: 7H


====================

Move 1 cards from stack 2 to stack 5

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D TD 9D 8D 7D 6D
: QC 2S JC 9H QS KC 4H 3H 2H
: 5D KD TH
: 2D JS KH TC 9C 8H 7C 6C 5C
: AS
: AH
: 7S 6S 5S 4S 3S
: 2C 3D
: 7H


====================

Move 1 cards from stack 6 to stack 2

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D TD 9D 8D 7D 6D
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7C 6C 5C
: AS
:
: 7S 6S 5S 4S 3S
: 2C 3D
: 7H


====================

Move 5 cards from stack 1 to stack 6

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7C 6C 5C
: AS
: TD 9D 8D 7D 6D
: 7S 6S 5S 4S 3S
: 2C 3D
: 7H


====================

Move 1 cards from stack 8 to stack 1

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D 3D
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7C 6C 5C
: AS
: TD 9D 8D 7D 6D
: 7S 6S 5S 4S 3S
: 2C
: 7H


====================

Move 1 cards from stack 5 to stack 8

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D 3D
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7C 6C 5C
:
: TD 9D 8D 7D 6D
: 7S 6S 5S 4S 3S
: 2C AS
: 7H


====================

Move 2 cards from stack 8 to stack 1

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D 3D 2C AS
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7C 6C 5C
:
: TD 9D 8D 7D 6D
: 7S 6S 5S 4S 3S
:
: 7H


====================

Move 3 cards from stack 4 to stack 5

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D 3D 2C AS
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H
: 7C 6C 5C
: TD 9D 8D 7D 6D
: 7S 6S 5S 4S 3S
:
: 7H


====================

Move 1 cards from stack 9 to stack 4

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TS
: 5H 9S 6H AC 4D 3D 2C AS
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7H
: 7C 6C 5C
: TD 9D 8D 7D 6D
: 7S 6S 5S 4S 3S
:
:


====================

Move 1 cards from stack 0 to stack 8

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD
: 5H 9S 6H AC 4D 3D 2C AS
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7H
: 7C 6C 5C
: TD 9D 8D 7D 6D
: 7S 6S 5S 4S 3S
: TS
:


====================

Move 5 cards from stack 6 to stack 0

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S KS QD JD TD 9D 8D 7D 6D
: 5H 9S 6H AC 4D 3D 2C AS
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7H
: 7C 6C 5C
:
: 7S 6S 5S 4S 3S
: TS
:


====================

Move 8 cards from stack 0 to stack 9

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S
: 5H 9S 6H AC 4D 3D 2C AS
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7H
: 7C 6C 5C
:
: 7S 6S 5S 4S 3S
: TS
: KS QD JD TD 9D 8D 7D 6D


====================

Move 5 cards from stack 7 to stack 0

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S 7S 6S 5S 4S 3S
: 5H 9S 6H AC 4D 3D 2C AS
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7H
: 7C 6C 5C
:
:
: TS
: KS QD JD TD 9D 8D 7D 6D


====================

Move 4 cards from stack 1 to stack 5

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S 7S 6S 5S 4S 3S
: 5H 9S 6H AC
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7H
: 7C 6C 5C 4D 3D 2C AS
:
:
: TS
: KS QD JD TD 9D 8D 7D 6D


====================

Move 1 cards from stack 1 to stack 6

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S 7S 6S 5S 4S 3S
: 5H 9S 6H
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7H
: 7C 6C 5C 4D 3D 2C AS
: AC
:
: TS
: KS QD JD TD 9D 8D 7D 6D


====================

Move 1 cards from stack 1 to stack 4

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH 8S 7S 6S 5S 4S 3S
: 5H 9S
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7H 6H
: 7C 6C 5C 4D 3D 2C AS
: AC
:
: TS
: KS QD JD TD 9D 8D 7D 6D


====================

Move 6 cards from stack 0 to stack 1

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH
: 5H 9S 8S 7S 6S 5S 4S 3S
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7H 6H
: 7C 6C 5C 4D 3D 2C AS
: AC
:
: TS
: KS QD JD TD 9D 8D 7D 6D


====================

Move 7 cards from stack 1 to stack 8

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH
: 5H
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7H 6H
: 7C 6C 5C 4D 3D 2C AS
: AC
:
: TS 9S 8S 7S 6S 5S 4S 3S
: KS QD JD TD 9D 8D 7D 6D


====================

Move 1 cards from stack 1 to stack 4

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH
:
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD TH
: 2D JS KH TC 9C 8H 7H 6H 5H
: 7C 6C 5C 4D 3D 2C AS
: AC
:
: TS 9S 8S 7S 6S 5S 4S 3S
: KS QD JD TD 9D 8D 7D 6D


====================

Move 1 cards from stack 3 to stack 0

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH
:
: QC 2S JC 9H QS KC 4H 3H 2H AH
: 5D KD
: 2D JS KH TC 9C 8H 7H 6H 5H
: 7C 6C 5C 4D 3D 2C AS
: AC
:
: TS 9S 8S 7S 6S 5S 4S 3S
: KS QD JD TD 9D 8D 7D 6D


====================

Move 4 cards from stack 2 to stack 4

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH
:
: QC 2S JC 9H QS KC
: 5D KD
: 2D JS KH TC 9C 8H 7H 6H 5H 4H 3H 2H AH
: 7C 6C 5C 4D 3D 2C AS
: AC
:
: TS 9S 8S 7S 6S 5S 4S 3S
: KS QD JD TD 9D 8D 7D 6D


====================

Move 7 cards from stack 9 to stack 3

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH
:
: QC 2S JC 9H QS KC
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH TC 9C 8H 7H 6H 5H 4H 3H 2H AH
: 7C 6C 5C 4D 3D 2C AS
: AC
:
: TS 9S 8S 7S 6S 5S 4S 3S
: KS


====================

Move 1 cards from stack 5 to stack 7

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH
:
: QC 2S JC 9H QS KC
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH TC 9C 8H 7H 6H 5H 4H 3H 2H AH
: 7C 6C 5C 4D 3D 2C
: AC
: AS
: TS 9S 8S 7S 6S 5S 4S 3S
: KS


====================

Move 1 cards from stack 6 to stack 5

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH
:
: QC 2S JC 9H QS KC
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH TC 9C 8H 7H 6H 5H 4H 3H 2H AH
: 7C 6C 5C 4D 3D 2C AC
:
: AS
: TS 9S 8S 7S 6S 5S 4S 3S
: KS


====================

Move 1 cards from stack 2 to stack 1

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH
: KC
: QC 2S JC 9H QS
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH TC 9C 8H 7H 6H 5H 4H 3H 2H AH
: 7C 6C 5C 4D 3D 2C AC
:
: AS
: TS 9S 8S 7S 6S 5S 4S 3S
: KS


====================

Move 1 cards from stack 2 to stack 9

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH
: KC
: QC 2S JC 9H
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH TC 9C 8H 7H 6H 5H 4H 3H 2H AH
: 7C 6C 5C 4D 3D 2C AC
:
: AS
: TS 9S 8S 7S 6S 5S 4S 3S
: KS QS


====================

Move 8 cards from stack 4 to stack 2

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH
: KC
: QC 2S JC 9H 8H 7H 6H 5H 4H 3H 2H AH
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH TC 9C
: 7C 6C 5C 4D 3D 2C AC
:
: AS
: TS 9S 8S 7S 6S 5S 4S 3S
: KS QS


====================

Move 9 cards from stack 2 to stack 0

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH
: KC
: QC 2S JC
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH TC 9C
: 7C 6C 5C 4D 3D 2C AC
:
: AS
: TS 9S 8S 7S 6S 5S 4S 3S
: KS QS


====================

Move 2 cards from stack 4 to stack 2

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH
: KC
: QC 2S JC TC 9C
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH
: 7C 6C 5C 4D 3D 2C AC
:
: AS
: TS 9S 8S 7S 6S 5S 4S 3S
: KS QS


====================

Move 3 cards from stack 2 to stack 9

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH
: KC
: QC 2S
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH
: 7C 6C 5C 4D 3D 2C AC
:
: AS
: TS 9S 8S 7S 6S 5S 4S 3S
: KS QS JC TC 9C


====================

Move 1 cards from stack 7 to stack 2

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH
: KC
: QC 2S AS
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH
: 7C 6C 5C 4D 3D 2C AC
:
:
: TS 9S 8S 7S 6S 5S 4S 3S
: KS QS JC TC 9C


====================

Move 2 cards from stack 2 to stack 8

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH
: KC
: QC
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH
: 7C 6C 5C 4D 3D 2C AC
:
:
: TS 9S 8S 7S 6S 5S 4S 3S 2S AS
: KS QS JC TC 9C


====================

Move 1 cards from stack 2 to stack 1

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH
: KC QC
:
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH
: 7C 6C 5C 4D 3D 2C AC
:
:
: TS 9S 8S 7S 6S 5S 4S 3S 2S AS
: KS QS JC TC 9C


====================

Move 3 cards from stack 9 to stack 1

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH
: KC QC JC TC 9C
:
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH
: 7C 6C 5C 4D 3D 2C AC
:
:
: TS 9S 8S 7S 6S 5S 4S 3S 2S AS
: KS QS


====================

Move 11 cards from stack 0 to stack 9

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C AD
: KC QC JC TC 9C
:
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH
: 7C 6C 5C 4D 3D 2C AC
:
:
: TS 9S 8S 7S 6S 5S 4S 3S 2S AS
: KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH


====================

Move 1 cards from stack 0 to stack 6

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 8C
: KC QC JC TC 9C
:
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH
: 7C 6C 5C 4D 3D 2C AC
: AD
:
: TS 9S 8S 7S 6S 5S 4S 3S 2S AS
: KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH


====================

Move 1 cards from stack 0 to stack 1

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C
: KC QC JC TC 9C 8C
:
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH
: 7C 6C 5C 4D 3D 2C AC
: AD
:
: TS 9S 8S 7S 6S 5S 4S 3S 2S AS
: KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH


====================

Move 2 cards from stack 5 to stack 0

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 2C AC
: KC QC JC TC 9C 8C
:
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH
: 7C 6C 5C 4D 3D
: AD
:
: TS 9S 8S 7S 6S 5S 4S 3S 2S AS
: KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH


====================

Move 5 cards from stack 5 to stack 1

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 2C AC
: KC QC JC TC 9C 8C 7C 6C 5C 4D 3D
:
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS KH
:
: AD
:
: TS 9S 8S 7S 6S 5S 4S 3S 2S AS
: KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH


====================

Move 1 cards from stack 4 to stack 7

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 2C AC
: KC QC JC TC 9C 8C 7C 6C 5C 4D 3D
:
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D JS
:
: AD
: KH
: TS 9S 8S 7S 6S 5S 4S 3S 2S AS
: KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH


====================

Move 1 cards from stack 4 to stack 2

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 2C AC
: KC QC JC TC 9C 8C 7C 6C 5C 4D 3D
: JS
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D
:
: AD
: KH
: TS 9S 8S 7S 6S 5S 4S 3S 2S AS
: KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH


====================

Move 1 cards from stack 6 to stack 4

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 2C AC
: KC QC JC TC 9C 8C 7C 6C 5C 4D 3D
: JS
: 5D KD QD JD TD 9D 8D 7D 6D
: 2D AD
:
:
: KH
: TS 9S 8S 7S 6S 5S 4S 3S 2S AS
: KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH


====================

Move 2 cards from stack 4 to stack 1

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 2C AC
: KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD
: JS
: 5D KD QD JD TD 9D 8D 7D 6D
:
:
:
: KH
: TS 9S 8S 7S 6S 5S 4S 3S 2S AS
: KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH


====================

Move 10 cards from stack 8 to stack 2

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 2C AC
: KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD
: JS TS 9S 8S 7S 6S 5S 4S 3S 2S AS
: 5D KD QD JD TD 9D 8D 7D 6D
:
:
:
: KH
:
: KS QS JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH


====================

Move 11 cards from stack 9 to stack 5

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 2C AC
: KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD
: JS TS 9S 8S 7S 6S 5S 4S 3S 2S AS
: 5D KD QD JD TD 9D 8D 7D 6D
:
: JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH
:
: KH
:
: KS QS


====================

Move 11 cards from stack 2 to stack 9

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C QH 3C 2C AC
: KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD
:
: 5D KD QD JD TD 9D 8D 7D 6D
:
: JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH
:
: KH
:
: KS QS JS TS 9S 8S 7S 6S 5S 4S 3S 2S AS


====================

Move the sequence on top of Stack 9 to the foundations

Foundations: H-0 C-0 D-0 S-K
Freecells:
: 4C QH 3C 2C AC
: KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD
:
: 5D KD QD JD TD 9D 8D 7D 6D
:
: JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH
:
: KH
:
:


====================

Move 3 cards from stack 0 to stack 6

Foundations: H-0 C-0 D-0 S-K
Freecells:
: 4C QH
: KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD
:
: 5D KD QD JD TD 9D 8D 7D 6D
:
: JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH
: 3C 2C AC
: KH
:
:


====================

Move 11 cards from stack 5 to stack 0

Foundations: H-0 C-0 D-0 S-K
Freecells:
: 4C QH JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH
: KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD
:
: 5D KD QD JD TD 9D 8D 7D 6D
:
:
: 3C 2C AC
: KH
:
:


====================

Move 12 cards from stack 0 to stack 7

Foundations: H-0 C-0 D-0 S-K
Freecells:
: 4C
: KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD
:
: 5D KD QD JD TD 9D 8D 7D 6D
:
:
: 3C 2C AC
: KH QH JH TH 9H 8H 7H 6H 5H 4H 3H 2H AH
:
:


====================

Move the sequence on top of Stack 7 to the foundations

Foundations: H-K C-0 D-0 S-K
Freecells:
: 4C
: KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD
:
: 5D KD QD JD TD 9D 8D 7D 6D
:
:
: 3C 2C AC
:
:
:


====================

Move 3 cards from stack 6 to stack 0

Foundations: H-K C-0 D-0 S-K
Freecells:
: 4C 3C 2C AC
: KC QC JC TC 9C 8C 7C 6C 5C 4D 3D 2D AD
:
: 5D KD QD JD TD 9D 8D 7D 6D
:
:
:
:
:
:


====================

Move 4 cards from stack 1 to stack 4

Foundations: H-K C-0 D-0 S-K
Freecells:
: 4C 3C 2C AC
: KC QC JC TC 9C 8C 7C 6C 5C
:
: 5D KD QD JD TD 9D 8D 7D 6D
: 4D 3D 2D AD
:
:
:
:
:


====================

Move 4 cards from stack 0 to stack 1

Foundations: H-K C-0 D-0 S-K
Freecells:
:
: KC QC JC TC 9C 8C 7C 6C 5C 4C 3C 2C AC
:
: 5D KD QD JD TD 9D 8D 7D 6D
: 4D 3D 2D AD
:
:
:
:
:


====================

Move the sequence on top of Stack 1 to the foundations

Foundations: H-K C-K D-0 S-K
Freecells:
:
:
:
: 5D KD QD JD TD 9D 8D 7D 6D
: 4D 3D 2D AD
:
:
:
:
:


====================

Move 8 cards from stack 3 to stack 8

Foundations: H-K C-K D-0 S-K
Freecells:
:
:
:
: 5D
: 4D 3D 2D AD
:
:
:
: KD QD JD TD 9D 8D 7D 6D
:


====================

Move 4 cards from stack 4 to stack 3

Foundations: H-K C-K D-0 S-K
Freecells:
:
:
:
: 5D 4D 3D 2D AD
:
:
:
:
: KD QD JD TD 9D 8D 7D 6D
:


====================

Move 5 cards from stack 3 to stack 8

Foundations: H-K C-K D-0 S-K
Freecells:
:
:
:
:
:
:
:
:
: KD QD JD TD 9D 8D 7D 6D 5D 4D 3D 2D AD
:


====================

Move the sequence on top of Stack 8 to the foundations

Foundations: H-K C-K D-K S-K
Freecells:
:
:
:
:
:
:
:
:
:
:


====================

`;
    const board_without_trailing_newline__proto =
`Freecells:  2S  KC  -  -
Foundations: H-A C-A D-0 S-0
8H 7C JH 2C 2H 4C
2D 9S 5S 4S 9C 8D KS
TC 9H 6C 8S KH 7H 4H
JS 3C TD 5C 6S 6H TS
JD AS QH 5H 3H KD
9D 7S AD 8C 3S JC
QC QD 6D 4D 3D
TH 7D QS 5D`;
    const board_without_trailing_newline =
    String(board_without_trailing_newline__proto).replace(/\n+$/, '');

    const solution_for_board_without_trailing_newline =
`-=-=-=-=-=-=-=-=-=-=-=-

Foundations: ♥-A ♣-A ♦-0 ♠-0
Freecells:  2♠  K♣
: 8♥ 7♣ J♥ 2♣ 2♥ 4♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦ A♠ Q♥ 5♥ 3♥ K♦
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦


====================

Move a card from stack 4 to freecell 2

Foundations: ♥-A ♣-A ♦-0 ♠-0
Freecells:  2♠  K♣  K♦
: 8♥ 7♣ J♥ 2♣ 2♥ 4♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦ A♠ Q♥ 5♥ 3♥
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦


====================

Move a card from freecell 0 to stack 4

Foundations: ♥-A ♣-A ♦-0 ♠-0
Freecells:      K♣  K♦
: 8♥ 7♣ J♥ 2♣ 2♥ 4♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦ A♠ Q♥ 5♥ 3♥ 2♠
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦


====================

Move a card from stack 0 to freecell 3

Foundations: ♥-A ♣-A ♦-0 ♠-0
Freecells:      K♣  K♦  4♣
: 8♥ 7♣ J♥ 2♣ 2♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦ A♠ Q♥ 5♥ 3♥ 2♠
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦


====================

Move a card from stack 0 to the foundations

Foundations: ♥-2 ♣-A ♦-0 ♠-0
Freecells:      K♣  K♦  4♣
: 8♥ 7♣ J♥ 2♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦ A♠ Q♥ 5♥ 3♥ 2♠
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦


====================

Move a card from stack 0 to the foundations

Foundations: ♥-2 ♣-2 ♦-0 ♠-0
Freecells:      K♣  K♦  4♣
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦ A♠ Q♥ 5♥ 3♥ 2♠
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦


====================

Move a card from freecell 3 to stack 7

Foundations: ♥-2 ♣-2 ♦-0 ♠-0
Freecells:      K♣  K♦
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦ A♠ Q♥ 5♥ 3♥ 2♠
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 4 to freecell 0

Foundations: ♥-2 ♣-2 ♦-0 ♠-0
Freecells:  2♠  K♣  K♦
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦ A♠ Q♥ 5♥ 3♥
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 4 to the foundations

Foundations: ♥-3 ♣-2 ♦-0 ♠-0
Freecells:  2♠  K♣  K♦
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥ 4♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦ A♠ Q♥ 5♥
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 2 to the foundations

Foundations: ♥-4 ♣-2 ♦-0 ♠-0
Freecells:  2♠  K♣  K♦
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦ A♠ Q♥ 5♥
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 4 to the foundations

Foundations: ♥-5 ♣-2 ♦-0 ♠-0
Freecells:  2♠  K♣  K♦
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦ A♠ Q♥
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from freecell 0 to stack 6

Foundations: ♥-5 ♣-2 ♦-0 ♠-0
Freecells:      K♣  K♦
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦ A♠ Q♥
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦ 2♠
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 4 to freecell 3

Foundations: ♥-5 ♣-2 ♦-0 ♠-0
Freecells:      K♣  K♦  Q♥
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦ A♠
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦ 2♠
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 4 to the foundations

Foundations: ♥-5 ♣-2 ♦-0 ♠-A
Freecells:      K♣  K♦  Q♥
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦ 2♠
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 6 to the foundations

Foundations: ♥-5 ♣-2 ♦-0 ♠-2
Freecells:      K♣  K♦  Q♥
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from freecell 3 to stack 1

Foundations: ♥-5 ♣-2 ♦-0 ♠-2
Freecells:      K♣  K♦
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦
: 9♦ 7♠ A♦ 8♣ 3♠ J♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 5 to freecell 0

Foundations: ♥-5 ♣-2 ♦-0 ♠-2
Freecells:  J♣  K♣  K♦
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦
: 9♦ 7♠ A♦ 8♣ 3♠
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 5 to the foundations

Foundations: ♥-5 ♣-2 ♦-0 ♠-3
Freecells:  J♣  K♣  K♦
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦
: 9♦ 7♠ A♦ 8♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from freecell 0 to stack 1

Foundations: ♥-5 ♣-2 ♦-0 ♠-3
Freecells:      K♣  K♦
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦
: 9♦ 7♠ A♦ 8♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 5 to freecell 3

Foundations: ♥-5 ♣-2 ♦-0 ♠-3
Freecells:      K♣  K♦  8♣
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦
: 9♦ 7♠ A♦
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 5 to the foundations

Foundations: ♥-5 ♣-2 ♦-A ♠-3
Freecells:      K♣  K♦  8♣
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦
: 9♦ 7♠
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 5 to freecell 0

Foundations: ♥-5 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦  8♣
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦
: 9♦
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from freecell 3 to stack 5

Foundations: ♥-5 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥ T♠
: J♦
: 9♦ 8♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 3 to freecell 3

Foundations: ♥-5 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦  T♠
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠ 6♥
: J♦
: 9♦ 8♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 3 to the foundations

Foundations: ♥-6 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦  T♠
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥ 7♥
: J♠ 3♣ T♦ 5♣ 6♠
: J♦
: 9♦ 8♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from stack 2 to the foundations

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦  T♠
: 8♥ 7♣ J♥
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥
: J♠ 3♣ T♦ 5♣ 6♠
: J♦
: 9♦ 8♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move a card from freecell 3 to stack 0

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥
: J♠ 3♣ T♦ 5♣ 6♠
: J♦
: 9♦ 8♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move 2 cards from stack 5 to stack 0

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥
: J♠ 3♣ T♦ 5♣ 6♠
: J♦
:
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠ 5♦ 4♣


====================

Move 2 cards from stack 7 to stack 5

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥
: J♠ 3♣ T♦ 5♣ 6♠
: J♦
: 5♦ 4♣
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠


====================

Move 2 cards from stack 5 to stack 3

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: J♦
:
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦ Q♠


====================

Move a card from stack 7 to freecell 3

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦  Q♠
: 8♥ 7♣ J♥ T♠ 9♦ 8♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: J♦
:
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥ 7♦


====================

Move 1 cards from stack 7 to stack 5

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦  Q♠
: 8♥ 7♣ J♥ T♠ 9♦ 8♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: J♦
: 7♦
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥


====================

Move a card from freecell 3 to stack 2

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥ Q♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: J♦
: 7♦
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥


====================

Move 1 cards from stack 5 to stack 0

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠ K♥ Q♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: J♦
:
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥


====================

Move 2 cards from stack 2 to stack 5

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣
: T♣ 9♥ 6♣ 8♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: J♦
: K♥ Q♠
: Q♣ Q♦ 6♦ 4♦ 3♦
: T♥


====================

Move 1 cards from stack 7 to stack 1

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥
: T♣ 9♥ 6♣ 8♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: J♦
: K♥ Q♠
: Q♣ Q♦ 6♦ 4♦ 3♦
:


====================

Move a card from stack 2 to freecell 3

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦  8♠
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥
: T♣ 9♥ 6♣
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: J♦
: K♥ Q♠
: Q♣ Q♦ 6♦ 4♦ 3♦
:


====================

Move 1 cards from stack 2 to stack 7

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦  8♠
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥
: T♣ 9♥
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: J♦
: K♥ Q♠
: Q♣ Q♦ 6♦ 4♦ 3♦
: 6♣


====================

Move a card from freecell 3 to stack 2

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥
: T♣ 9♥ 8♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: J♦
: K♥ Q♠
: Q♣ Q♦ 6♦ 4♦ 3♦
: 6♣


====================

Move 1 cards from stack 7 to stack 0

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥
: T♣ 9♥ 8♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: J♦
: K♥ Q♠
: Q♣ Q♦ 6♦ 4♦ 3♦
:


====================

Move a card from stack 6 to freecell 3

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦  3♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥
: T♣ 9♥ 8♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: J♦
: K♥ Q♠
: Q♣ Q♦ 6♦ 4♦
:


====================

Move 1 cards from stack 6 to stack 7

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦  3♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥
: T♣ 9♥ 8♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: J♦
: K♥ Q♠
: Q♣ Q♦ 6♦
: 4♦


====================

Move a card from freecell 3 to stack 3

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥
: T♣ 9♥ 8♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦
: J♦
: K♥ Q♠
: Q♣ Q♦ 6♦
: 4♦


====================

Move 1 cards from stack 4 to stack 5

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥
: T♣ 9♥ 8♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦
:
: K♥ Q♠ J♦
: Q♣ Q♦ 6♦
: 4♦


====================

Move 3 cards from stack 2 to stack 5

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦ K♠ Q♥ J♣ T♥
:
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦
:
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 4♦


====================

Move 4 cards from stack 1 to stack 4

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 2♦ 9♠ 5♠ 4♠ 9♣ 8♦
:
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦
: K♠ Q♥ J♣ T♥
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 4♦


====================

Move 2 cards from stack 1 to stack 2

Foundations: ♥-7 ♣-2 ♦-A ♠-3
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 2♦ 9♠ 5♠ 4♠
: 9♣ 8♦
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦
: K♠ Q♥ J♣ T♥
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 4♦


====================

Move a card from stack 1 to the foundations

Foundations: ♥-7 ♣-2 ♦-A ♠-4
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 2♦ 9♠ 5♠
: 9♣ 8♦
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦
: K♠ Q♥ J♣ T♥
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 4♦


====================

Move a card from stack 1 to the foundations

Foundations: ♥-7 ♣-2 ♦-A ♠-5
Freecells:  7♠  K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 2♦ 9♠
: 9♣ 8♦
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦
: K♠ Q♥ J♣ T♥
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 4♦


====================

Move a card from freecell 0 to stack 2

Foundations: ♥-7 ♣-2 ♦-A ♠-5
Freecells:      K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 2♦ 9♠
: 9♣ 8♦ 7♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦
: K♠ Q♥ J♣ T♥
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 4♦


====================

Move 1 cards from stack 1 to stack 4

Foundations: ♥-7 ♣-2 ♦-A ♠-5
Freecells:      K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 2♦
: 9♣ 8♦ 7♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦
: K♠ Q♥ J♣ T♥ 9♠
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 4♦


====================

Move a card from stack 1 to the foundations

Foundations: ♥-7 ♣-2 ♦-2 ♠-5
Freecells:      K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
:
: 9♣ 8♦ 7♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣ 3♦
: K♠ Q♥ J♣ T♥ 9♠
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 4♦


====================

Move a card from stack 3 to the foundations

Foundations: ♥-7 ♣-2 ♦-3 ♠-5
Freecells:      K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
:
: 9♣ 8♦ 7♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: K♠ Q♥ J♣ T♥ 9♠
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 4♦


====================

Move a card from stack 7 to the foundations

Foundations: ♥-7 ♣-2 ♦-4 ♠-5
Freecells:      K♣  K♦
: 8♥ 7♣ J♥ T♠ 9♦ 8♣ 7♦ 6♣
:
: 9♣ 8♦ 7♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: K♠ Q♥ J♣ T♥ 9♠
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
:


====================

Move 6 cards from stack 0 to stack 1

Foundations: ♥-7 ♣-2 ♦-4 ♠-5
Freecells:      K♣  K♦
: 8♥ 7♣
: J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 9♣ 8♦ 7♠
: J♠ 3♣ T♦ 5♣ 6♠ 5♦ 4♣
: K♠ Q♥ J♣ T♥ 9♠
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
:


====================

Move 3 cards from stack 3 to stack 7

Foundations: ♥-7 ♣-2 ♦-4 ♠-5
Freecells:      K♣  K♦
: 8♥ 7♣
: J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 9♣ 8♦ 7♠
: J♠ 3♣ T♦ 5♣
: K♠ Q♥ J♣ T♥ 9♠
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 6♠ 5♦ 4♣


====================

Move 2 cards from stack 0 to stack 4

Foundations: ♥-7 ♣-2 ♦-4 ♠-5
Freecells:      K♣  K♦
:
: J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 9♣ 8♦ 7♠
: J♠ 3♣ T♦ 5♣
: K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 6♠ 5♦ 4♣


====================

Move a card from stack 3 to freecell 3

Foundations: ♥-7 ♣-2 ♦-4 ♠-5
Freecells:      K♣  K♦  5♣
:
: J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 9♣ 8♦ 7♠
: J♠ 3♣ T♦
: K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 6♠ 5♦ 4♣


====================

Move a card from stack 3 to freecell 0

Foundations: ♥-7 ♣-2 ♦-4 ♠-5
Freecells:  T♦  K♣  K♦  5♣
:
: J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 9♣ 8♦ 7♠
: J♠ 3♣
: K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 6♠ 5♦ 4♣


====================

Move 1 cards from stack 3 to stack 0

Foundations: ♥-7 ♣-2 ♦-4 ♠-5
Freecells:  T♦  K♣  K♦  5♣
: 3♣
: J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 9♣ 8♦ 7♠
: J♠
: K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 6♠ 5♦ 4♣


====================

Move a card from stack 0 to the foundations

Foundations: ♥-7 ♣-3 ♦-4 ♠-5
Freecells:  T♦  K♣  K♦  5♣
:
: J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 9♣ 8♦ 7♠
: J♠
: K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 6♠ 5♦ 4♣


====================

Move a card from stack 7 to the foundations

Foundations: ♥-7 ♣-4 ♦-4 ♠-5
Freecells:  T♦  K♣  K♦  5♣
:
: J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 9♣ 8♦ 7♠
: J♠
: K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 6♠ 5♦


====================

Move a card from freecell 3 to the foundations

Foundations: ♥-7 ♣-5 ♦-4 ♠-5
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 9♣ 8♦ 7♠
: J♠
: K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 6♠ 5♦


====================

Move a card from stack 7 to the foundations

Foundations: ♥-7 ♣-5 ♦-5 ♠-5
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦ 8♣ 7♦ 6♣
: 9♣ 8♦ 7♠
: J♠
: K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 6♠


====================

Move a card from stack 1 to the foundations

Foundations: ♥-7 ♣-6 ♦-5 ♠-5
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦ 8♣ 7♦
: 9♣ 8♦ 7♠
: J♠
: K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦ 6♦
: 6♠


====================

Move a card from stack 6 to the foundations

Foundations: ♥-7 ♣-6 ♦-6 ♠-5
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦ 8♣ 7♦
: 9♣ 8♦ 7♠
: J♠
: K♠ Q♥ J♣ T♥ 9♠ 8♥ 7♣
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦
: 6♠


====================

Move a card from stack 4 to the foundations

Foundations: ♥-7 ♣-7 ♦-6 ♠-5
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦ 8♣ 7♦
: 9♣ 8♦ 7♠
: J♠
: K♠ Q♥ J♣ T♥ 9♠ 8♥
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦
: 6♠


====================

Move a card from stack 7 to the foundations

Foundations: ♥-7 ♣-7 ♦-6 ♠-6
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦ 8♣ 7♦
: 9♣ 8♦ 7♠
: J♠
: K♠ Q♥ J♣ T♥ 9♠ 8♥
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦
:


====================

Move a card from stack 2 to the foundations

Foundations: ♥-7 ♣-7 ♦-6 ♠-7
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦ 8♣ 7♦
: 9♣ 8♦
: J♠
: K♠ Q♥ J♣ T♥ 9♠ 8♥
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦
:


====================

Move a card from stack 1 to the foundations

Foundations: ♥-7 ♣-7 ♦-7 ♠-7
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦ 8♣
: 9♣ 8♦
: J♠
: K♠ Q♥ J♣ T♥ 9♠ 8♥
: K♥ Q♠ J♦ T♣ 9♥ 8♠
: Q♣ Q♦
:


====================

Move a card from stack 5 to the foundations

Foundations: ♥-7 ♣-7 ♦-7 ♠-8
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦ 8♣
: 9♣ 8♦
: J♠
: K♠ Q♥ J♣ T♥ 9♠ 8♥
: K♥ Q♠ J♦ T♣ 9♥
: Q♣ Q♦
:


====================

Move a card from stack 4 to the foundations

Foundations: ♥-8 ♣-7 ♦-7 ♠-8
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦ 8♣
: 9♣ 8♦
: J♠
: K♠ Q♥ J♣ T♥ 9♠
: K♥ Q♠ J♦ T♣ 9♥
: Q♣ Q♦
:


====================

Move a card from stack 2 to the foundations

Foundations: ♥-8 ♣-7 ♦-8 ♠-8
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦ 8♣
: 9♣
: J♠
: K♠ Q♥ J♣ T♥ 9♠
: K♥ Q♠ J♦ T♣ 9♥
: Q♣ Q♦
:


====================

Move a card from stack 1 to the foundations

Foundations: ♥-8 ♣-8 ♦-8 ♠-8
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦
: 9♣
: J♠
: K♠ Q♥ J♣ T♥ 9♠
: K♥ Q♠ J♦ T♣ 9♥
: Q♣ Q♦
:


====================

Move a card from stack 5 to the foundations

Foundations: ♥-9 ♣-8 ♦-8 ♠-8
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦
: 9♣
: J♠
: K♠ Q♥ J♣ T♥ 9♠
: K♥ Q♠ J♦ T♣
: Q♣ Q♦
:


====================

Move a card from stack 4 to the foundations

Foundations: ♥-9 ♣-8 ♦-8 ♠-9
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦
: 9♣
: J♠
: K♠ Q♥ J♣ T♥
: K♥ Q♠ J♦ T♣
: Q♣ Q♦
:


====================

Move a card from stack 2 to the foundations

Foundations: ♥-9 ♣-9 ♦-8 ♠-9
Freecells:  T♦  K♣  K♦
:
: J♥ T♠ 9♦
:
: J♠
: K♠ Q♥ J♣ T♥
: K♥ Q♠ J♦ T♣
: Q♣ Q♦
:


====================

Move a card from stack 1 to the foundations

Foundations: ♥-9 ♣-9 ♦-9 ♠-9
Freecells:  T♦  K♣  K♦
:
: J♥ T♠
:
: J♠
: K♠ Q♥ J♣ T♥
: K♥ Q♠ J♦ T♣
: Q♣ Q♦
:


====================

Move a card from stack 5 to the foundations

Foundations: ♥-9 ♣-T ♦-9 ♠-9
Freecells:  T♦  K♣  K♦
:
: J♥ T♠
:
: J♠
: K♠ Q♥ J♣ T♥
: K♥ Q♠ J♦
: Q♣ Q♦
:


====================

Move a card from stack 4 to the foundations

Foundations: ♥-T ♣-T ♦-9 ♠-9
Freecells:  T♦  K♣  K♦
:
: J♥ T♠
:
: J♠
: K♠ Q♥ J♣
: K♥ Q♠ J♦
: Q♣ Q♦
:


====================

Move a card from freecell 0 to the foundations

Foundations: ♥-T ♣-T ♦-T ♠-9
Freecells:      K♣  K♦
:
: J♥ T♠
:
: J♠
: K♠ Q♥ J♣
: K♥ Q♠ J♦
: Q♣ Q♦
:


====================

Move a card from stack 1 to the foundations

Foundations: ♥-T ♣-T ♦-T ♠-T
Freecells:      K♣  K♦
:
: J♥
:
: J♠
: K♠ Q♥ J♣
: K♥ Q♠ J♦
: Q♣ Q♦
:


====================

Move a card from stack 3 to the foundations

Foundations: ♥-T ♣-T ♦-T ♠-J
Freecells:      K♣  K♦
:
: J♥
:
:
: K♠ Q♥ J♣
: K♥ Q♠ J♦
: Q♣ Q♦
:


====================

Move a card from stack 5 to the foundations

Foundations: ♥-T ♣-T ♦-J ♠-J
Freecells:      K♣  K♦
:
: J♥
:
:
: K♠ Q♥ J♣
: K♥ Q♠
: Q♣ Q♦
:


====================

Move a card from stack 4 to the foundations

Foundations: ♥-T ♣-J ♦-J ♠-J
Freecells:      K♣  K♦
:
: J♥
:
:
: K♠ Q♥
: K♥ Q♠
: Q♣ Q♦
:


====================

Move a card from stack 1 to the foundations

Foundations: ♥-J ♣-J ♦-J ♠-J
Freecells:      K♣  K♦
:
:
:
:
: K♠ Q♥
: K♥ Q♠
: Q♣ Q♦
:


====================

Move a card from stack 6 to the foundations

Foundations: ♥-J ♣-J ♦-Q ♠-J
Freecells:      K♣  K♦
:
:
:
:
: K♠ Q♥
: K♥ Q♠
: Q♣
:


====================

Move a card from stack 5 to the foundations

Foundations: ♥-J ♣-J ♦-Q ♠-Q
Freecells:      K♣  K♦
:
:
:
:
: K♠ Q♥
: K♥
: Q♣
:


====================

Move a card from stack 4 to the foundations

Foundations: ♥-Q ♣-J ♦-Q ♠-Q
Freecells:      K♣  K♦
:
:
:
:
: K♠
: K♥
: Q♣
:


====================

Move a card from freecell 2 to the foundations

Foundations: ♥-Q ♣-J ♦-K ♠-Q
Freecells:      K♣
:
:
:
:
: K♠
: K♥
: Q♣
:


====================

Move a card from stack 6 to the foundations

Foundations: ♥-Q ♣-Q ♦-K ♠-Q
Freecells:      K♣
:
:
:
:
: K♠
: K♥
:
:


====================

Move a card from stack 5 to the foundations

Foundations: ♥-K ♣-Q ♦-K ♠-Q
Freecells:      K♣
:
:
:
:
: K♠
:
:
:


====================

Move a card from stack 4 to the foundations

Foundations: ♥-K ♣-Q ♦-K ♠-K
Freecells:      K♣
:
:
:
:
:
:
:
:


====================

Move a card from freecell 1 to the foundations

Foundations: ♥-K ♣-K ♦-K ♠-K
Freecells:
:
:
:
:
:
:
:
:


====================

`;
    const solution_for_deal_24__expanded_moves = `-=-=-=-=-=-=-=-=-=-=-=-

Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 3 to the foundations

Foundations: H-0 C-0 D-0 S-A
Freecells:
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 6 to freecell 0

Foundations: H-0 C-0 D-0 S-A
Freecells:  JD
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 6 to freecell 1

Foundations: H-0 C-0 D-0 S-A
Freecells:  JD  8H
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 6 to the foundations

Foundations: H-0 C-0 D-A S-A
Freecells:  JD  8H
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 0 to freecell 2

Foundations: H-0 C-0 D-A S-A
Freecells:  JD  8H  2H
: 4C 2C 9C 8C QS 4S
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 0 to freecell 3

Foundations: H-0 C-0 D-A S-A
Freecells:  JD  8H  2H  4S
: 4C 2C 9C 8C QS
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S
: 7S 6C 7D 4D 8S 9D


====================

Move a card from freecell 0 to stack 0

Foundations: H-0 C-0 D-A S-A
Freecells:      8H  2H  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 6 to freecell 0

Foundations: H-0 C-0 D-A S-A
Freecells:  6S  8H  2H  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 6 to stack 3

Foundations: H-0 C-0 D-A S-A
Freecells:  6S  8H  2H  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 6 to the foundations

Foundations: H-A C-0 D-A S-A
Freecells:  6S  8H  2H  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
:
: 7S 6C 7D 4D 8S 9D


====================

Move a card from freecell 2 to the foundations

Foundations: H-2 C-0 D-A S-A
Freecells:  6S  8H      4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
:
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 1 to freecell 2

Foundations: H-2 C-0 D-A S-A
Freecells:  6S  8H  QD  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC 3H 4H
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
:
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 1 to stack 6

Foundations: H-2 C-0 D-A S-A
Freecells:  6S  8H  QD  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC 3H
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: 4H
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 1 to the foundations

Foundations: H-3 C-0 D-A S-A
Freecells:  6S  8H  QD  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: 4H
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 6 to the foundations

Foundations: H-4 C-0 D-A S-A
Freecells:  6S  8H  QD  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C AC
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
:
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 1 to the foundations

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H  QD  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
:
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 5 to stack 6

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H  QD  4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC
: 7C
: 7S 6C 7D 4D 8S 9D


====================

Move a card from freecell 2 to stack 5

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H      4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D
: 7H JS KH TS KC QD
: 7C
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 6 to stack 4

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H      4S
: 4C 2C 9C 8C QS JD
: 5H QH 3C
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
:
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 1 to freecell 2

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 5H QH
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
:
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 1 to stack 6

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 5H
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D


====================

Move a card from freecell 3 to stack 1

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H  3C
: 4C 2C 9C 8C QS JD
: 5H 4S
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 1 to freecell 3

Foundations: H-4 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 5H
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D


====================

Move a card from stack 1 to the foundations

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
:
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 3 to stack 1

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 5S
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 3 to stack 4

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 5S
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 1 to stack 4

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
:
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 2 to stack 1

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 6 to stack 2

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD
:
: 7S 6C 7D 4D 8S 9D


====================

Move 1 cards from stack 7 to stack 6

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD
: 9D
: 7S 6C 7D 4D 8S


====================

Move 1 cards from stack 7 to stack 6

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD
: 9D 8S
: 7S 6C 7D 4D


====================

Move a card from freecell 2 to stack 7

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H      4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD
: 9D 8S
: 7S 6C 7D 4D 3C


====================

Move a card from stack 7 to freecell 2

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD
: 9D 8S
: 7S 6C 7D 4D


====================

Move 1 cards from stack 7 to stack 4

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S 4D
: 7H JS KH TS KC QD
: 9D 8S
: 7S 6C 7D


====================

Move a card from freecell 2 to stack 4

Foundations: H-5 C-A D-A S-A
Freecells:  6S  8H      4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD
: 9D 8S
: 7S 6C 7D


====================

Move a card from freecell 0 to stack 7

Foundations: H-5 C-A D-A S-A
Freecells:      8H      4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C JH
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 3 to freecell 2

Foundations: H-5 C-A D-A S-A
Freecells:      8H  JH  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC 5C
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 3 to freecell 0

Foundations: H-5 C-A D-A S-A
Freecells:  5C  8H  JH  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S JC
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD
: 9D 8S
: 7S 6C 7D 6S


====================

Move 1 cards from stack 3 to stack 5

Foundations: H-5 C-A D-A S-A
Freecells:  5C  8H  JH  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 2S
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 3 to the foundations

Foundations: H-5 C-A D-A S-2
Freecells:  5C  8H  JH  4S
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from freecell 3 to stack 3

Foundations: H-5 C-A D-A S-2
Freecells:  5C  8H  JH
: 4C 2C 9C 8C QS JD
: 3D
: QC 9S 6H 9H 3S KS QH
: 5D 4S
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move 1 cards from stack 1 to stack 3

Foundations: H-5 C-A D-A S-2
Freecells:  5C  8H  JH
: 4C 2C 9C 8C QS JD
:
: QC 9S 6H 9H 3S KS QH
: 5D 4S 3D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 2 to freecell 3

Foundations: H-5 C-A D-A S-2
Freecells:  5C  8H  JH  QH
: 4C 2C 9C 8C QS JD
:
: QC 9S 6H 9H 3S KS
: 5D 4S 3D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move 1 cards from stack 2 to stack 1

Foundations: H-5 C-A D-A S-2
Freecells:  5C  8H  JH  QH
: 4C 2C 9C 8C QS JD
: KS
: QC 9S 6H 9H 3S
: 5D 4S 3D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 2 to the foundations

Foundations: H-5 C-A D-A S-3
Freecells:  5C  8H  JH  QH
: 4C 2C 9C 8C QS JD
: KS
: QC 9S 6H 9H
: 5D 4S 3D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from freecell 3 to stack 1

Foundations: H-5 C-A D-A S-3
Freecells:  5C  8H  JH
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 6H 9H
: 5D 4S 3D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 2 to freecell 3

Foundations: H-5 C-A D-A S-3
Freecells:  5C  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 6H
: 5D 4S 3D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from freecell 0 to stack 2

Foundations: H-5 C-A D-A S-3
Freecells:      8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 6H 5C
: 5D 4S 3D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 3 to freecell 0

Foundations: H-5 C-A D-A S-3
Freecells:  3D  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 6H 5C
: 5D 4S
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 3 to the foundations

Foundations: H-5 C-A D-A S-4
Freecells:  3D  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 6H 5C
: 5D
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move 1 cards from stack 3 to stack 7

Foundations: H-5 C-A D-A S-4
Freecells:  3D  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 6H 5C
:
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D


====================

Move 1 cards from stack 2 to stack 3

Foundations: H-5 C-A D-A S-4
Freecells:  3D  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 6H
: 5C
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D


====================

Move a card from stack 2 to the foundations

Foundations: H-6 C-A D-A S-4
Freecells:  3D  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S
: 5C
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D


====================

Move a card from freecell 1 to stack 2

Foundations: H-6 C-A D-A S-4
Freecells:  3D      JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C
: 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D


====================

Move a card from stack 4 to freecell 1

Foundations: H-6 C-A D-A S-4
Freecells:  3D  3C  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C
: 2D KD TH TC TD 8D 7C 6D 5S 4D
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D


====================

Move 1 cards from stack 4 to stack 3

Foundations: H-6 C-A D-A S-4
Freecells:  3D  3C  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D


====================

Move a card from freecell 1 to stack 3

Foundations: H-6 C-A D-A S-4
Freecells:  3D      JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D


====================

Move a card from stack 4 to the foundations

Foundations: H-6 C-A D-A S-5
Freecells:  3D      JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D


====================

Move a card from stack 7 to freecell 1

Foundations: H-6 C-A D-A S-5
Freecells:  3D  5D  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S


====================

Move a card from stack 7 to the foundations

Foundations: H-6 C-A D-A S-6
Freecells:  3D  5D  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D


====================

Move 1 cards from stack 7 to stack 6

Foundations: H-6 C-A D-A S-6
Freecells:  3D  5D  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D
: 7S 6C


====================

Move a card from freecell 1 to stack 7

Foundations: H-6 C-A D-A S-6
Freecells:  3D      JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D
: 7S 6C 5D


====================

Move a card from stack 7 to freecell 1

Foundations: H-6 C-A D-A S-6
Freecells:  3D  5D  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D
: 7S 6C


====================

Move 1 cards from stack 7 to stack 6

Foundations: H-6 C-A D-A S-6
Freecells:  3D  5D  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C
: 7S


====================

Move a card from freecell 1 to stack 6

Foundations: H-6 C-A D-A S-6
Freecells:  3D      JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 7S


====================

Move a card from stack 7 to the foundations

Foundations: H-6 C-A D-A S-7
Freecells:  3D      JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
:


====================

Move a card from stack 2 to freecell 1

Foundations: H-6 C-A D-A S-7
Freecells:  3D  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
:


====================

Move 1 cards from stack 2 to stack 7

Foundations: H-6 C-A D-A S-7
Freecells:  3D  8H  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 9S


====================

Move a card from freecell 2 to stack 2

Foundations: H-6 C-A D-A S-7
Freecells:  3D  8H      9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 9S


====================

Move a card from freecell 1 to stack 7

Foundations: H-6 C-A D-A S-7
Freecells:  3D          9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 3 to freecell 1

Foundations: H-6 C-A D-A S-7
Freecells:  3D  3C      9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: 5C 4D
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 3 to freecell 2

Foundations: H-6 C-A D-A S-7
Freecells:  3D  3C  4D  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: 5C
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move 1 cards from stack 3 to stack 4

Foundations: H-6 C-A D-A S-7
Freecells:  3D  3C  4D  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
:
: 2D KD TH TC TD 8D 7C 6D 5C
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from freecell 2 to stack 4

Foundations: H-6 C-A D-A S-7
Freecells:  3D  3C      9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
:
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from freecell 1 to stack 4

Foundations: H-6 C-A D-A S-7
Freecells:  3D          9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
:
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 5 to freecell 2

Foundations: H-6 C-A D-A S-7
Freecells:  3D      JC  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
:
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS KC QD
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 5 to freecell 1

Foundations: H-6 C-A D-A S-7
Freecells:  3D  QD  JC  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
:
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS KC
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move 1 cards from stack 5 to stack 3

Foundations: H-6 C-A D-A S-7
Freecells:  3D  QD  JC  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: KC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from freecell 3 to stack 5

Foundations: H-6 C-A D-A S-7
Freecells:  3D  QD  JC
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: KC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from freecell 1 to stack 3

Foundations: H-6 C-A D-A S-7
Freecells:  3D      JC
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from freecell 2 to stack 3

Foundations: H-6 C-A D-A S-7
Freecells:  3D
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 0 to freecell 3

Foundations: H-6 C-A D-A S-7
Freecells:  3D          JD
: 4C 2C 9C 8C QS
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 0 to freecell 1

Foundations: H-6 C-A D-A S-7
Freecells:  3D  QS      JD
: 4C 2C 9C 8C
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move 1 cards from stack 0 to stack 5

Foundations: H-6 C-A D-A S-7
Freecells:  3D  QS      JD
: 4C 2C 9C
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 0 to freecell 2

Foundations: H-6 C-A D-A S-7
Freecells:  3D  QS  9C  JD
: 4C 2C
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 0 to the foundations

Foundations: H-6 C-2 D-A S-7
Freecells:  3D  QS  9C  JD
: 4C
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 4 to the foundations

Foundations: H-6 C-3 D-A S-7
Freecells:  3D  QS  9C  JD
: 4C
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 0 to the foundations

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  QS  9C  JD
:
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from freecell 0 to stack 0

Foundations: H-6 C-4 D-A S-7
Freecells:      QS  9C  JD
: 3D
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move 1 cards from stack 3 to stack 1

Foundations: H-6 C-4 D-A S-7
Freecells:      QS  9C  JD
: 3D
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from stack 0 to freecell 0

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  QS  9C  JD
:
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move a card from freecell 2 to stack 0

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  QS      JD
: 9C
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H


====================

Move 1 cards from stack 7 to stack 0

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  QS      JD
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S


====================

Move 1 cards from stack 1 to stack 3

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  QS      JD
: 9C 8H
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S


====================

Move a card from stack 7 to freecell 2

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  QS  9S  JD
: 9C 8H
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
:


====================

Move a card from freecell 0 to stack 7

Foundations: H-6 C-4 D-A S-7
Freecells:      QS  9S  JD
: 9C 8H
: KS QH
: QC JH
: KC QD JC
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 3D


====================

Move 1 cards from stack 3 to stack 1

Foundations: H-6 C-4 D-A S-7
Freecells:      QS  9S  JD
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 3D


====================

Move a card from stack 7 to freecell 0

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  QS  9S  JD
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
:


====================

Move a card from freecell 1 to stack 7

Foundations: H-6 C-4 D-A S-7
Freecells:  3D      9S  JD
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: QS


====================

Move a card from freecell 3 to stack 7

Foundations: H-6 C-4 D-A S-7
Freecells:  3D      9S
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: QS JD


====================

Move a card from stack 4 to freecell 1

Foundations: H-6 C-4 D-A S-7
Freecells:  3D  4D  9S
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: QS JD


====================

Move a card from stack 4 to the foundations

Foundations: H-6 C-5 D-A S-7
Freecells:  3D  4D  9S
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: QS JD


====================

Move a card from stack 6 to freecell 3

Foundations: H-6 C-5 D-A S-7
Freecells:  3D  4D  9S  5D
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C
: QS JD


====================

Move a card from stack 6 to the foundations

Foundations: H-6 C-6 D-A S-7
Freecells:  3D  4D  9S  5D
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS 9H 8C
: 9D 8S 7D
: QS JD


====================

Move 1 cards from stack 6 to stack 5

Foundations: H-6 C-6 D-A S-7
Freecells:  3D  4D  9S  5D
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS 9H 8C 7D
: 9D 8S
: QS JD


====================

Move a card from stack 6 to the foundations

Foundations: H-6 C-6 D-A S-8
Freecells:  3D  4D  9S  5D
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD


====================

Move a card from freecell 2 to the foundations

Foundations: H-6 C-6 D-A S-9
Freecells:  3D  4D      5D
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD


====================

Move a card from stack 4 to freecell 2

Foundations: H-6 C-6 D-A S-9
Freecells:  3D  4D  6D  5D
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD


====================

Move 1 cards from stack 4 to stack 0

Foundations: H-6 C-6 D-A S-9
Freecells:  3D  4D  6D  5D
: 9C 8H 7C
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD


====================

Move a card from freecell 2 to stack 0

Foundations: H-6 C-6 D-A S-9
Freecells:  3D  4D      5D
: 9C 8H 7C 6D
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD


====================

Move a card from stack 4 to freecell 2

Foundations: H-6 C-6 D-A S-9
Freecells:  3D  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD


====================

Move 1 cards from stack 4 to stack 1

Foundations: H-6 C-6 D-A S-9
Freecells:  3D  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH
: KC QD
: 2D KD TH TC
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD


====================

Move 1 cards from stack 4 to stack 2

Foundations: H-6 C-6 D-A S-9
Freecells:  3D  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC
: KC QD
: 2D KD TH
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD


====================

Move 1 cards from stack 6 to stack 2

Foundations: H-6 C-6 D-A S-9
Freecells:  3D  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D
: KC QD
: 2D KD TH
: 7H JS KH TS 9H 8C 7D
:
: QS JD


====================

Move a card from freecell 0 to stack 6

Foundations: H-6 C-6 D-A S-9
Freecells:      4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D
: KC QD
: 2D KD TH
: 7H JS KH TS 9H 8C 7D
: 3D
: QS JD


====================

Move a card from stack 5 to freecell 0

Foundations: H-6 C-6 D-A S-9
Freecells:  7D  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D
: KC QD
: 2D KD TH
: 7H JS KH TS 9H 8C
: 3D
: QS JD


====================

Move 1 cards from stack 5 to stack 2

Foundations: H-6 C-6 D-A S-9
Freecells:  7D  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D 8C
: KC QD
: 2D KD TH
: 7H JS KH TS 9H
: 3D
: QS JD


====================

Move a card from freecell 0 to stack 2

Foundations: H-6 C-6 D-A S-9
Freecells:      4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
: 2D KD TH
: 7H JS KH TS 9H
: 3D
: QS JD


====================

Move a card from stack 5 to freecell 0

Foundations: H-6 C-6 D-A S-9
Freecells:  9H  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
: 2D KD TH
: 7H JS KH TS
: 3D
: QS JD


====================

Move 1 cards from stack 5 to stack 7

Foundations: H-6 C-6 D-A S-9
Freecells:  9H  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
: 2D KD TH
: 7H JS KH
: 3D
: QS JD TS


====================

Move a card from freecell 0 to stack 7

Foundations: H-6 C-6 D-A S-9
Freecells:      4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
: 2D KD TH
: 7H JS KH
: 3D
: QS JD TS 9H


====================

Move a card from stack 5 to freecell 0

Foundations: H-6 C-6 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
: 2D KD TH
: 7H JS
: 3D
: QS JD TS 9H


====================

Move 1 cards from stack 5 to stack 3

Foundations: H-6 C-6 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD TH
: 7H
: 3D
: QS JD TS 9H


====================

Move a card from stack 5 to the foundations

Foundations: H-7 C-6 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C 8H 7C 6D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD TH
:
: 3D
: QS JD TS 9H


====================

Move 1 cards from stack 0 to stack 5

Foundations: H-7 C-6 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C 8H 7C
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD TH
: 6D
: 3D
: QS JD TS 9H


====================

Move a card from stack 0 to the foundations

Foundations: H-7 C-7 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C 8H
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD TH
: 6D
: 3D
: QS JD TS 9H


====================

Move a card from stack 0 to the foundations

Foundations: H-8 C-7 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD TH
: 6D
: 3D
: QS JD TS 9H


====================

Move a card from stack 7 to the foundations

Foundations: H-9 C-7 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD TH
: 6D
: 3D
: QS JD TS


====================

Move a card from stack 4 to the foundations

Foundations: H-T C-7 D-A S-9
Freecells:  KH  4D  8D  5D
: 9C
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD
: 6D
: 3D
: QS JD TS


====================

Move a card from stack 7 to the foundations

Foundations: H-T C-7 D-A S-T
Freecells:  KH  4D  8D  5D
: 9C
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD JS
: 2D KD
: 6D
: 3D
: QS JD


====================

Move a card from stack 3 to the foundations

Foundations: H-T C-7 D-A S-J
Freecells:  KH  4D  8D  5D
: 9C
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
: 2D KD
: 6D
: 3D
: QS JD


====================

Move a card from freecell 2 to stack 0

Foundations: H-T C-7 D-A S-J
Freecells:  KH  4D      5D
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
: 2D KD
: 6D
: 3D
: QS JD


====================

Move a card from stack 4 to freecell 2

Foundations: H-T C-7 D-A S-J
Freecells:  KH  4D  KD  5D
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
: 2D
: 6D
: 3D
: QS JD


====================

Move a card from stack 4 to the foundations

Foundations: H-T C-7 D-2 S-J
Freecells:  KH  4D  KD  5D
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
:
: 6D
: 3D
: QS JD


====================

Move a card from stack 6 to the foundations

Foundations: H-T C-7 D-3 S-J
Freecells:  KH  4D  KD  5D
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
:
: 6D
:
: QS JD


====================

Move a card from freecell 1 to the foundations

Foundations: H-T C-7 D-4 S-J
Freecells:  KH      KD  5D
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
:
: 6D
:
: QS JD


====================

Move a card from freecell 3 to the foundations

Foundations: H-T C-7 D-5 S-J
Freecells:  KH      KD
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
:
: 6D
:
: QS JD


====================

Move a card from stack 5 to the foundations

Foundations: H-T C-7 D-6 S-J
Freecells:  KH      KD
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 2 to the foundations

Foundations: H-T C-7 D-7 S-J
Freecells:  KH      KD
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 0 to the foundations

Foundations: H-T C-7 D-8 S-J
Freecells:  KH      KD
: 9C
: KS QH JC TD
: QC JH TC 9D 8C
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 2 to the foundations

Foundations: H-T C-8 D-8 S-J
Freecells:  KH      KD
: 9C
: KS QH JC TD
: QC JH TC 9D
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 0 to the foundations

Foundations: H-T C-9 D-8 S-J
Freecells:  KH      KD
:
: KS QH JC TD
: QC JH TC 9D
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 2 to the foundations

Foundations: H-T C-9 D-9 S-J
Freecells:  KH      KD
:
: KS QH JC TD
: QC JH TC
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 2 to the foundations

Foundations: H-T C-T D-9 S-J
Freecells:  KH      KD
:
: KS QH JC TD
: QC JH
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 2 to the foundations

Foundations: H-J C-T D-9 S-J
Freecells:  KH      KD
:
: KS QH JC TD
: QC
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 1 to the foundations

Foundations: H-J C-T D-T S-J
Freecells:  KH      KD
:
: KS QH JC
: QC
: KC QD
:
:
:
: QS JD


====================

Move a card from stack 7 to the foundations

Foundations: H-J C-T D-J S-J
Freecells:  KH      KD
:
: KS QH JC
: QC
: KC QD
:
:
:
: QS


====================

Move a card from stack 7 to the foundations

Foundations: H-J C-T D-J S-Q
Freecells:  KH      KD
:
: KS QH JC
: QC
: KC QD
:
:
:
:


====================

Move a card from stack 3 to the foundations

Foundations: H-J C-T D-Q S-Q
Freecells:  KH      KD
:
: KS QH JC
: QC
: KC
:
:
:
:


====================

Move a card from stack 1 to the foundations

Foundations: H-J C-J D-Q S-Q
Freecells:  KH      KD
:
: KS QH
: QC
: KC
:
:
:
:


====================

Move a card from stack 2 to the foundations

Foundations: H-J C-Q D-Q S-Q
Freecells:  KH      KD
:
: KS QH
:
: KC
:
:
:
:


====================

Move a card from stack 3 to the foundations

Foundations: H-J C-K D-Q S-Q
Freecells:  KH      KD
:
: KS QH
:
:
:
:
:
:


====================

Move a card from stack 1 to the foundations

Foundations: H-Q C-K D-Q S-Q
Freecells:  KH      KD
:
: KS
:
:
:
:
:
:


====================

Move a card from stack 1 to the foundations

Foundations: H-Q C-K D-Q S-K
Freecells:  KH      KD
:
:
:
:
:
:
:
:


====================

Move a card from freecell 0 to the foundations

Foundations: H-K C-K D-Q S-K
Freecells:          KD
:
:
:
:
:
:
:
:


====================

Move a card from freecell 2 to the foundations

Foundations: H-K C-K D-K S-K
Freecells:
:
:
:
:
:
:
:
:


====================

`;
    const my_func = (_my_mod, my_callback) =>
        () => {
            FC_Solve_init_wrappers_with_module(_my_mod[0] || this);
            const deal_ms_fc_board = w.deal_ms_fc_board;

            QUnit.module("FC_Solve.Algorithmic");

            function test_for_equal(assert, instance, board,
                expected_sol, blurb) {
                let success = false;

                let solve_err_code = instance.do_solve(board);

                while (solve_err_code == FCS_STATE_SUSPEND_PROCESS) {
                    solve_err_code = instance.resume_solution();
                }

                if (solve_err_code == FCS_STATE_WAS_SOLVED) {
                    const buffer = instance.display_solution( {} );
                    success = true;
                    assert.equal(buffer, expected_sol, blurb);
                }

                return success;
            };

            QUnit.test("FC_Solve main test", function(assert) {
                assert.expect(3);

                // TEST
                assert.ok(true, "True is, well, true.");

                let instance = new FC_Solve({
                    cmd_line_preset: 'default',
                    set_status_callback: function() {
                        return;
                    },
                });

                // TEST*2
                assert.ok(test_for_equal(assert, instance,
                    ms_deal_24,
                    solution_for_deal_24__default,
                    "Solution is right."
                ),
                "do_solve was successful."
                );
            });

            QUnit.test("FC_Solve unicoded solution", function(assert) {
                assert.expect(2);

                let instance = new FC_Solve({
                    is_unicode_cards: true,
                    cmd_line_preset: 'default',
                    set_status_callback: function() {
                        return;
                    },
                });

                // TEST*2
                assert.ok(test_for_equal(
                    assert,
                    instance,
                    ms_deal_24,
                    solution_for_deal_24__default__with_unicoded_suits,
                    "Unicoded solution was right"
                ),
                "do_solve was successful."
                );
            });

            QUnit.test("FC_Solve unicode cards solution", function(assert) {
                assert.expect(2);

                let instance = new FC_Solve({
                    is_unicode_cards: true,
                    is_unicode_cards_chars: true,
                    cmd_line_preset: 'default',
                    set_status_callback: function() {
                        return;
                    },
                });

                // TEST*2
                assert.ok(test_for_equal(
                    assert,
                    instance,
                    ms_deal_24,
                    solution_for_deal_24__default__with_unicoded_card_chars,
                    "Unicoded cards chars solution was right"
                ),
                "do_solve was successful."
                );
            });

            QUnit.test("FC_Solve arbitrary parameters " +
                        "- Solve Simple Simon.",
            function(assert) {
                assert.expect(2);

                let instance = new FC_Solve({
                    cmd_line_preset: 'default',
                    string_params: '-g simple_simon',
                    dir_base: 'fcs1',
                    set_status_callback: function() {
                        return;
                    },
                });

                // TEST*2
                assert.ok(
                    test_for_equal(
                        assert,
                        instance,
                        pysol_simple_simon_deal_24,
                        solution_for_pysol_simple_simon_deal_24__default,
                        "Simple Simon Solution is right"
                    ),
                    "do_solve was successful."
                );
            });

            QUnit.test("FC_Solve solve board without a trailing newline",
                function(assert) {
                    assert.expect(3);

                    // TEST
                    assert.ok(true, "True is, well, true.");

                    let instance = new FC_Solve({
                        is_unicode_cards: true,
                        cmd_line_preset: "as",
                        set_status_callback: function() {
                            return;
                        },
                    });

                    // TEST*2
                    assert.ok(test_for_equal(
                        assert,
                        instance,
                        board_without_trailing_newline,
                        solution_for_board_without_trailing_newline,
                        "Board without a trailing newline solution is right."
                    ),
                    "do_solve was successful."
                    );
                });


            QUnit.test("FC_Solve Expanded Moves test", function(assert) {
                assert.expect(4);

                // TEST
                assert.ok(true, "True is, well, true.");

                let success = false;

                let instance = new FC_Solve({
                    cmd_line_preset: 'default',
                    set_status_callback: function() {
                        return;
                    },
                });

                if (true) {
                    const ints =
                        s2i.find_index__board_string_to_ints(ms_deal_24);
                    const ints_s = ints.map((i) => {
                        let ret = i.toString();
                        return " ".repeat(10-ret.length) + ret;
                    }).join('');
                    let df = new w.Freecell_Deal_Finder({});
                    df.fill(ints_s);
                    df.run(1, 1000, (args) => {});
                    const ret_Deal = df.cont();
                    // TEST
                    assert.equal(ret_Deal.result, '24', 'Freecell_Deal_Finder');
                } else {
                    assert.ok(true, "skipped.");
                }

                let solve_err_code = instance.do_solve(ms_deal_24);

                while (solve_err_code == FCS_STATE_SUSPEND_PROCESS) {
                    solve_err_code = instance.resume_solution();
                }

                const buffer = instance.display_expanded_moves_solution( {} );

                success = true;
                // TEST
                assert.equal(buffer, solution_for_deal_24__expanded_moves,
                    "Expanded-moves Solution is right"
                );

                // TEST
                assert.ok(success, "do_solve was successful.");
            });

            QUnit.test("FC_Solve get_num_stacks #1", function(assert) {
                assert.expect(1);

                let instance = new FC_Solve({
                    cmd_line_preset: 'default',
                    set_status_callback: function() {
                        return;
                    },
                });

                // TEST
                assert.equal(
                    instance.get_num_stacks(),
                    8, "get_num_stacks() Returns 8."
                );
            });

            QUnit.test("FC_Solve get_num_stacks simple_simon",
                function(assert) {
                    assert.expect(1);

                    let instance = new FC_Solve({
                        cmd_line_preset: 'default',
                        string_params: '-g simple_simon',
                        set_status_callback: function() {
                            return;
                        },
                    });

                    // TEST
                    assert.equal(instance.get_num_stacks(), 10,
                        "get_num_stacks() Returns 10 for Simple Simon."
                    );
                }
            );

            QUnit.test("FC_Solve get_num_stacks command line settings",
                function(assert) {
                    assert.expect(1);

                    let instance = new FC_Solve({
                        cmd_line_preset: 'default',
                        string_params: '--stacks-num 6',
                        dir_base: 'fcs2',
                        set_status_callback: function() {
                            return;
                        },
                    });

                    // TEST
                    assert.equal(instance.get_num_stacks(), 6,
                        "get_num_stacks() Returns 6 after command line."
                    );
                });

            QUnit.test("FC_Solve get_num_freecells #1", function(assert) {
                assert.expect(1);

                let instance = new FC_Solve({
                    cmd_line_preset: 'default',
                    set_status_callback: function() {
                        return;
                    },
                });

                // TEST
                assert.equal(instance.get_num_freecells(), 4,
                    "get_num_freecells() returns 4.");
            });

            QUnit.test("FC_Solve get_num_freecells #1", function(assert) {
                assert.expect(1);

                let instance = new FC_Solve({
                    cmd_line_preset: 'default',
                    string_params: '--freecells-num 2',
                    dir_base: 'fcs3',
                    set_status_callback: function() {
                        return;
                    },
                });

                // TEST
                assert.equal(instance.get_num_freecells(), 2,
                    "get_num_freecells() returns 2 after command line.");
            });

            QUnit.test("FC_Solve deal_ms_fc_board", function(assert) {
                assert.expect(2);

                // TEST
                assert.equal(deal_ms_fc_board(3000000000),
                    `: 8D TS JS TD JH JD JC
: 4D QS TH AD 4S TC 3C
: 9H KH QH 4C 5C KD AS
: 9D 5D 8S 4H KS 6S 9S
: 6H 2S 7H 3D KC 2C
: 9C 7C QC 7S QD 7D
: 6C 3H 8H AC 6D 3S
: 8C AH 2H 5H 2D 5S
`,
                    "deal_ms_fc_board for 3e9");

                // TEST
                assert.equal(deal_ms_fc_board(6000000000),
                    `: 2D 2C QS 8D KD 8C 4C
: 3D AH 2H 4H TS 6H QD
: 4D JS AD 6S JH JC JD
: KH 3H KS AS TC 5D AC
: TD 7C 9C 7H 3C 3S
: QH 9H 9D 5S 7S 6C
: 5C 5H 2S KC 9S 4S
: 6D QC 8S TH 7D 8H
`,
                    "deal_ms_fc_board for 6e9");
            });

            QUnit.test("FC_Solve custom baker's game preset twice",
                function(assert) {
                    // This is microsoft deal #10 which is
                    // impossible to solve
                    // in Baker's Game.
                    const ms10_deal =`5S KD JC TS 9D KH 8D
        5H 2S 9H 7H TD AD 6D
        6H QD 6C TC AH 8S TH
        6S 2D 7C QC QS 7D 3H
        5D AS 7S KC 3D AC
        4D 9C QH 4H 4C 5C
        2H 3S 8H 9S JS 4S
        JH JD 3C KS 2C 8C
        `;
                    assert.expect(2);

                    for (let mytry = 1; mytry <= 2; ++mytry) {
                        let instance = new FC_Solve({
                            cmd_line_preset: 'default',
                            set_status_callback: function() {
                                return;
                            },
                            string_params: '--game bakers_game -to 01ABCDE',
                        });

                        // TEST*2
                        assert.ok((!test_for_equal(
                            assert,
                            instance,
                            ms10_deal,
                            '',
                            ''
                        )),
                        "do_solve failed try=" + mytry
                        );
                    }
                });

            my_callback();
            return;
        };

    function test_js_fc_solve_class(my_callback) {
        // var _my_mod = Module({});
        let _my_mod = [null];
        _my_mod[0] = Module()({
            onRuntimeInitialized: my_func(_my_mod, my_callback)});
        return;
    }
    return {test_js_fc_solve_class: test_js_fc_solve_class};
});

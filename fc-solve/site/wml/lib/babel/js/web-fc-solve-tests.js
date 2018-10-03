"use strict";
function mydef(x, y) {
    if (typeof define !== 'function') {
        return require('amdefine')(module)(x, y);
    } else {
        return define(x, y);
    }
}
mydef(["web-fc-solve", "libfcs-wrap",
    'dist/fc_solve_find_index_s2ints.js', 'web-fcs-tests-strings'],
function(w, Module, s2i, test_strings) {
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
    const solution_for_deal_24__default = test_strings.dict['24_default_ascii'];
    const solution_for_deal_24__default__with_unicoded_suits =
        test_strings.dict['24_default_unicode_suits'];
    const solution_for_deal_24__default__with_unicoded_card_chars =
        test_strings.dict['24_default_unicode_cards'];

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
        test_strings.dict['solution_for_pysol_simple_simon_deal_24__default'];
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
        test_strings.dict['solution_for_board_without_trailing_newline'];
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

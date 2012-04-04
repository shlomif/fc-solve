## vim:ts=4:et:nowrap
##
##---------------------------------------------------------------------------##
##
## PySol -- a Python Solitaire game
##
## Copyright (C) 2003 Markus Franz Xaver Johannes Oberhumer
## Copyright (C) 2002 Markus Franz Xaver Johannes Oberhumer
## Copyright (C) 2001 Markus Franz Xaver Johannes Oberhumer
## Copyright (C) 2000 Markus Franz Xaver Johannes Oberhumer
## Copyright (C) 1999 Markus Franz Xaver Johannes Oberhumer
## Copyright (C) 1998 Markus Franz Xaver Johannes Oberhumer
## All Rights Reserved.
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
## <markus@oberhumer.com>
## http://www.oberhumer.com/pysol
##
##---------------------------------------------------------------------------##

## This file originated from the file game.py in the PySolFC-1.1
## distribution. Shlomi Fish disclaims all rights to his modifications and
## places them under the public domain or MIT/X11 licence.

# imports
import time
import math
import traceback
# from gettext import ungettext
import gettext
from cStringIO import StringIO
import argparse

from pysollib.init import init
# PySol imports
from mfxutil import Pickler, Unpickler, UnpicklingError
from mfxutil import Image, ImageTk
from mfxutil import destruct, Struct, SubclassResponsibility
from mfxutil import uclock, usleep
from mfxutil import format_time, print_err
from settings import PACKAGE, TITLE, TOOLKIT, TOP_TITLE
from settings import VERSION, VERSION_TUPLE
from settings import DEBUG
from gamedb import GI
from pysolrandom import PysolRandom, LCRandom31
from pysoltk import EVENT_HANDLED, EVENT_PROPAGATE
from pysoltk import CURSOR_WATCH
from pysoltk import bind, wm_map
from pysoltk import after, after_idle, after_cancel
from pysoltk import MfxMessageDialog, MfxExceptionDialog
from pysoltk import MfxCanvasText, MfxCanvasLine, MfxCanvasRectangle
from pysoltk import Card
from pysoltk import reset_solver_dialog
from move import AMoveMove, AFlipMove, AFlipAndMoveMove
from move import ASingleFlipMove, ATurnStackMove
from move import ANextRoundMove, ASaveSeedMove, AShuffleStackMove
from move import AUpdateStackMove, AFlipAllMove, ASaveStateMove
from move import ASingleCardMove
from hint import DefaultHint
from help import help_about

PLAY_TIME_TIMEOUT = 200

# /***********************************************************************
# // Base class for all solitaire games
# //
# // Handles:
# //   load/save
# //   undo/redo (using a move history)
# //   hints/demo
# ************************************************************************/

class Game:
    # for self.gstats.updated
    U_PLAY       =  0
    U_WON        = -2
    U_LOST       = -3
    U_PERFECT    = -4

    # for self.moves.state
    S_INIT = 0x00
    S_DEAL = 0x10
    S_FILL = 0x20
    S_PLAY = 0x30
    S_UNDO = 0x40
    S_REDO = 0x50

    # for loading and saving - subclasses should override if
    # the format for a saved game changed (see also canLoadGame())
    GAME_VERSION = 1

    #
    # low level load/save
    #

    def _loadGame(self, filename):
        game = None
        f = None
        try:
            f = open(filename, "rb")
            p = Unpickler(f)
            game = self._undumpGame(p)
            game.gstats.loaded = game.gstats.loaded + 1
        finally:
            if f: f.close()
        return game

    def _undumpGame(self, p):
        self.updateTime()
        #
        err_txt = _("Invalid or damaged %s save file") % PACKAGE
        #
        def pload(t=None, p=p):
            obj = p.load()
            if isinstance(t, type):
                assert isinstance(obj, t), err_txt
            return obj
        def validate(v, txt):
            if not v:
                raise UnpicklingError(txt)
        #
        package = pload(str)
        validate(package == PACKAGE, err_txt)
        version = pload(str)
        #validate(isinstance(version, str) and len(version) <= 20, err_txt)
        version_tuple = pload(tuple)
        validate(version_tuple >= (1,0), _('''\
Cannot load games saved with
%s version %s''') % (PACKAGE, version))
        game_version = 1
        bookmark = pload(int)
        validate(0 <= bookmark <= 2, err_txt)
        game_version = pload(int)
        validate(game_version > 0, err_txt)
        #
        id = pload(int)
        validate(id > 0, err_txt)
        # if id not in GI.PROTECTED_GAMES:
        #    game = app.constructGame(id)
        #    if game:
        #        if not game.canLoadGame(version_tuple, game_version):
        #            destruct(game)
        #            game = None
        validate(game is not None, _('''\
Cannot load this game from version %s
as the game rules have changed
in the current implementation.''') % version)
        game.version = version
        game.version_tuple = version_tuple
        #
        initial_seed = pload(long)
        if initial_seed <= 32000:
            game.random = LCRandom31(initial_seed)
        else:
            game.random = PysolRandom(initial_seed)
        state = pload()
        game.random.setstate(state)
        #if not hasattr(game.random, "origin"):
        #    game.random.origin = game.random.ORIGIN_UNKNOWN
        game.loadinfo.stacks = []
        game.loadinfo.ncards = 0
        nstacks = pload(int)
        validate(1 <= nstacks, err_txt)
        for i in range(nstacks):
            stack = []
            ncards = pload(int)
            validate(0 <= ncards <= 1024, err_txt)
            for j in range(ncards):
                card_id = pload(int)
                face_up = pload(int)
                stack.append((card_id, face_up))
            game.loadinfo.stacks.append(stack)
            game.loadinfo.ncards = game.loadinfo.ncards + ncards
        validate(game.loadinfo.ncards == game.gameinfo.ncards, err_txt)
        game.loadinfo.talon_round = pload()
        game.finished = pload()
        if 0 <= bookmark <= 1:
            saveinfo = pload(Struct)
            game.saveinfo.__dict__.update(saveinfo.__dict__)
            gsaveinfo = pload(Struct)
            game.gsaveinfo.__dict__.update(gsaveinfo.__dict__)
        moves = pload(Struct)
        game.moves.__dict__.update(moves.__dict__)
        snapshots = pload(list)
        game.snapshots = snapshots
        if 0 <= bookmark <= 1:
            gstats = pload(Struct)
            game.gstats.__dict__.update(gstats.__dict__)
            stats = pload(Struct)
            game.stats.__dict__.update(stats.__dict__)
        game._loadGameHook(p)
        dummy = pload(str)
        validate(dummy == "EOF", err_txt)
        if bookmark == 2:
            # copy back all variables that are not saved
            game.stats = self.stats
            game.gstats = self.gstats
            game.saveinfo = self.saveinfo
            game.gsaveinfo = self.gsaveinfo
        return game


def main():
    init()

    parser = argparse.ArgumentParser()

    parser.add_argument('--input', action='store', dest='input_filename',
            help='the input filename (a PySol saved game)',
            required=True)

    results = parser.parse_args()

    g = Game()
    g._loadGame(results.input_filename)

if __name__ == "__main__":
    main()



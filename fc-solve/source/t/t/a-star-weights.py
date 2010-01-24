#!/usr/bin/env python

import sys
sys.path.append("./t/lib");

from TAP.Simple import *

plan(1)

# TEST
ok(1, "Everything is OK.")

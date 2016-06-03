/*
 * Copyright (c) 2016 Shlomi Fish.
 *
 * Licensed under the MIT/X11 license.
 * */
/*
 * This is a small thunk to trigger the building of the DLL in the relevant
 * source files.
 * */
#pragma once

#ifdef _MSC_VER
#ifndef BUILDING_DLL
#define BUILDING_DLL
#endif
#endif

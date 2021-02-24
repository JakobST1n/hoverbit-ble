/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/
#ifndef SCREEN_H_
#define SCREEN_H_

#include <MicroBit.h>

enum DisplayMainScreenMode { GRAPHS, BATTERY, OFF };

const char* const strBattDead = "\
    000,255,255,255,000\n\
    255,000,255,000,255\n\
    255,255,255,255,255\n\
    000,255,000,255,000\n\
    000,255,000,255,000\n";
const char* const strBattLow = "\
    000,000,255,000,000\n\
    000,255,255,255,000\n\
    000,255,000,255,000\n\
    000,255,000,255,000\n\
    000,255,255,255,000\n";
static const char* const strBattLevel[] = {
    "\
    000,000,255,000,000\n\
    000,255,000,255,000\n\
    000,255,000,255,000\n\
    000,255,000,255,000\n\
    000,255,255,255,000\n",
    "\
    000,000,255,000,000\n\
    000,255,000,255,000\n\
    000,255,000,255,000\n\
    000,255,255,255,000\n\
    000,255,255,255,000\n",
    "\
    000,000,255,000,000\n\
    000,255,000,255,000\n\
    000,255,255,255,000\n\
    000,255,255,255,000\n\
    000,255,255,255,000\n",
    "\
    000,000,255,000,000\n\
    000,255,255,255,000\n\
    000,255,255,255,000\n\
    000,255,255,255,000\n\
    000,255,255,255,000\n"
};
const char* const bluetoothSymbol = "\
    000,000,255,255,000\n\
    255,000,255,000,255\n\
    000,255,255,255,000\n\
    255,000,255,000,255\n\
    000,000,255,255,000\n";

void plotYLine(MicroBit *uBit, int y1, int y2, int x);

#endif // SCREEN_H_

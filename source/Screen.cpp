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
#include "Screen.h"

DisplayMainScreenMode displayMainScreenMode = OFF;

/**
 * Method for plotting a line, gotten from wonder-bit-source.
 */
void plotYLine(int y1, int y2, int x) {
    if (y1 >= y2) {
        for (int y = y2; y <= y1; y++) {
            uBit.display.image.setPixelValue(x, y, 255);
        }
    }
    else if (y1 < y2) {
        for (int y = y1; y <= y2; y++) {
            uBit.display.image.setPixelValue(x, y, 255);
        }
    }
}

/**
 * Draw a line along the X axis
 */
void plotXLine(int x1, int x2, int y) {

     if (x1 >= x2) {
         for (int x = x2; x <= x1; x++) {
             uBit.display.image.setPixelValue(x, y, 255);
         }
     }
     else if (x1 < x2) {
         for (int x = x1; x <= x2; x++) {
             uBit.display.image.setPixelValue(x, y, 255);
         }
     }
 }

/**
 * Display the dead battery icon.
 */
void iconBatteryDead() {
    MicroBitImage img(strBattDead);
    uBit.display.print(img);
}

/**
 * Display the low battery icon.
 */
void iconBatteryLow() {
    MicroBitImage img(strBattLow);
    uBit.display.print(img);
}

/**
 * Show a battery charging icon.
 */
void iconBatteryCharging() {
    int batteryMilliVolt = controller.GetBatteryVoltage();
    int low = 0;
    int high = 3;
    if (batteryMilliVolt >= 4200) {
        low = 3;
    } else if (batteryMilliVolt >= 4040) {
        low = 2;
    } else if (batteryMilliVolt >= 3900) {
        low = 1;
    }

    for (int i = low; i <= high; i++) {
        MicroBitImage img(strBattLevel[i]);
        uBit.display.print(img);
        uBit.sleep(400);
    }
}

/**
 * Display the appropriate battery icon based on battery voltage.
 */
void batteryLevelFullScreen() {
    int batteryMilliVolt = controller.GetBatteryVoltage();
    int level = 0;
    if (controller.Arm()) {
        level = (((batteryMilliVolt - 3400) * 3) / 500);
    } else {
        level = (((batteryMilliVolt - 3700) * 3) / 500);
    }
    if (level < 0) { level = 0; }
    if (level > 3) { level = 3; }
    MicroBitImage img(strBattLevel[level]);
    uBit.display.print(img);
}

/**
 * Get DisplayScreenMode.
 */
DisplayMainScreenMode HoverBitDisplay::mode() {
    return screenMode;
}

/**
 * Set DisplayScreenMode.
 */
void HoverBitDisplay::mode(DisplayMainScreenMode mode) {
    screenMode = mode;
}

/**
 * Go to the next HoverBitDisplay mode.
 */
void HoverBitDisplay::nextMode() {
    uBit.display.clear();
    switch (screenMode) {
        case GRAPHS:
            screenMode = BATTERY;
            break;
        case BATTERY:
            screenMode = OFF;
            break;
        case OFF:
            screenMode = GRAPHS;
            break;
    }
}

/**
 * Pause the HoverBitDisplay module, this means that you are
 * free to print what you want without this module interefering
 */
void HoverBitDisplay::pause() {
    pause(true);
}

/**
 * Turn on or off HoverBitDisplay pause.
 */
void HoverBitDisplay::pause(bool p) {
    if (p) {
        uBit.display.clear();
    }
    isPause = p;
}

/**
 * Set BLE connected state, this determines if the bluetooth icon is
 * shown on screen or not.
 */
void HoverBitDisplay::updateBLEState(bool connected) {
    BLEconnected = connected;
}

/**
 * This updates the matrix display to what should be displayed now.
 */
void HoverBitDisplay::update() {
    if (isPause) { return; }
    int batteryMilliVolt = controller.GetBatteryVoltage();

    flipFrame = (uBit.systemTime() - tmpTimer) > 1000;
    if (flipFrame) { tmpTimer = uBit.systemTime(); }

    if ((((&uBit.io.P0)->getAnalogValue()) < 600) && (((&uBit.io.P0)->getAnalogValue()) >= 400)) {
        iconBatteryCharging();
    } else if (controller.BatteryEmpty() || (batteryMilliVolt < BATTERY_LOW_LIMIT && (&uBit.io.P0)->getAnalogValue() > 300)) {
        lowBattery();
    } else if (!BLEconnected) {
        BLENotConnected();
    } else {
        mainScreen();
    }
}

/**
 * Displays dead or low battery icon.
 */
void HoverBitDisplay::lowBattery() {
    if (controller.BatteryEmpty()) {
        iconBatteryDead();
    } else if (controller.GetBatteryVoltage() > BATTERY_LOW_LIMIT - 50) {
        iconBatteryLow();
    } else {
        iconBatteryDead();
    }
}

/**
 * Called by HoverBitDisplay::update when BLEConnected is false.
 * Flashes a bluetooth symbol on screen.
 */
void HoverBitDisplay::BLENotConnected() {
    if ((((uBit.systemTime() >> (12 - 1) & 1)) == 1)) {
        bBLEIndicator = !bBLEIndicator;
        uBit.display.clear();
        if (bBLEIndicator) {
            MicroBitImage img(bluetoothSymbol);
            uBit.display.print(img);
        } else {
            // Need to actually see this to know if I want to flash only
            // blank screen or with battery.
            //batteryLevelFullScreen();
        }
    }
}

/**
 * Method that does the "default" main screen mode.
 * Called when in a connected "normal" operating state.
 */
void HoverBitDisplay::mainScreen() {
     switch (displayMainScreenMode) {
         case OFF:
             break;
         case BATTERY:
             uBit.display.clear();
             batteryLevelFullScreen();
             break;
         case GRAPHS:
         default:
            showGraphs();
            break;
        }
}

/**
 * Show the GRAPH displayMainScreenMode
 */
void HoverBitDisplay::showGraphs() {
    uBit.display.clear();
    if (controller.Arm()) {
        if (uBit.systemTime() % 500 > 250) {
            uBit.display.image.setPixelValue(0, 0, 255);
        }
    }
    uBit.display.image.setPixelValue(0, (100 - controller.Throttle()) / 25, 255);
    uBit.display.image.setPixelValue((45 - controller.Rudder()) / 18, 2, 255);
    int batteryMilliVolt = controller.GetBatteryVoltage();
    if (batteryMilliVolt > 100) {
        if (controller.Arm()) {
            plotYLine(0, (((batteryMilliVolt - 3400) * 4) / 500), 4);
        } else {
            plotYLine(0, (((batteryMilliVolt - 3700) * 4) / 500), 4);
        }
    } else if (uBit.systemTime() % 500 > 250) {
        uBit.display.image.setPixelValue(4, 4, 255);
    }
}

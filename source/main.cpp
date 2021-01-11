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
#include <MicroBit.h>
#include "MicroBitUARTService.h"
#include "HoverBitController.h"
#include "Screen.h"

MicroBit uBit;
MicroBitUARTService *uart;
HoverBitController controller;

bool bConnected = false;

bool batteryEmpty = false;
bool bCapLogoIsPressed = false;
int batteryMilliVolt = 3700;
unsigned long tmpTimer;
bool bBLEIndicator = false;

DisplayMainScreenMode displayMainScreenMode = GRAPHS;

void onConnected(MicroBitEvent) {
    bConnected = 1;
    uBit.audio.soundExpressions.play(ManagedString("giggle"));

    // mobile app will send ASCII strings terminated with the colon character
    ManagedString eom(":");

    while (bConnected) {
        ManagedString msg = uart->readUntil(eom);
        int length = msg.length();
        const char* command = msg.toCharArray();

        char cCommand = command[0];
        char cChar;
        int startI = 1;
        for (int i = 1; i < length; i++) {
            cChar = command[i];
            if (cChar == 'R' || cChar == 'T' || cChar == 'A' || cChar == 'S' || cChar == ':') {
                int valLength = i - startI;
                char val[valLength];
                for (int o = 0; o < valLength; o++) {
                    val[o] = command[startI + o];
                }
                int value = atoi(val);

                if (cCommand == 'R') {
                    controller.Roll(value);
                } else if (cCommand == 'T') {
                    controller.Throttle(value);
                } else if (cCommand == 'A') {
                    controller.Arm(value == 1);
                } else if (cCommand == 'S') {
                    controller.Servo1(value);
                }

                cCommand = cChar;
                startI = i+1;
            }
        }
    }

}

void onDisconnected(MicroBitEvent) {
    bConnected = 0;
    uBit.audio.soundExpressions.play(ManagedString("sad"));
}

void iconBatteryDead() {
    MicroBitImage img(strBattDead);
    uBit.display.print(img);
}

void iconBatteryLow() {
    MicroBitImage img(strBattLow);
    uBit.display.print(img);
}

void lowBattery() {
    if (batteryEmpty) {
        iconBatteryDead();
    } else if (batteryMilliVolt > BATTERY_LOW_LIMIT - 50){
        iconBatteryLow();
    } else {
        iconBatteryDead();
    }
}

void iconBatteryCharging() {
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

void batteryLevelFullScreen() {
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

void plotYLine(int y1, int y2, int x) {
    /**
     * Draw a line along the Y axis. y1: first pixel, y2: last pixel
     */

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

void nextMainScreenDisplayMode() {
    uBit.display.clear();
    switch (displayMainScreenMode) {
        case GRAPHS:
            displayMainScreenMode = BATTERY;
            break;
        case BATTERY:
            displayMainScreenMode = OFF;
            break;
        case OFF:
            displayMainScreenMode = GRAPHS;
            break;
    }
}

void mainScreen() {
    bool bDelayElapsed = (uBit.systemTime() - tmpTimer) > 1000;
    if (bDelayElapsed) { tmpTimer = uBit.systemTime(); }

    if (bDelayElapsed && bConnected) { uart->send(batteryMilliVolt); }

    switch (displayMainScreenMode) {
        case OFF:
            break;
        case BATTERY:
            uBit.display.clear();
            batteryLevelFullScreen();
            break;
        case GRAPHS:
        default:
            uBit.display.clear();
            if (batteryMilliVolt > 100) {
                if (controller.Arm()) {
                    plotYLine(0, (((batteryMilliVolt - 3400) * 4) / 500), 4);
                } else {
                    plotYLine(0, (((batteryMilliVolt - 3700) * 4) / 500), 4);
                }
            }
            break;
    }

    if (bConnected) {
        uBit.display.image.setPixelValue(0, 0, 255);
    } else {
        if (bDelayElapsed) { bBLEIndicator = !bBLEIndicator; }
        if (bBLEIndicator) {
            uBit.display.image.setPixelValue(0, 0, 0);
        } else {
            uBit.display.image.setPixelValue(0, 0, 255);
        }
    }
}

void onButtonA_press(MicroBitEvent e) {
}
void onButtonB_press(MicroBitEvent e) {
}

int main() {
    uBit.init();
    uBit.audio.setVolume(255);
    tmpTimer = uBit.systemTime();

    // Setup serial for Spektsat communication with air:bit board
    uBit.serial.setBaud(115200);
    uBit.serial.redirect(uBit.io.P1, uBit.io.P2);

    /* Initialize hover:bit controller module
     * the init procedure have to be run within 100ms after air:bit power up */
    controller.init(&uBit);

    // Setup listeners
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButtonA_press);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButtonB_press);

    // uartService
    // Note GATT table size increased from default in MicroBitConfig.h
    // #define MICROBIT_SD_GATT_TABLE_SIZE             0x500
    uart = new MicroBitUARTService(*uBit.ble, 32, 32);

    uBit.audio.soundExpressions.play(ManagedString("hello"));

    while (1) {
        batteryMilliVolt = controller.getBatteryVoltage();

        if (uBit.logo.isPressed()) {
            if (!bCapLogoIsPressed) {
                bCapLogoIsPressed = true;
                nextMainScreenDisplayMode();
            }
        } else if (bCapLogoIsPressed ){
            bCapLogoIsPressed = false;
        }

        if ((((&uBit.io.P0)->getAnalogValue()) < 600) && (((&uBit.io.P0)->getAnalogValue()) >= 400)) {
            iconBatteryCharging();
        } else if (controller.BatteryEmpty() || (batteryMilliVolt < BATTERY_LOW_LIMIT && (&uBit.io.P0)->getAnalogValue() > 300)) {
            lowBattery();
        } else {
            mainScreen();
        }

        controller.HoverControl();
        uBit.sleep(20);
    }

    // If main exits, there may still be other fibers running or registered event handlers etc.
    // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
    // sit in the idle task forever, in a power efficient sleep.
    release_fiber();
}

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

#define VERSION "0.0.1"
#define BLE_UART_DELIM ":"

MicroBit uBit;
MicroBitUARTService *uart;
HoverBitController controller;
HoverBitDisplay hoverBitDisplay;

bool bConnected = false;
bool bCapLogoIsPressed = false;

void onConnected(MicroBitEvent) {
    bConnected = 1;
    hoverBitDisplay.updateBLEState(true);
    uBit.audio.soundExpressions.play(ManagedString("giggle"));
}

void onDisconnected(MicroBitEvent) {
    bConnected = 0;
    hoverBitDisplay.updateBLEState(false);
    uBit.audio.soundExpressions.play(ManagedString("sad"));
}

void onDelim(MicroBitEvent) {
    ManagedString msg = uart->readUntil(BLE_UART_DELIM);

    int length = msg.length();
    const char* command = msg.toCharArray();

    ManagedString accString("ACC:");

    char cCommand = command[0];
    char cChar;
    int startI = 1;
    bool bEOC = false;
    int valLength = 0;

    for (int i = 1; i < length; i++) {
        cChar = command[i];

        if (i >= length - 1) {
            bEOC = true;
            valLength = i - startI + 1;
        } else if (cChar == 'R' || // Roll
                   cChar == 'T' || // Throttle
                   cChar == 'A' || // Arm
                   cChar == 'S' || // (Servo1) Keeping this for compatability
                   cChar == 'D'    // DisplayMainScreenMode
                  ) {
            bEOC = true;
            valLength = i - startI;
        }

        if (bEOC) {
            /* We will just assume that we end up with a valid integer here */
            int value = atoi(msg.substring(startI, startI + valLength).toCharArray());

            if (cCommand == 'R') {
                controller.Rudder(value);
                accString = accString + ManagedString("R") + ManagedString(controller.Rudder());
            } else if (cCommand == 'T') {
                controller.Throttle(value);
                accString = accString + ManagedString("T") + ManagedString(controller.Throttle());
            } else if (cCommand == 'A') {
                controller.Arm(value == 1);
                accString = accString + ManagedString("A") + ManagedString(controller.Arm());
            } else if (cCommand == 'D') {
                switch (value) {
                    case 0:
                        hoverBitDisplay.mode(GRAPHS);
                        break;
                    case 1:
                        hoverBitDisplay.mode(BATTERY);
                        break;
                    case 2:
                        hoverBitDisplay.mode(OFF);
                        break;
                }
            } else {
                // We ignore it :)
            }

            cCommand = cChar;
            startI = i+1;
            bEOC = false;
        }
    }
    // @TODO: Move this to the hoverControl module, we would rather like to have that there, or in the main loop.
    // it could also be in the same clause as the batttery sending, but we might want to have it more
    // dependent on actual received values.
    uart->send(accString);
}

void onButtonA_press(MicroBitEvent e) {
    hoverBitDisplay.nextMode();
}

void onButtonB_press(MicroBitEvent e) {
}

void onButtonAB_press(MicroBitEvent e) {
    hoverBitDisplay.pause();
    uBit.display.scroll(VERSION);
    hoverBitDisplay.pause(false);
}

int main() {
    uBit.init();
    uBit.audio.setVolume(255);

    // Setup serial for Spektsat communication with air:bit board
    uBit.serial.setBaud(115200);
    uBit.serial.redirect(uBit.io.P1, uBit.io.P2);

    /* Initialize hover:bit controller module
     * the init procedure have to be run within 100ms after air:bit power up */
    controller.init();

    // Setup listeners
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE_UART, MICROBIT_UART_S_EVT_DELIM_MATCH, onDelim);

    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButtonA_press);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButtonB_press);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, onButtonAB_press);

    // uartService
    // Note GATT table size increased from default in MicroBitConfig.h
    // #define MICROBIT_SD_GATT_TABLE_SIZE             0x500
    uart = new MicroBitUARTService(*uBit.ble, 32, 32);
    uart->eventOn(BLE_UART_DELIM);

    uBit.audio.soundExpressions.play(ManagedString("hello"));

    while (1) {
        if (uBit.logo.isPressed()) {
            if (!bCapLogoIsPressed) {
                bCapLogoIsPressed = true;
                hoverBitDisplay.nextMode();
            }
        } else if (bCapLogoIsPressed) {
            bCapLogoIsPressed = false;
        }

        hoverBitDisplay.update();
        controller.HoverControl();
        uBit.sleep(20);
    }

    // If main exits, there may still be other fibers running or registered event handlers etc.
    // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
    // sit in the idle task forever, in a power efficient sleep.
    release_fiber();
}

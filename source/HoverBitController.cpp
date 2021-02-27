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
#include "HoverBitController.h"

/**
 * Init method for HoverBitController, this sets everything to the default values.
 * It also initializes the airbit-pcb with some protocol magic.
 *
 * @param _uBit the MicroBit instance
 */
void HoverBitController::init() {
    mainController = false;
    batteryEmpty   = false;
    batteryMilliVolt = 3700;
    batteryFactor   = 4.42;

    buzzer  = 0;
    servo_1 = 0;
    arm = 0;
    roll = 0;
    yaw = 0;
    throttle = 0;
    lastReceiveTime = uBit.systemTime();

    /* I am not completly sure what this does, but it seems to me like this is
       putting the air:bit board in some kind of "bind-mode", on the spec-sheet
       there isn't any documentation for what 20 pulses means tho... */
    uBit.sleep(100);
    int o;
    for (o = 0; o < 20; o++) {
        AirBit(-90, 0, 90, 0, 90, 0, 0);
        uBit.sleep(20);
    }
}

/**
 * This is not implemented yet.
 */
bool HoverBitController::failSafe(void) {
    unsigned long deltaReceiveTime = uBit.systemTime() - lastReceiveTime;
    if (deltaReceiveTime > FSAFE_TLIM_THROTTLE) {
        Throttle(0);
        Rudder(0);
        AirBit(0, arm, 0, throttle, roll, roll + 45, servo_1);
    }
    if (deltaReceiveTime > FSAFE_TLIM_ARM) {
        Arm(0);
        AirBit(0, arm, 0, throttle, roll, roll + 45, servo_1);
    }
    return (deltaReceiveTime > FSAFE_TLIM_THROTTLE) || (deltaReceiveTime > FSAFE_TLIM_ARM);
}

/**
 * This returns the current voltage of the battery.
 */
unsigned int HoverBitController::GetBatteryVoltage() {
    float batteryFactor = 4.42;
    int batteryMilliVolt = 3700;
    return ((float)((&uBit.io.P0)->getAnalogValue()) * batteryFactor * 0.05) + ((float)batteryMilliVolt * 0.95);
}

/**
 * Method for sending commands to the AirBit-card,
 * this code is translated from the ts-code in MakeKit's original hex-file.
 *
 * Control TYPR12 (Throttle, Yaw, Pitch, Roll and AUX1 and AUX2) using the Spektsat 2048 protocol
 * Throttle min: 0, max: 100
 * Yaw, Pitch Roll: min -90, max 90
 * Arm: 0 = Disarm, 1 = Arm
 * Aux1: 0 - 180
 * Aux2: 0 - 180
 */
void HoverBitController::AirBit(int Pitch,int Arm,int Roll,int Throttle,int Yaw,int Aux1,int Aux2) {
    uint8_t buf[16];
    float scaling = 1024 / 180;
    int offset = 512;
    float scalingServo = 1024 / 90;

    unsigned int armS = 0;
    if (Arm == 0) { armS = 0; }
    if (Arm == 1) { armS = 1023; }

    Pitch = - Pitch;
    unsigned int aux1S     = Aux1  * scalingServo;
    unsigned int aux2S     = Aux2  * scalingServo;
    unsigned int pitchS    = static_cast<unsigned int>((float)Pitch * scaling + (float)offset);
    unsigned int rollS     = static_cast<unsigned int>((float)Roll  * scaling + (float)offset);
    unsigned int yawS      = static_cast<unsigned int>((float)Yaw   * scaling + (float)offset);
    unsigned int throttleS = (Throttle * 512) / 50;
    if (Throttle == 0) { throttleS = 0; }

    if (aux1S > 1023) { aux1S = 1023; }
    if (aux2S > 1023) { aux2S = 1023; }

    if (throttleS > 1023) { throttleS = 1023; }
    if (yawS > 1023) { yawS = 1023; }
    if (pitchS > 1023) { pitchS = 1023; }
    if (rollS > 1023) { rollS = 1023; }

    // Header "Fade" (Spektsat code)
    buf[0] = 0;
    // Header "System" (Spektsat code)
    buf[1] = 0x01;
    // 0x01 22MS 1024 DSM2
    // 0x12 11MS 2048 DSM2
    // 0xa2 22MS 2048 DSMS
    // 0xb2 11MS 2048 DSMX
    buf[2]  = (0 << 2) | ((rollS >> 8) & 3);
    buf[3]  = rollS & 255;
    buf[4]  = (1 << 2) | ((pitchS >> 8) & 3);
    buf[5]  = pitchS & 255;
    buf[6]  = (2 << 2) | ((throttleS >> 8) & 3);
    buf[7]  = throttleS & 255;
    buf[8]  = (3 << 2) | ((yawS >> 8) & 3);
    buf[9]  = yawS & 255;
    buf[10] = (4 << 2) | ((armS >> 8) & 3);
    buf[11] = armS & 255;
    buf[12] = (5 << 2) | ((aux1S >> 8) & 3);
    buf[13] = aux1S & 255;
    buf[14] = (6 << 2) | ((aux2S >> 8) & 3);
    buf[15] = aux2S & 255;
    uBit.serial.send(buf, 16, SYNC_SPINWAIT);
}

/**
 * Method that sends commands with the current values for all parameters.
 */
void HoverBitController::HoverControl() {
    if (!failSafe()) {
        AirBit(0, arm, 0, throttle, roll, roll + 45, servo_1);
    }
}

int HoverBitController::Throttle() {
    return throttle;
}
void HoverBitController::Throttle(int _throttle) {
    if      (_throttle > 99) { throttle = 100; }
    else if (_throttle <  0) { throttle =   0; }
    else { throttle = _throttle; }
    lastReceiveTime = uBit.systemTime();
}
int HoverBitController::Rudder() {
    // The AirBit uses the roll parameter to control the hoverbit's rudder.
    return roll;
}
void HoverBitController::Rudder(int _rudder) {
    // The AirBit uses the roll parameter to control the hoverbit's rudder.
    if      (_rudder >   90) { roll =  90; }
    else if (_rudder <  -90) { roll = -90; }
    else    { roll = _rudder; }
    lastReceiveTime = uBit.systemTime();
}
bool HoverBitController::Arm() {
    return (arm == 1);
}
void HoverBitController::Arm(bool _arm) {
    arm = (int)_arm;
    lastReceiveTime = uBit.systemTime();
}
bool HoverBitController::BatteryEmpty() {
    return batteryEmpty;
}

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
#ifndef HOVERBITCONTROLLER_H_
#define HOVERBITCONTROLLER_H_

#include <MicroBit.h>

#define BATTERY_LOW_LIMIT   3500
#define FSAFE_TLIM_THROTTLE 1000 // When to cut the throttle
#define FSAFE_TLIM_ARM      5000 // When to disarm

extern MicroBit uBit;

/**
 * This class can be used to interface with a AirBit card for controlling a HOVER:BIT kit.
 *
 * A lot of the features of the airbit is ignored here and made easy to understand if all
 * you want to do is use it for a hoverbit.
 */
class HoverBitController {
    private:
        int buzzer;
        int servo_1;
        int arm;
        int roll;
        int pitch;
        int yaw;
        int throttle;
        unsigned long lastReceiveTime;

        bool mainController;
        bool bBatteryEmpty;
        int batteryMilliVolt;
        float batteryFactor;

        bool failSafe(void);
        void checkBattery();
        void AirBit(int Pitch,int Arm,int Roll,int Throttle,int Yaw,int Aux1,int Aux2);

    public:
        void init();
        unsigned int GetBatteryVoltage(void);
        void HoverControl();

        int  Throttle();
        void Throttle(int _throttle);
        int  Rudder();
        void Rudder(int _rudder);
        bool Arm();
        void Arm(bool _arm);
        bool BatteryEmpty();
};

#endif // HOVERBITCONTROLLER_H_

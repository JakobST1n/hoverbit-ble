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

class HoverBitController {
    private:
        MicroBit* uBit;

        int buzzer;
        int servo_1;
        int arm;
        int roll;
        int pitch;
        int yaw;
        int throttle;
        unsigned long receiveTime;

        bool mainController;
        bool batteryEmpty;
        int batteryMilliVolt;
        float batteryFactor;

        bool failSafe(void);

    public:
        void init(MicroBit* _uBit);
        unsigned int getBatteryVoltage(void);
        void AirBit(int Pitch,int Arm,int Roll,int Throttle,int Yaw,int Aux1,int Aux2);
        void HoverControl();

        int  Throttle();
        void Throttle(int _throttle);
        int  Servo1();
        void Servo1(int _servo1);
        int  Roll();
        void Roll(int _roll);
        bool Arm();
        void Arm(bool _arm);
        bool BatteryEmpty();
};

#endif // HOVERBITCONTROLLER_H_

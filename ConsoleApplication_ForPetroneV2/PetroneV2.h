/*
The MIT License (MIT)

Copyright (c) 2017 BYROBOT (www.byrobot.co.kr)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <sal.h>
#include <atlstr.h>

#pragma once
class PetroneV2
{
public:
    PetroneV2();
    virtual ~PetroneV2();

private:
    HANDLE      hSerial;    // serial port handle
    CString     portname;

    bool        flagOpen;

private:
    BOOL        write(BYTE * dataArray, DWORD length);

public:
    bool        open(CString portname);
    void        close();

    bool        isOpen();

    bool        sendFlightEvent(unsigned char flightEvent, int delay = 0);
    bool        sendTakeOff(int delay = 0);
    bool        sendLanding(int delay = 0);
    bool        sendStop(int delay = 0);

    bool        sendControl(signed char roll, signed char pitch, signed char yaw, signed char throttle, int delay = 0);
};



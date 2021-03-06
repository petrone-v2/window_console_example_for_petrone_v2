// ConsoleApplication_ForPetroneV2.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//
#include <Windows.h>

#include "stdafx.h"
#include "PetroneV2.h"

int main()
{
    PetroneV2 drone;

    drone.open("\\\\.\\COM22");

    if (drone.isOpen())
    {
        drone.sendTakeOff(3000);
        drone.sendControl(0, 30, 0, 0, 3000);
        drone.sendControl(0, 0, 0, 0, 2000);
        drone.sendLanding(2000);
    }

    Sleep(1000);

    return 0;
}


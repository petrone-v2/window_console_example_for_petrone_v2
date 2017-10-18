#include <time.h>

#include "stdafx.h"
#include "PetroneV2.h"
#include "CRC16.h"

PetroneV2::PetroneV2()
{
	hSerial = INVALID_HANDLE_VALUE;
}


PetroneV2::~PetroneV2()
{
	close();
}

bool PetroneV2::isOpen()
{
	return hSerial != INVALID_HANDLE_VALUE;
}

bool PetroneV2::open(CString portname)
{
	close();

	// Serial port open
	{
		this->portname = portname;

		hSerial = CreateFile(portname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if (hSerial == INVALID_HANDLE_VALUE)
		{
			return false;
		}
	}

	// DCB
	{
		DCB dcb;

		memset(&dcb, 0, sizeof(DCB));
		FillMemory(&dcb, sizeof(dcb), 0);
		dcb.DCBlength = sizeof(DCB);

		GetCommState(hSerial, &dcb);

		dcb.BaudRate = CBR_115200;
		dcb.Parity = NOPARITY;
		dcb.ByteSize = 8;
		dcb.StopBits = ONESTOPBIT;
		dcb.fDsrSensitivity = FALSE;
		dcb.fOutxCtsFlow = FALSE;
		dcb.fOutxDsrFlow = FALSE;
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;

		SetCommState(hSerial, &dcb);

		SetCommMask(hSerial, EV_RXCHAR);
	}
}

void PetroneV2::close()
{
	if (isOpen())
	{
		CloseHandle(hSerial);
		hSerial = INVALID_HANDLE_VALUE;
	}
}

void PetroneV2::sleep(unsigned int mseconds)
{
	clock_t goal = mseconds + clock();
	while (goal > clock());
}

DWORD PetroneV2::write(BYTE * dataArray, int length)
{
	DWORD sizeOfWritten = 0;

	if (isOpen())
	{
		WriteFile(hSerial, dataArray, length, &sizeOfWritten, NULL);
	}

	return sizeOfWritten;
}

bool PetroneV2::sendControl(signed char roll, signed char pitch, signed char yaw, signed char throttle)
{
	BYTE dataArray[12];
	BYTE i = 0;

	// start code
	dataArray[i++] = 0x0A;
	dataArray[i++] = 0x55;

	// header
	dataArray[i++] = 0x10;
	dataArray[i++] = 0x04;
	dataArray[i++] = 0x33;
	dataArray[i++] = 0x30;

	// body
	dataArray[i++] = roll;
	dataArray[i++] = pitch;
	dataArray[i++] = yaw;
	dataArray[i++] = throttle;

	// crc
	WORD crc16 = CRC16::calc(&dataArray[2], (i - 2));
	dataArray[i++] = ((crc16 >> 8) & 0xFF);
	dataArray[i++] = (crc16 & 0xFF);

	int sizeOfWritten = write(dataArray, i);

	{
		printf("sendControl / ");

		for (int j = 0; j++; j < i)
		{
			printf("%2X ", dataArray[i]);
		}

		printf("\r\n");
	}

	if (sizeOfWritten == 0)
		return false;
	else
		return true;
}

bool PetroneV2::sendFlightEvent(unsigned char flightEvent)
{
	BYTE dataArray[10];
	BYTE i = 0;

	// start code
	dataArray[i++] = 0x0A;
	dataArray[i++] = 0x55;

	// header
	dataArray[i++] = 0x11;
	dataArray[i++] = 0x02;
	dataArray[i++] = 0x33;
	dataArray[i++] = 0x30;

	// body
	dataArray[i++] = 0x22;
	dataArray[i++] = flightEvent;

	// crc
	WORD crc16 = CRC16::calc(&dataArray[2], (i - 2));
	dataArray[i++] = ((crc16 >> 8) & 0xFF);
	dataArray[i++] = (crc16 & 0xFF);

	int sizeOfWritten = write(dataArray, i);

	{
		printf("sendFlightEvent / ");

		for (int j = 0; j++; j < i)
		{
			printf("%2X ", dataArray[i]);
		}

		printf("\r\n");
	}

	if (sizeOfWritten == 0)
		return false;
	else
		return true;
}

bool PetroneV2::sendTakeOff()
{
	return sendFlightEvent(0x11);
}

bool PetroneV2::sendLanding()
{
	return sendFlightEvent(0x12);
}

bool PetroneV2::sendStop()
{
	return sendFlightEvent(0x10);
}



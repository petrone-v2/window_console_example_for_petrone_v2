#include <iostream>
#include <ctime>
#include <Windows.h>

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

		//hSerial = CreateFile(portname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		hSerial = CreateFile(portname, GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
		if (hSerial == INVALID_HANDLE_VALUE)
		{
			DWORD dwLastError = GetLastError();
			printf("CSerialPort::Open, Failed to open the comms port, Error:%u\n", dwLastError);
			return false;
		}
	}

	// DCB
	{
		DCB dcb;

		//memset(&dcb, 0, sizeof(DCB));
		FillMemory(&dcb, sizeof(dcb), 0);
		dcb.DCBlength = sizeof(DCB);

		//GetCommState(hSerial, &dcb);
		if (!GetCommState(hSerial, &dcb))
		{
			DWORD dwLastError = GetLastError();
			printf("CSerialPort::GetState, Failed in call to GetCommState, Error:%u\n", dwLastError);
		}

		dcb.BaudRate = CBR_115200;
		dcb.Parity = NOPARITY;
		dcb.ByteSize = 8;
		dcb.StopBits = ONESTOPBIT;
		//dcb.fDsrSensitivity = FALSE;
		dcb.fOutxCtsFlow = FALSE;
		dcb.fOutxDsrFlow = FALSE;
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;

		//SetCommState(hSerial, &dcb);

		//SetCommMask(hSerial, EV_RXCHAR);

		if (!SetCommState(hSerial, &dcb))
		{
			DWORD dwLastError = GetLastError();
			printf("CSerialPort::SetState, Failed in call to SetCommState, Error:%u\n", dwLastError);
		}
	}

	return true;
}

void PetroneV2::close()
{
	if (isOpen())
	{
		CloseHandle(hSerial);
		hSerial = INVALID_HANDLE_VALUE;
	}
}

// https://msdn.microsoft.com/en-us/library/ms810467.aspx
BOOL PetroneV2::write(BYTE * dataArray, DWORD length)
{
	OVERLAPPED osWrite = { 0 };
	DWORD dwRes;
	DWORD sizeOfWritten;
	BOOL fRes;

	// Create this write operation's OVERLAPPED structure's hEvent.
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL)
	{
		// error creating overlapped event handle
		return FALSE;
	}

	// Issue write.
	if (!WriteFile(hSerial, dataArray, length, &sizeOfWritten, &osWrite))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			// WriteFile failed, but isn't delayed. Report error and abort.
			fRes = FALSE;
		}
		else
		{
			// Write is pending.
			dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
			switch (dwRes)
			{
				// OVERLAPPED structure's event has been signaled. 
			case WAIT_OBJECT_0:
				if (!GetOverlappedResult(hSerial, &osWrite, &sizeOfWritten, FALSE))
					fRes = FALSE;
				else
					// Write operation completed successfully.
					fRes = TRUE;
				break;

			default:
				// An error has occurred in WaitForSingleObject.
				// This usually indicates a problem with the
				// OVERLAPPED structure's event handle.
				fRes = FALSE;
				break;
			}
		}
	}
	else
	{
		// WriteFile completed immediately.
		fRes = TRUE;
	}

	CloseHandle(osWrite.hEvent);
	return fRes;
}

bool PetroneV2::sendControl(signed char roll, signed char pitch, signed char yaw, signed char throttle, int delay)
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
	dataArray[i++] = (crc16 & 0xFF);
	dataArray[i++] = ((crc16 >> 8) & 0xFF);
	
	if(write(dataArray, i))
	{
		printf("sendControl / ");

		for (int j = 0; j < i; j++)
		{
			printf("%02X ", dataArray[j]);
		}

		printf("\r\n");

		Sleep(delay);
		return true;
	}

	return false;
}

bool PetroneV2::sendFlightEvent(unsigned char flightEvent, int delay)
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
	dataArray[i++] = (crc16 & 0xFF);
	dataArray[i++] = ((crc16 >> 8) & 0xFF);

	if(write(dataArray, i))
	{
		printf("sendFlightEvent / ");

		for (int j = 0; j < i; j++)
		{
			printf("%02X ", dataArray[j]);
		}

		printf("\r\n");

		Sleep(delay);
		return true;
	}

	return false;
}

bool PetroneV2::sendTakeOff(int delay)
{
	return sendFlightEvent(0x11, delay);
}

bool PetroneV2::sendLanding(int delay)
{
	return sendFlightEvent(0x12, delay);
}

bool PetroneV2::sendStop(int delay)
{
	return sendFlightEvent(0x10, delay);
}



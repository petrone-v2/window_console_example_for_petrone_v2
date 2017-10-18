#pragma once
class CRC16
{
private:
	static const unsigned short crc16table[256];		// CRC16 값으로 변환하기 위한 참조 배열

public:
	static unsigned short calc(unsigned char data, unsigned short crc = 0);									// byte 값을 입력하면 CRC16 값을 반환
	static unsigned short calc(const unsigned char * data, unsigned char length, unsigned short crc = 0);	// byte 배열과 길이를 입력하면 CRC16 값을 반환
};

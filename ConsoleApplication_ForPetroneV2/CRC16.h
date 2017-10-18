#pragma once
class CRC16
{
private:
	static const unsigned short crc16table[256];		// CRC16 ������ ��ȯ�ϱ� ���� ���� �迭

public:
	static unsigned short calc(unsigned char data, unsigned short crc = 0);									// byte ���� �Է��ϸ� CRC16 ���� ��ȯ
	static unsigned short calc(const unsigned char * data, unsigned char length, unsigned short crc = 0);	// byte �迭�� ���̸� �Է��ϸ� CRC16 ���� ��ȯ
};

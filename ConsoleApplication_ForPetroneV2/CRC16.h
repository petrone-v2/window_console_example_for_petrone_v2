#pragma once
class CRC16
{
private:
    static const unsigned short crc16table[256];        // CRC16 °ª8·?º???±?§? uv ¹迭

public:
    static unsigned short calc(unsigned char data, unsigned short crc = 0);                                 // byte °ª; T·C??RC16 °ª; ¹??
    static unsigned short calc(const unsigned char * data, unsigned char length, unsigned short crc = 0);   // byte ¹迭°???| T·C??RC16 °ª; ¹??
};

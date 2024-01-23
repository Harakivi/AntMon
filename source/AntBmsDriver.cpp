#include "AntBmsDriver.hpp"
#include <string.h>

using namespace Drivers;

struct LSB16
{
    uint8_t _0;
    uint8_t _1;
};

struct LSB32
{
    uint8_t _0;
    uint8_t _1;
    uint8_t _2;
    uint8_t _3;
};

AntBmsDriver::AntBmsDriver(uint32_t updatePeriod_ms, InternalPeriph::iUart &uart) : _liveData{0}, _liveDataWritePos(0), _liveDataValid(false), _updatePeriod_ms(updatePeriod_ms), _lastUpdateTime(0), _uart(uart)
{
    _uart.Open(19200, this);
}

const bool &AntBmsDriver::GetValidOfLastLiveData()
{
    return _liveDataValid;
}

const AntLiveData &AntBmsDriver::GetLastLiveData()
{
    return _liveData;
}

void AntBmsDriver::Loop(uint32_t time)
{
    if (!_lastUpdateTime || (time - _lastUpdateTime) > _updatePeriod_ms)
    {
        _lastUpdateTime = time;
        _RequestLiveData();
    }
    _UpdateLiveData();
}

void AntBmsDriver::_RequestLiveData()
{
    _liveDataWritePos = 0;
    _liveDataValid = false;
    uint8_t packetToSend[] = {0x5A, 0x5A, 0x00, 0x00, 0x00, 0x00};
    _uart.Write(packetToSend, sizeof(packetToSend));
}

void AntBmsDriver::_UpdateLiveData()
{
    if (_liveDataWritePos == 140)
    {
        _liveData.Struct.CheckSum = _ReverseLSB(_liveData.Struct.CheckSum);
        if (_CalcCheckSum(_liveData) == _liveData.Struct.CheckSum)
        {
            _liveData.Struct.TotalVoltage = _ReverseLSB(_liveData.Struct.TotalVoltage);
            _liveData.Struct.Current = _ReverseLSB(_liveData.Struct.Current);
            _liveData.Struct.RemainCapacity = _ReverseLSB(_liveData.Struct.RemainCapacity);
            _liveData.Struct.SystemLogs.Data = _ReverseLSB(_liveData.Struct.SystemLogs.Data);
            _liveDataValid = true;
        }
        _liveDataWritePos = 0;
    }
}

void AntBmsDriver::onByteReceived(uint8_t data)
{
    _liveData.Array[_liveDataWritePos++] = data;
}

uint16_t AntBmsDriver::_CalcCheckSum(const AntLiveData &liveData)
{
    uint16_t checksum = {0};
    for (uint8_t i = 4; i < 138; i++)
    {
        checksum += liveData.Array[i];
    }
    return checksum;
}

uint16_t __attribute__((always_inline)) AntBmsDriver::_ReverseLSB(uint16_t toReverse)
{
    return (uint16_t)(((LSB16 *)&toReverse)->_0 << 8) + ((LSB16 *)&toReverse)->_1;
}

uint32_t __attribute__((always_inline)) AntBmsDriver::_ReverseLSB(uint32_t toReverse)
{
    return (uint32_t)(((LSB32 *)&toReverse)->_0 << 24) + (((LSB32 *)&toReverse)->_1 << 16) + (((LSB32 *)&toReverse)->_2 << 8) + ((LSB32 *)&toReverse)->_3;
}

int32_t __attribute__((always_inline)) AntBmsDriver::_ReverseLSB(int32_t toReverse)
{
    return (int32_t)(((LSB32 *)&toReverse)->_0 << 24) + (((LSB32 *)&toReverse)->_1 << 16) + (((LSB32 *)&toReverse)->_2 << 8) + ((LSB32 *)&toReverse)->_3;
}
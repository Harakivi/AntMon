#include "AntBms.hpp"

using namespace Drivers;

AntBms::AntBms(InternalPeriph::iUart *uart) : _uart(uart)
{
    if (_uart != nullptr)
    {
        _uart->Open(19200);
    }
}

AntLiveData AntBms::ReadLiveData(bool &valid)
{
    valid = false;
    AntLiveData liveData = {0};
    uint8_t packetToSend[] = {0x5A, 0x5A, 0x00, 0x00, 0x00, 0x00};
    _uart->Write(packetToSend, sizeof(packetToSend));
    uint8_t readed = 0;
    uint32_t startTick = HAL_GetTick();
    while (readed != 140)
    {
        readed += _uart->GetReceived(liveData.Array + readed, sizeof(liveData) - readed);
        if ((HAL_GetTick() - startTick) > 1000)
        {
            return liveData;
        }
    }
    if(_calcCheckSum(liveData) == liveData.Struct.CheckSum)
    {
        valid = true;
    }
    return liveData;
}

uint16_t AntBms::_calcCheckSum(const AntLiveData& liveData)
{
    uint8_t sum[2] = {0};
    for (uint8_t i = 4; i < 138; i++)
    {
        *(uint16_t*)sum += liveData.Array[i];
    }
    uint16_t checksum = (sum[0] << 8) + sum[1]; // big endian
    return checksum;
}
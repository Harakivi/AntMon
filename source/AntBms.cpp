#include "AntBms.hpp"

using namespace Drivers;

AntBms::AntBms(InternalPeriph::iUart *uart) : _uart(uart)
{
    if (_uart != nullptr)
    {
        _uart->Open(19200);
    }
}

bool AntBms::UpdateLiveData()
{
    uint8_t liveData[140] = {0};
    uint8_t packetToSend[] = {0x5A, 0x5A, 0x00, 0x00, 0x00, 0x00};
    _uart->Write(packetToSend, sizeof(packetToSend));
    uint8_t readed = 0;
    while (readed != 140)
    {
        readed += _uart->GetReceived(liveData + readed, sizeof(liveData) - readed);
    }
    return true;
}
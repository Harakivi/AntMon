#pragma once
#include "uart.hpp"
#include <stdint.h>

namespace Drivers
{

#pragma pack(push, 1)
    typedef struct
    {
        uint8_t FrameHeader[4];
        uint8_t Voltage[65];
        uint32_t Current;
        uint8_t RemainPercentage;
        uint32_t PhysCapacuty;
        uint32_t RemainCapacity;
        uint32_t CycleCapacity;
        uint32_t AccBootCapacity;
        uint8_t ActualTemp[11];
        uint8_t : 8;
        uint8_t : 8;
        uint8_t : 8;
        uint16_t TireLen;
        uint16_t PulsePerWeek;
        uint8_t : 8;
        uint32_t CurrPower;
        uint8_t : 8;
        uint16_t HighestMonomer;
        uint8_t : 8;
        uint16_t LowestMonomer;
        uint16_t Average;
        uint8_t EffBattNum;
        uint16_t : 16;
        uint16_t : 16;
        uint16_t : 16;
        uint16_t : 16;
        uint32_t : 32;
        uint16_t : 16;
        uint16_t SystemLogs;
        uint16_t CheckSum;
    } AntLiveDataStruct;
#pragma pack(pop)

    typedef union
    {
        AntLiveDataStruct Struct;
        uint8_t Array[140];
    } AntLiveData;


    class AntBms
    {
    private:
        AntBms(){}
        InternalPeriph::iUart *_uart;
        uint16_t _calcCheckSum(const AntLiveData& liveData);
    public:
        AntBms(InternalPeriph::iUart *uart);
        AntLiveData ReadLiveData(bool &valid);
    };
}
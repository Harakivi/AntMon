#pragma once
#include "uart.hpp"
#include <stdint.h>

namespace Drivers
{

#pragma pack(push, 1)

    typedef struct 
    {
        uint16_t Status : 5;
        uint16_t BatteryNumber : 5;
        uint16_t SequentialOrder : 5;
        bool Charge : 1;
    }_AntBmsSystemLog;

    typedef union 
    {
        uint16_t Data;
        _AntBmsSystemLog Struct;

    }AntBmsSystemLog;
    

    typedef struct
    {
        uint8_t FrameHeader[4];
        uint16_t TotalVoltage;
        uint16_t Voltage[32];
        int32_t Current;
        uint8_t SOC;
        uint32_t PhysCapacuty;
        uint32_t RemainCapacity;
        uint32_t CycleCapacity;
        uint32_t UpTime;
        int16_t ActualTemp[6];
        uint8_t : 8;
        uint8_t : 8;
        uint8_t : 8;
        uint16_t TireLen;
        uint16_t PulsePerWeek;
        uint8_t : 8;
        uint32_t CurrPower;
        uint8_t  HighestCellNum;
        uint16_t HighestCellVoltage;
        uint8_t LowestCellNum;
        uint16_t LowestCellVoltage;
        uint16_t Average;
        uint8_t CellCnt;
        uint16_t : 16;
        uint16_t : 16;
        uint16_t : 16;
        uint16_t : 16;
        uint32_t CellsBalancedBitMask;
        AntBmsSystemLog SystemLogs;
        uint16_t CheckSum;
    } AntLiveDataStruct;
#pragma pack(pop)

    typedef union
    {
        AntLiveDataStruct Struct;
        uint8_t Array[140];
    } AntLiveData;


    class AntBmsDriver: public InternalPeriph::UartHandler
    {
    private:
        AntLiveData _liveData;
        uint8_t _liveDataWritePos;
        bool _liveDataValid;
        uint32_t _updatePeriod_ms;
        uint32_t _lastUpdateTime;
        InternalPeriph::iUart &_uart;
        void _UpdateLiveData();
        void _RequestLiveData();
        uint16_t _CalcCheckSum(const AntLiveData& liveData);
        inline uint16_t _ReverseLSB(uint16_t toReverse);
        inline uint32_t _ReverseLSB(uint32_t toReverse);
    public:
        AntBmsDriver(uint32_t updatePeriod_ms, InternalPeriph::iUart &uart);
        const bool& GetValidOfLastLiveData();
        const AntLiveData& GetLastLiveData();
        void Loop(uint32_t time);
        void onByteReceived(uint8_t data);
    };
}
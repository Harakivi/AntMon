#include "DeviceClass.hpp"

using namespace DeviceClass;

const uint16_t AntBmsMonitor::dacTable[] = {2400, 2750, 2925, 3075, 3350};

AntBmsMonitor::AntBmsMonitor(Drivers::AntBmsDriver &bmsDriver, Drivers::Cli &cli, InternalPeriph::iDac &dac) : _bmsDriver(bmsDriver), _cli(cli), _dac(dac), _chargeBlink(false), _chargeDetectTime(0)
{
    static Drivers::AntBmsDriver &constBmsDriver = bmsDriver;
    static Drivers::Cli &constCli = cli;
    static InternalPeriph::iDac &constDac = dac;

    static auto printSOC = []()
    {
        if (constBmsDriver.GetValidOfLastLiveData())
        {
            constCli.clear();
            constCli.clearHeader();
            if (constBmsDriver.GetLastDispData().SOC >= 75)
                constCli.printHeader("\033[92m");
            else if (constBmsDriver.GetLastDispData().SOC < 75 && constBmsDriver.GetLastDispData().SOC >= 35)
                constCli.printHeader("\033[93m");
            else if (constBmsDriver.GetLastDispData().SOC < 35)
                constCli.printHeader("\033[91m");
            constCli.printHeader("========================\r\n");
            constCli.printHeader("SOC = %d%\r\n", constBmsDriver.GetLastDispData().SOC);
            constCli.printHeader("Total Voltage = %3.1f\r\n", constBmsDriver.GetLastDispData().TotalVoltage);
            constCli.printHeader("Current = %3.1fA\r\n", constBmsDriver.GetLastDispData().Current);
            constCli.printHeader("Remain capacity = %3.6fA*h\r\n", constBmsDriver.GetLastDispData().RemainCapacity);
            constCli.printHeader("DAC ch1 Value = %d\r\n", constDac.GetValue());
            constCli.printHeader("========================\r\n");
        }
    };

    static auto stateSOC = []()
    {
        constCli.setHeaderUpdater(printSOC);
    };

    cli.AddCmd(cmd_t{"print_soc", NULL, stateSOC, NULL});

    dac.ChannelInit();
}

void AntBmsMonitor::Loop(uint32_t time)
{
    static uint32_t lastBlinkTime = time;
    static uint32_t lastTimeChargeDetect = time;
    if (_bmsDriver.GetValidOfLastDispData())
    {
        if(_bmsDriver.GetLastDispData().Current < -0.9 && _chargeDetectTime < 5000)
        {
            _chargeDetectTime += time - lastTimeChargeDetect;
        }
        else if(_bmsDriver.GetLastDispData().Current >= -0.9)
        {
            _chargeDetectTime = 0;
        }
        if ((_chargeDetectTime >= 5000) && (time - lastBlinkTime > 1000))
        {
            _chargeBlink = !_chargeBlink;
            lastBlinkTime = time;
        }
        else if (_chargeDetectTime == 0)
        {
            _chargeBlink = false;
        }

        if (_bmsDriver.GetLastLiveData().Struct.SOC >= 10 && _bmsDriver.GetLastLiveData().Struct.SOC < 25)
            _dac.SetValue(dacTable[1 - _chargeBlink]);
        else if (_bmsDriver.GetLastLiveData().Struct.SOC >= 25 && _bmsDriver.GetLastLiveData().Struct.SOC < 50)
            _dac.SetValue(dacTable[2 - _chargeBlink]);
        else if (_bmsDriver.GetLastLiveData().Struct.SOC >= 50 && _bmsDriver.GetLastLiveData().Struct.SOC < 75)
            _dac.SetValue(dacTable[3 - _chargeBlink]);
        else if (_bmsDriver.GetLastLiveData().Struct.SOC >= 75 && _bmsDriver.GetLastLiveData().Struct.SOC <= 100)
            _dac.SetValue(dacTable[4 - _chargeBlink]);
        else
            _dac.SetValue(dacTable[0 + _chargeBlink]);
    }
    lastTimeChargeDetect = time;
}

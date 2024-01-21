#include "DeviceClass.hpp"

using namespace DeviceClass;

const uint16_t AntBmsMonitor::dacTable[] = {2400, 2750, 2925, 3075, 3350};

AntBmsMonitor::AntBmsMonitor(Drivers::AntBmsDriver &bmsDriver, Drivers::Cli &cli, InternalPeriph::iDac &dac) : _bmsDriver(bmsDriver), _cli(cli), _dac(dac), _chargeBlink(false)
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
            if (constBmsDriver.GetLastLiveData().Struct.SOC >= 75)
                constCli.printHeader("\033[92m");
            else if (constBmsDriver.GetLastLiveData().Struct.SOC < 75 && constBmsDriver.GetLastLiveData().Struct.SOC >= 35)
                constCli.printHeader("\033[93m");
            else if (constBmsDriver.GetLastLiveData().Struct.SOC < 35)
                constCli.printHeader("\033[91m");
            constCli.printHeader("========================\r\n");
            constCli.printHeader("SOC = %d%\r\n", constBmsDriver.GetLastLiveData().Struct.SOC);
            constCli.printHeader("Total Voltage = %d.%dV\r\n", constBmsDriver.GetLastLiveData().Struct.TotalVoltage / 10, constBmsDriver.GetLastLiveData().Struct.TotalVoltage % 10);
            constCli.printHeader("Current = %d.%dA\r\n", constBmsDriver.GetLastLiveData().Struct.Current / 10, constBmsDriver.GetLastLiveData().Struct.Current % 10);
            constCli.printHeader("Remain capacity = %3.6fA*h\r\n", constBmsDriver.GetLastLiveData().Struct.RemainCapacity / 1000000.0);
            constCli.printHeader("Charge = %d\r\n", constBmsDriver.GetLastLiveData().Struct.SystemLogs.Struct.Charge);
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
    if (_bmsDriver.GetValidOfLastLiveData())
    {
        if (_bmsDriver.GetLastLiveData().Struct.SystemLogs.Struct.Charge && (HAL_GetTick() - lastBlinkTime > 1000))
        {
            _chargeBlink = !_chargeBlink;
            lastBlinkTime = HAL_GetTick();
        }
        else if (!_bmsDriver.GetLastLiveData().Struct.SystemLogs.Struct.Charge)
        {
            _chargeBlink = false;
        }

        if (_bmsDriver.GetLastLiveData().Struct.SOC >= 10 && _bmsDriver.GetLastLiveData().Struct.SOC < 25)
            _dac.SetValue(dacTable[1 + _chargeBlink]);
        else if (_bmsDriver.GetLastLiveData().Struct.SOC >= 25 && _bmsDriver.GetLastLiveData().Struct.SOC < 50)
            _dac.SetValue(dacTable[2 + _chargeBlink]);
        else if (_bmsDriver.GetLastLiveData().Struct.SOC >= 50 && _bmsDriver.GetLastLiveData().Struct.SOC < 75)
            _dac.SetValue(dacTable[3 + _chargeBlink]);
        else if (_bmsDriver.GetLastLiveData().Struct.SOC >= 75 && _bmsDriver.GetLastLiveData().Struct.SOC <= 100)
            _dac.SetValue(dacTable[4 - _chargeBlink]);
        else
            _dac.SetValue(dacTable[0 + _chargeBlink]);
    }
}
